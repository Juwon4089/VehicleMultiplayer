// CXGameState.cpp

#include "CXGameState.h"
#include "CXPlayerController.h"
#include "Engine/World.h"

void ACXGameState::Multicast_BroadcastChatMessage_Implementation(const FString& SenderName, const FString& Message, bool bIsSystemMessage)
{
	// a dedicated server has no screen to draw on
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACXPlayerController* PlayerController = Cast<ACXPlayerController>(It->Get());

		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->AddChatMessage(SenderName, Message, bIsSystemMessage);
		}
	}
}
