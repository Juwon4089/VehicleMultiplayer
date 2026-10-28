// CXSpeedBuffItem.h

#pragma once

#include "CoreMinimal.h"
#include "CXItemBase.h"
#include "CXSpeedBuffItem.generated.h"

/**
 *  Speed buff item. Raises the bonus max speed of the player who picks it up.
 */
UCLASS()
class CX_API ACXSpeedBuffItem : public ACXItemBase
{
	GENERATED_BODY()

public:

	ACXSpeedBuffItem();

protected:

	virtual void ApplyItemEffect(ACXPlayerState* TargetPlayerState) override;

	/** Bonus max speed given on pickup */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	float BonusMaxSpeedAmount = 10.0f;
};
