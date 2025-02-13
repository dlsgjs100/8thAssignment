#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectInterface.h"
#include "BaseItem.generated.h"

class USphereComponent;

UCLASS()
class RAINBOW_API ABaseItem : public AActor, public IObjectInterface
{
	GENERATED_BODY()

public:
	ABaseItem();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemType;
	
	// 아이템이 회전하는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Rotate")
	float RotationSpeed;

	virtual void OnObjectOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	
	virtual FName GetObjectType() const override;
	virtual void ActivateObject(AActor* Activator) ;

	virtual void Tick(float DeltaTime) override;
	void RotateActor(float DeltaTime);

	virtual void DestroyItem();
};
