#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SpeedItem.generated.h"

class AMyCharacter;
UCLASS()
class RAINBOW_API ASpeedItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ASpeedItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Speed")
	AMyCharacter* SpeededPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float BuffDuration;

	void ActivateObject(AActor* Activator) override;
};
