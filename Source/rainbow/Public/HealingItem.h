#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealingItem.generated.h"

UCLASS()
class RAINBOW_API AHealingItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AHealingItem();
	
	// È¸º¹·®
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 HealAmount;

	virtual void ActivateObject(AActor* Activator) override;
};
