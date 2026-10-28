// CXPlayerState.cpp

#include "CXPlayerState.h"
#include "CXPawn.h"
#include "Net/UnrealNetwork.h"

void ACXPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only the owner needs to see its own coins and bonus speed
	DOREPLIFETIME_CONDITION(ThisClass, Coins, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, BonusMaxSpeed, COND_OwnerOnly);
}

bool ACXPlayerState::CheckAuthority(const TCHAR* FunctionName) const
{
	if (HasAuthority() == false)
	{
		UE_LOG(LogTemplateVehicle, Warning, TEXT("%s: %s can only be called on the server."), *GetNameSafe(this), FunctionName);
		return false;
	}

	return true;
}

void ACXPlayerState::AddCoins(int32 Amount)
{
	if (CheckAuthority(TEXT("AddCoins")) == false || Amount <= 0)
	{
		return;
	}

	Coins += Amount;

	// OnRep is not called on the server, so notify the server side listeners directly
	OnRep_Coins();
}

bool ACXPlayerState::RemoveCoins(int32 Amount)
{
	if (CheckAuthority(TEXT("RemoveCoins")) == false || Amount <= 0 || Coins < Amount)
	{
		return false;
	}

	Coins -= Amount;
	OnRep_Coins();

	return true;
}

void ACXPlayerState::AddBonusMaxSpeed(float Amount)
{
	if (CheckAuthority(TEXT("AddBonusMaxSpeed")) == false || Amount <= 0.0f)
	{
		return;
	}

	BonusMaxSpeed += Amount;
	OnRep_BonusMaxSpeed();
}

void ACXPlayerState::OnRep_Coins()
{
	OnCoinsChanged.Broadcast(Coins);
}

void ACXPlayerState::OnRep_BonusMaxSpeed()
{
	OnBonusMaxSpeedChanged.Broadcast(BonusMaxSpeed);
}
