// CXChatWidget.cpp

#include "CXChatWidget.h"
#include "CXPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UCXChatWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// the Blueprint subclass has no ChatScrollBox / ChatInput, build the default layout
	if (WidgetTree && (ChatScrollBox == nullptr || ChatInput == nullptr))
	{
		BuildDefaultLayout();
	}

	if (ChatInput)
	{
		ChatInput->OnTextCommitted.AddDynamic(this, &UCXChatWidget::OnInputCommitted);
	}
}

void UCXChatWidget::BuildDefaultLayout()
{
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
	Root->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WidgetTree->RootWidget = Root;

	// bottom left: chat log + input box
	UBorder* ChatBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ChatBorder"));
	ChatBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.45f));
	ChatBorder->SetPadding(FMargin(10.0f));

	UCanvasPanelSlot* ChatSlot = Root->AddChildToCanvas(ChatBorder);
	ChatSlot->SetAnchors(FAnchors(0.0f, 1.0f));
	ChatSlot->SetAlignment(FVector2D(0.0f, 1.0f));
	ChatSlot->SetPosition(FVector2D(24.0f, -24.0f));
	ChatSlot->SetSize(FVector2D(520.0f, 280.0f));

	UVerticalBox* ChatBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ChatBox"));
	ChatBorder->SetContent(ChatBox);

	ChatScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ChatScrollBox"));
	UVerticalBoxSlot* ScrollSlot = ChatBox->AddChildToVerticalBox(ChatScrollBox);
	ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ScrollSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));

	ChatInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("ChatInput"));
	ChatInput->SetHintText(FText::FromString(TEXT("Enter: 채팅 입력  |  좌클릭: 공격")));
	ChatBox->AddChildToVerticalBox(ChatInput);
}

void UCXChatWidget::AddChatMessage(const FString& Message, const FLinearColor& Color)
{
	if (IsValid(ChatScrollBox) == false)
	{
		return;
	}

	UTextBlock* Line = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Line->SetText(FText::FromString(Message));
	Line->SetColorAndOpacity(FSlateColor(Color));
	Line->SetAutoWrapText(true);

	FSlateFontInfo Font = Line->GetFont();
	Font.Size = 14;
	Line->SetFont(Font);

	if (UScrollBoxSlot* LineSlot = Cast<UScrollBoxSlot>(ChatScrollBox->AddChild(Line)))
	{
		LineSlot->SetPadding(FMargin(0.0f, 1.0f));
	}

	while (ChatScrollBox->GetChildrenCount() > MaxChatLines)
	{
		ChatScrollBox->RemoveChildAt(0);
	}

	ChatScrollBox->ScrollToEnd();
}

void UCXChatWidget::StartTyping()
{
	if (ChatInput)
	{
		ChatInput->SetKeyboardFocus();
	}
}

void UCXChatWidget::ClearInput()
{
	if (ChatInput)
	{
		ChatInput->SetText(FText::GetEmpty());
	}
}

void UCXChatWidget::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	ACXPlayerController* PlayerController = GetOwningPlayer<ACXPlayerController>();
	if (IsValid(PlayerController) == false)
	{
		return;
	}

	// Enter sends the message, Esc (OnCleared) just closes the chat
	if (CommitMethod == ETextCommit::OnEnter)
	{
		PlayerController->SendChatMessage(Text.ToString());
		ClearInput();
		PlayerController->StopChatting();
	}
	else if (CommitMethod == ETextCommit::OnCleared)
	{
		ClearInput();
		PlayerController->StopChatting();
	}
}
