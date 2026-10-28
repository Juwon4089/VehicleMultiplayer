// CXCoinItem.h

#pragma once

#include "CoreMinimal.h"
#include "CXItemBase.h"
#include "CXCoinItem.generated.h"

/**
 *  Coin item. Gives coins to the player who picks it up.
 */
UCLASS()
class CX_API ACXCoinItem : public ACXItemBase
{
	GENERATED_BODY()

public:

	ACXCoinItem();

protected:

	virtual void ApplyItemEffect(ACXPlayerState* TargetPlayerState) override;

	/** Coins given on pickup */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 CoinAmount = 10;
};
