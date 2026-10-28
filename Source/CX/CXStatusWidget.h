// CXStatusWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CXStatusWidget.generated.h"

class UTextBlock;

/**
 *  Shows the local player's coins and bonus max speed.
 *  If no layout is authored in a Blueprint subclass, a default layout is built in C++.
 *  A Blueprint subclass can provide its own layout by naming widgets CoinsText / BonusMaxSpeedText.
 */
UCLASS()
class CX_API UCXStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetCoins(int32 NewCoins);

	void SetBonusMaxSpeed(float NewBonusMaxSpeed);

protected:

	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CoinsText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> BonusMaxSpeedText;

private:

	void BuildDefaultLayout();
};
