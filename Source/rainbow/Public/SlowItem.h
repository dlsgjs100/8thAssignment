#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowItem.generated.h"

class AMyCharacter;
UCLASS()
class RAINBOW_API ASlowItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ASlowItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Slow")
	AMyCharacter* SlowedPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slow")
	float BuffDuration;

	void ActivateObject(AActor* Activator) override;
};
