#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SpikeTrap.generated.h"

UCLASS()
class RAINBOW_API ASpikeTrap : public ABaseItem
{
	GENERATED_BODY()

private:
	FName ItemType;

public:
	ASpikeTrap();

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spike")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spike")
	float CurrentTime;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spike")
	float IntervalTime;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spike")
	float MovingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spike")
	FVector Direction;

	virtual void Tick(float DeltaTime) override;
	virtual void OnObjectOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual FName GetObjectType() const override;
	
};
