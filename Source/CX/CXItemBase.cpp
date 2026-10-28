// CXItemBase.cpp

#include "CXItemBase.h"
#include "CXPawn.h"
#include "CXPlayerState.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ACXItemBase::ACXItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(false);

	// items never move, so one update per second is plenty
	SetNetUpdateFrequency(1.0f);

	// always relevant, so a client only sees the item destroyed when it was really picked up
	bAlwaysRelevant = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(120.0f);
	Collision->SetCollisionProfileName(FName("OverlapAllDynamic"));
	Collision->SetGenerateOverlapEvents(true);
	RootComponent = Collision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.0f, 90.0f, 0.0f);

	// default effects from the starter content. Can be changed in the Blueprint
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PickupParticleFinder(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (PickupParticleFinder.Succeeded())
	{
		PickupParticle = PickupParticleFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundFinder(TEXT("/Game/StarterContent/Audio/Explosion_Cue.Explosion_Cue"));
	if (PickupSoundFinder.Succeeded())
	{
		PickupSound = PickupSoundFinder.Object;
	}
}

void ACXItemBase::BeginPlay()
{
	Super::BeginPlay();

	// pickups are decided by the server only
	if (HasAuthority())
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ACXItemBase::OnItemBeginOverlap);
	}
}

void ACXItemBase::OnItemBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bPickedUp || HasAuthority() == false)
	{
		return;
	}

	ACXPawn* Vehicle = Cast<ACXPawn>(OtherActor);
	if (IsValid(Vehicle) == false)
	{
		return;
	}

	ACXPlayerState* TargetPlayerState = Vehicle->GetPlayerState<ACXPlayerState>();
	if (IsValid(TargetPlayerState) == false)
	{
		return;
	}

	bPickedUp = true;

	ApplyItemEffect(TargetPlayerState);

	Destroy();
}

void ACXItemBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Destroyed is the reason on the server after Destroy() and on clients when the destruction replicates.
	// Level unload / PIE end use other reasons, so no effects play then.
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		PlayPickupEffects();
	}

	Super::EndPlay(EndPlayReason);
}

void ACXItemBase::PlayPickupEffects()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (IsValid(PickupParticle))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, PickupParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(0.5f));
	}

	if (IsValid(PickupSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}
