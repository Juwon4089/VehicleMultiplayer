// CXPlayerController.cpp


#include "CXPlayerController.h"
#include "CXPawn.h"
#include "CXUI.h"
#include "CXChatWidget.h"
#include "CXStatusWidget.h"
#include "CXPlayerState.h"
#include "CXGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Net/UnrealNetwork.h"

ACXPlayerController::ACXPlayerController()
{
	ChatWidgetClass = UCXChatWidget::StaticClass();
	StatusWidgetClass = UCXStatusWidget::StaticClass();
}

void ACXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	// spawn the UI widget and add it to the viewport
	VehicleUI = CreateWidget<UCXUI>(this, VehicleUIClass);

	check(VehicleUI);

	VehicleUI->AddToViewport();

	CreateWidgets();
	BindPlayerState();
}

void ACXPlayerController::CreateWidgets()
{
	if (IsValid(ChatWidgetClass))
	{
		ChatWidget = CreateWidget<UCXChatWidget>(this, ChatWidgetClass);
		ChatWidget->AddToViewport(1);
	}

	if (IsValid(StatusWidgetClass))
	{
		StatusWidget = CreateWidget<UCXStatusWidget>(this, StatusWidgetClass);
		StatusWidget->AddToViewport(1);
	}
}

void ACXPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);

		// optionally add the steering wheel context
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}

		// no chat action asset assigned, create one bound to Enter
		if (ChatAction == nullptr)
		{
			ChatAction = NewObject<UInputAction>(this, TEXT("IA_Chat_Runtime"));
			ChatAction->ValueType = EInputActionValueType::Boolean;

			RuntimeChatMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Chat_Runtime"));
			RuntimeChatMappingContext->MapKey(ChatAction, EKeys::Enter);

			Subsystem->AddMappingContext(RuntimeChatMappingContext, 1);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ChatAction, ETriggerEvent::Started, this, &ACXPlayerController::OnChatAction);
	}
}

void ACXPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, VehiclePawn);
}

void ACXPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
	}
}

void ACXPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// get a pointer to the controlled pawn
	VehiclePawn = CastChecked<ACXPawn>(InPawn);
}

void ACXPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// on clients the player state may arrive after BeginPlay
	BindPlayerState();
}

void ACXPlayerController::BindPlayerState()
{
	ACXPlayerState* CXPlayerState = GetPlayerState<ACXPlayerState>();

	if (IsLocalController() == false || IsValid(CXPlayerState) == false || BoundPlayerState == CXPlayerState)
	{
		return;
	}

	BoundPlayerState = CXPlayerState;

	CXPlayerState->OnCoinsChanged.AddUObject(this, &ACXPlayerController::HandleCoinsChanged);
	CXPlayerState->OnBonusMaxSpeedChanged.AddUObject(this, &ACXPlayerController::HandleBonusMaxSpeedChanged);

	// show the values that were already replicated
	HandleCoinsChanged(CXPlayerState->GetCoins());
	HandleBonusMaxSpeedChanged(CXPlayerState->GetBonusMaxSpeed());
}

void ACXPlayerController::HandleCoinsChanged(int32 NewCoins)
{
	if (IsValid(StatusWidget))
	{
		StatusWidget->SetCoins(NewCoins);
	}
}

void ACXPlayerController::HandleBonusMaxSpeedChanged(float NewBonusMaxSpeed)
{
	if (IsValid(StatusWidget))
	{
		StatusWidget->SetBonusMaxSpeed(NewBonusMaxSpeed);
	}
}

void ACXPlayerController::OnChatAction(const FInputActionValue& Value)
{
	if (bIsChatting == false)
	{
		StartChatting();
	}
}

void ACXPlayerController::StartChatting()
{
	if (IsValid(ChatWidget) == false)
	{
		return;
	}

	bIsChatting = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	ChatWidget->StartTyping();
}

void ACXPlayerController::StopChatting()
{
	bIsChatting = false;

	SetInputMode(FInputModeGameOnly());
}

void ACXPlayerController::SendChatMessage(const FString& Message)
{
	// first filter on the client. The server checks again
	const FString TrimmedMessage = Message.TrimStartAndEnd();

	if (TrimmedMessage.IsEmpty())
	{
		return;
	}

	Server_SendChatMessage(TrimmedMessage.Left(MaxChatMessageLength));
}

void ACXPlayerController::Server_SendChatMessage_Implementation(const FString& Message)
{
	// never trust the client: empty, too long or spammed messages are rejected here
	const FString SafeMessage = Message.TrimStartAndEnd().Left(MaxChatMessageLength);

	if (SafeMessage.IsEmpty())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastChatTime < MinChatInterval)
	{
		return;
	}

	LastChatTime = CurrentTime;

	if (ACXGameMode* GameMode = GetWorld()->GetAuthGameMode<ACXGameMode>())
	{
		GameMode->HandleChatMessage(this, SafeMessage);
	}
}

void ACXPlayerController::AddChatMessage(const FString& SenderName, const FString& Message, bool bIsSystemMessage)
{
	if (IsValid(ChatWidget) == false)
	{
		return;
	}

	if (bIsSystemMessage)
	{
		ChatWidget->AddChatMessage(FString::Printf(TEXT("[시스템] %s"), *Message), FLinearColor(1.0f, 0.85f, 0.3f));
	}
	else
	{
		ChatWidget->AddChatMessage(FString::Printf(TEXT("%s: %s"), *SenderName, *Message), FLinearColor::White);
	}
}
