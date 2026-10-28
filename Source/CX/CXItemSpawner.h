// CXItemSpawner.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CXItemSpawner.generated.h"

class ACXItemBase;

/**
 *  Placed in the level. Spawns one item on the server and,
 *  when that item is destroyed, spawns the same kind of item again after a delay.
 */
UCLASS()
class CX_API ACXItemSpawner : public AActor
{
	GENERATED_BODY()

public:

	ACXItemSpawner();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Server. Spawns ItemClass at the spawner location */
	void SpawnItem();

	/** Bound to the spawned item's OnDestroyed delegate */
	UFUNCTION()
	void OnSpawnedItemDestroyed(AActor* DestroyedActor);

	/** Item to spawn (coin or speed buff) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawner)
	TSubclassOf<ACXItemBase> ItemClass;

	/** Seconds to wait before respawning after the item was destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawner)
	float RespawnDelay = 2.0f;

	/** Item currently spawned by this spawner */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = Spawner)
	TObjectPtr<ACXItemBase> SpawnedItem;

private:

	FTimerHandle RespawnTimerHandle;
};
