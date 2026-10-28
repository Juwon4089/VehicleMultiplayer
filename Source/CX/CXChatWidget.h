// CXChatWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CXChatWidget.generated.h"

class UScrollBox;
class UEditableTextBox;

/**
 *  Chat window widget (chat log + input box).
 *  If no layout is authored in a Blueprint subclass, a default layout is built in C++.
 *  A Blueprint subclass can provide its own layout by naming widgets ChatScrollBox / ChatInput.
 */
UCLASS()
class CX_API UCXChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Adds one line to the chat log */
	void AddChatMessage(const FString& Message, const FLinearColor& Color);

	/** Focuses the input box so the player can type */
	void StartTyping();

	/** Clears the input box */
	void ClearInput();

protected:

	virtual void NativeOnInitialized() override;

	/** Called when Enter (or Esc) is pressed in the input box */
	UFUNCTION()
	void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UScrollBox> ChatScrollBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UEditableTextBox> ChatInput;

	/** Oldest lines are removed beyond this count */
	UPROPERTY(EditDefaultsOnly, Category = Chat)
	int32 MaxChatLines = 50;

private:

	void BuildDefaultLayout();
};
