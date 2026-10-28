// CXPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CXPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class ACXPawn;
class ACXPlayerState;
class UCXUI;
class UCXChatWidget;
class UCXStatusWidget;
struct FInputActionValue;

/**
 *  Vehicle Player Controller class
 *  Handles input mapping and user interface
 */
UCLASS(abstract)
class CX_API ACXPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	/** If true, the optional steering wheel input mapping context will be registered */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bUseSteeringWheelControls = false;

	/** Optional Input Mapping Context to be used for steering wheel input.
	 *  This is added alongside the default Input Mapping Context and does not block other forms of input.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(EditCondition="bUseSteeringWheelControls"))
	UInputMappingContext* SteeringWheelInputMappingContext;

	/** Opens the chat input. If not set, an action bound to Enter is created at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ChatAction;

	/** Pointer to the controlled vehicle pawn */
	UPROPERTY(Replicated)
	TObjectPtr<ACXPawn> VehiclePawn;

	/** Type of the UI to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UCXUI> VehicleUIClass;

	/** Pointer to the UI widget */
	TObjectPtr<UCXUI> VehicleUI;

	/** Type of the chat widget to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UCXChatWidget> ChatWidgetClass;

	/** Type of the coins / bonus speed widget to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UCXStatusWidget> StatusWidgetClass;

	UPROPERTY()
	TObjectPtr<UCXChatWidget> ChatWidget;

	UPROPERTY()
	TObjectPtr<UCXStatusWidget> StatusWidget;

	/** Longest chat message the server accepts */
	UPROPERTY(EditDefaultsOnly, Category = Chat)
	int32 MaxChatMessageLength = 100;

	/** Minimum seconds between two chat messages (checked on the server) */
	UPROPERTY(EditDefaultsOnly, Category = Chat)
	float MinChatInterval = 0.3f;

public:

	ACXPlayerController();

	// Begin Actor interface
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:

	virtual void Tick(float Delta) override;

	// End Actor interface

	// Begin PlayerController interface
protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;

	// End PlayerController interface

public:

	/** Local. Sends a chat message typed in the chat widget to the server */
	void SendChatMessage(const FString& Message);

	/** Local. Adds a chat line to this player's chat widget */
	void AddChatMessage(const FString& SenderName, const FString& Message, bool bIsSystemMessage);

	/** Local. Focuses the chat input box */
	void StartChatting();

	/** Local. Gives input back to the vehicle */
	void StopChatting();

protected:

	/** Client -> Server. Validates the message and asks the game mode to broadcast it */
	UFUNCTION(Server, Reliable)
	void Server_SendChatMessage(const FString& Message);

	/** Handles chat input */
	void OnChatAction(const FInputActionValue& Value);

private:

	void CreateWidgets();

	/** Subscribes the status widget to the player state's change delegates */
	void BindPlayerState();

	void HandleCoinsChanged(int32 NewCoins);
	void HandleBonusMaxSpeedChanged(float NewBonusMaxSpeed);

	/** Runtime mapping context used when ChatAction is not set */
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> RuntimeChatMappingContext;

	TWeakObjectPtr<ACXPlayerState> BoundPlayerState;

	/** Server side time of the last accepted chat message */
	float LastChatTime = -1000.0f;

	bool bIsChatting = false;
};
