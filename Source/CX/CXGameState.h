// CXGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CXGameState.generated.h"

/**
 *  Game State
 *  Exists on the server and every client, so it is used to broadcast chat to everyone.
 */
UCLASS()
class CX_API ACXGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	/** Server -> all clients. Shows a chat line on every local player's screen */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BroadcastChatMessage(const FString& SenderName, const FString& Message, bool bIsSystemMessage);
};
