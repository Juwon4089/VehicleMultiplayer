// CXGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CXGameMode.generated.h"

class ACXPawn;
class ACXPlayerController;

UCLASS(MinimalAPI)
class ACXGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACXGameMode();

	/** Server. Broadcasts an already validated chat message to every player */
	void HandleChatMessage(ACXPlayerController* Sender, const FString& Message);

	/** Server. Called when Attacker's vehicle hits Victim. Moves coins from the victim to the attacker */
	void HandleVehicleAttack(AController* Attacker, ACXPawn* Victim);

protected:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	bool IsPlayerStartAvailable(const APlayerStart* PlayerStart) const;

	void BroadcastChatMessage(const FString& SenderName, const FString& Message, bool bIsSystemMessage);

private:
	UPROPERTY(EditDefaultsOnly)
	FVector VehicleSpawnCheckHalfExtent = FVector(250.0, 120.0, 80.0);

	UPROPERTY(EditDefaultsOnly)
	FVector VehicleSpawnCheckOffset = FVector(0.0, 0.0, 80.0);

	/** Coins moved from the victim to the attacker per hit */
	UPROPERTY(EditDefaultsOnly, Category = Rules)
	int32 CoinsStolenPerHit = 5;

};
