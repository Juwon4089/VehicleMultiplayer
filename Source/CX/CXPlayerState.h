// CXPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CXPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCoinsChanged, int32 /*NewCoins*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBonusMaxSpeedChanged, float /*NewBonusMaxSpeed*/);

/**
 *  Player State
 *  Holds per-player data (coins, bonus max speed).
 *  Values are changed on the server only and replicated to the owning client only.
 */
UCLASS()
class CX_API ACXPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Server only. Adds coins */
	void AddCoins(int32 Amount);

	/** Server only. Removes coins. Returns false (and changes nothing) if there are not enough coins */
	bool RemoveCoins(int32 Amount);

	/** Server only. Adds bonus max speed */
	void AddBonusMaxSpeed(float Amount);

	FORCEINLINE int32 GetCoins() const { return Coins; }
	FORCEINLINE float GetBonusMaxSpeed() const { return BonusMaxSpeed; }

	/** Broadcast on the server when changed, and on the owning client on replication */
	FOnCoinsChanged OnCoinsChanged;
	FOnBonusMaxSpeedChanged OnBonusMaxSpeedChanged;

protected:

	UFUNCTION()
	void OnRep_Coins();

	UFUNCTION()
	void OnRep_BonusMaxSpeed();

private:

	/** Returns true on the server. Logs a warning otherwise */
	bool CheckAuthority(const TCHAR* FunctionName) const;

	/** Coins collected by this player */
	UPROPERTY(ReplicatedUsing = OnRep_Coins, VisibleInstanceOnly, Category = Stats)
	int32 Coins = 0;

	/** Extra max speed collected by this player */
	UPROPERTY(ReplicatedUsing = OnRep_BonusMaxSpeed, VisibleInstanceOnly, Category = Stats)
	float BonusMaxSpeed = 0.0f;
};
