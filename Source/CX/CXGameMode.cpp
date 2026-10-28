// CXGameMode.cpp

#include "CXGameMode.h"
#include "CXPlayerController.h"
#include "CXPlayerState.h"
#include "CXGameState.h"
#include "CXPawn.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

ACXGameMode::ACXGameMode()
{
	PlayerControllerClass = ACXPlayerController::StaticClass();
	PlayerStateClass = ACXPlayerState::StaticClass();
	GameStateClass = ACXGameState::StaticClass();
}

void ACXGameMode::HandleChatMessage(ACXPlayerController* Sender, const FString& Message)
{
	if (IsValid(Sender) == false || IsValid(Sender->PlayerState) == false)
	{
		return;
	}

	BroadcastChatMessage(Sender->PlayerState->GetPlayerName(), Message, false);
}

void ACXGameMode::HandleVehicleAttack(AController* Attacker, ACXPawn* Victim)
{
	if (IsValid(Attacker) == false || IsValid(Victim) == false)
	{
		return;
	}

	ACXPlayerState* AttackerState = Attacker->GetPlayerState<ACXPlayerState>();
	ACXPlayerState* VictimState = Victim->GetPlayerState<ACXPlayerState>();

	if (IsValid(AttackerState) == false || IsValid(VictimState) == false || AttackerState == VictimState)
	{
		return;
	}

	// the victim has less than the stolen amount, nothing happens
	if (VictimState->RemoveCoins(CoinsStolenPerHit) == false)
	{
		return;
	}

	AttackerState->AddCoins(CoinsStolenPerHit);

	BroadcastChatMessage(
		FString(),
		FString::Printf(TEXT("%s 님이 %s 님에게서 %d원을 빼앗았습니다!"), *AttackerState->GetPlayerName(), *VictimState->GetPlayerName(), CoinsStolenPerHit),
		true);
}

void ACXGameMode::BroadcastChatMessage(const FString& SenderName, const FString& Message, bool bIsSystemMessage)
{
	if (ACXGameState* CXGameState = GetGameState<ACXGameState>())
	{
		CXGameState->Multicast_BroadcastChatMessage(SenderName, Message, bIsSystemMessage);
	}
}

AActor* ACXGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> AvailablePlayerStarts;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (IsPlayerStartAvailable(PlayerStart))
		{
			AvailablePlayerStarts.Add(PlayerStart);
		}
	}

	checkf(AvailablePlayerStarts.IsEmpty() == false, TEXT("There is no available Vehicle PlayerStart."));

	const int32 RandomIndex = FMath::RandHelper(AvailablePlayerStarts.Num());
	return Cast<AActor>(AvailablePlayerStarts[RandomIndex]);
}

bool ACXGameMode::IsPlayerStartAvailable(const APlayerStart* PlayerStart) const
{
	if (IsValid(PlayerStart) == false || IsValid(GetWorld()) == false)
	{
		return false;
	}

	const FTransform PlayerStartTransform = PlayerStart->GetActorTransform();
	const FVector CheckLocation = PlayerStartTransform.TransformPosition(VehicleSpawnCheckOffset);
	const FQuat CheckRotation = PlayerStartTransform.GetRotation();
	const FCollisionShape CheckShape = FCollisionShape::MakeBox(VehicleSpawnCheckHalfExtent);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);

	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VehiclePlayerStartCheck), false);

	const bool bVehicleExists =
		GetWorld()->OverlapAnyTestByObjectType(
			CheckLocation,
			CheckRotation,
			ObjectQueryParams,
			CheckShape,
			QueryParams
		);

	return !bVehicleExists;
}