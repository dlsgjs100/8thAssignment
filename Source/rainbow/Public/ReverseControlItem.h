#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ReverseControlItem.generated.h"

class AMyCharacter;
UCLASS()
class RAINBOW_API AReverseControlItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AReverseControlItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Slow")
	AMyCharacter* ReversedPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slow")
	float BuffDuration;

	void ActivateObject(AActor* Activator) override;
};
