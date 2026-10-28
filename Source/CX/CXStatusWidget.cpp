// CXStatusWidget.cpp

#include "CXStatusWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UCXStatusWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// the Blueprint subclass has no CoinsText / BonusMaxSpeedText, build the default layout
	if (WidgetTree && (CoinsText == nullptr || BonusMaxSpeedText == nullptr))
	{
		BuildDefaultLayout();
	}

	SetCoins(0);
	SetBonusMaxSpeed(0.0f);
}

void UCXStatusWidget::BuildDefaultLayout()
{
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
	Root->SetVisibility(ESlateVisibility::HitTestInvisible);
	WidgetTree->RootWidget = Root;

	// top right: coins / bonus max speed
	UBorder* StatusBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("StatusBorder"));
	StatusBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.45f));
	StatusBorder->SetPadding(FMargin(16.0f, 10.0f));

	UCanvasPanelSlot* StatusSlot = Root->AddChildToCanvas(StatusBorder);
	StatusSlot->SetAnchors(FAnchors(1.0f, 0.0f));
	StatusSlot->SetAlignment(FVector2D(1.0f, 0.0f));
	StatusSlot->SetPosition(FVector2D(-24.0f, 24.0f));
	StatusSlot->SetAutoSize(true);

	UVerticalBox* StatusBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StatusBox"));
	StatusBorder->SetContent(StatusBox);

	CoinsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CoinsText"));
	CoinsText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.85f, 0.3f)));
	StatusBox->AddChildToVerticalBox(CoinsText);

	BonusMaxSpeedText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BonusMaxSpeedText"));
	BonusMaxSpeedText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.85f, 1.0f)));
	StatusBox->AddChildToVerticalBox(BonusMaxSpeedText);

	for (UTextBlock* Text : { CoinsText.Get(), BonusMaxSpeedText.Get() })
	{
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = 20;
		Text->SetFont(Font);
	}
}

void UCXStatusWidget::SetCoins(int32 NewCoins)
{
	if (CoinsText)
	{
		CoinsText->SetText(FText::FromString(FString::Printf(TEXT("코인: %d원"), NewCoins)));
	}
}

void UCXStatusWidget::SetBonusMaxSpeed(float NewBonusMaxSpeed)
{
	if (BonusMaxSpeedText)
	{
		BonusMaxSpeedText->SetText(FText::FromString(FString::Printf(TEXT("추가 최대 속도: +%.0f"), NewBonusMaxSpeed)));
	}
}
