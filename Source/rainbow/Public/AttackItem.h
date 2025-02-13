#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "AttackItem.generated.h"

class AMyCharacter;
UCLASS()
class RAINBOW_API AAttackItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AAttackItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AttackItem")
	AMyCharacter* SpeededPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackItem")
	float BuffDuration;

	void ActivateObject(AActor* Activator) override;
};
