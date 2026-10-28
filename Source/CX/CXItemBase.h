// CXItemBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CXItemBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class UParticleSystem;
class USoundBase;
class ACXPlayerState;

/**
 *  Base class for pickup items placed in the level.
 *  Overlap is handled on the server only. The item is destroyed after being picked up,
 *  and every machine plays the pickup particle / sound when the destruction is replicated.
 */
UCLASS(abstract)
class CX_API ACXItemBase : public AActor
{
	GENERATED_BODY()

public:

	ACXItemBase();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Server. Gives the item's effect to the player who picked it up */
	virtual void ApplyItemEffect(ACXPlayerState* TargetPlayerState) PURE_VIRTUAL(ACXItemBase::ApplyItemEffect, );

	UFUNCTION()
	void OnItemBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Plays the pickup particle and sound at the item location */
	void PlayPickupEffects();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	TObjectPtr<URotatingMovementComponent> RotatingMovement;

	/** Played when the item is destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TObjectPtr<UParticleSystem> PickupParticle;

	/** Played when the item is destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TObjectPtr<USoundBase> PickupSound;

private:

	/** Prevents the same item from being picked up twice in one frame */
	bool bPickedUp = false;
};
