// CXItemSpawner.cpp

#include "CXItemSpawner.h"
#include "CXItemBase.h"
#include "Components/BillboardComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACXItemSpawner::ACXItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// the spawner only works on the server, the spawned items replicate by themselves
	bReplicates = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

#if WITH_EDITORONLY_DATA
	if (UBillboardComponent* Billboard = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Billboard")))
	{
		Billboard->SetupAttachment(RootComponent);
	}
#endif
}

void ACXItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnItem();
	}
}

void ACXItemSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(RespawnTimerHandle);

	// the item may outlive the spawner, do not get called back into a dead spawner
	if (IsValid(SpawnedItem))
	{
		SpawnedItem->OnDestroyed.RemoveDynamic(this, &ACXItemSpawner::OnSpawnedItemDestroyed);
	}

	Super::EndPlay(EndPlayReason);
}

void ACXItemSpawner::SpawnItem()
{
	if (HasAuthority() == false || IsValid(ItemClass) == false || IsValid(SpawnedItem))
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedItem = GetWorld()->SpawnActor<ACXItemBase>(ItemClass, GetActorTransform(), SpawnParams);

	if (IsValid(SpawnedItem))
	{
		SpawnedItem->OnDestroyed.AddDynamic(this, &ACXItemSpawner::OnSpawnedItemDestroyed);
	}
}

void ACXItemSpawner::OnSpawnedItemDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor != SpawnedItem)
	{
		return;
	}

	SpawnedItem = nullptr;

	UWorld* World = GetWorld();
	if (IsValid(World) == false || World->bIsTearingDown)
	{
		return;
	}

	// same kind of item comes back after RespawnDelay seconds
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ACXItemSpawner::SpawnItem, RespawnDelay, false);
}
