#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BlackholeTrap.generated.h"

UCLASS()
class RAINBOW_API ABlackholeTrap : public ABaseItem
{
	GENERATED_BODY()

private:
	FName ItemType;

public:
	ABlackholeTrap();

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Blackhole")
	float Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackhole")
	bool bIsScalingUp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackhole")
	bool bIsScalingDown;

	virtual void Tick(float DeltaTime) override;

	void StartScalingUp();
	void StartScalingDown();

	virtual void OnObjectOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual FName GetObjectType() const override;
};
