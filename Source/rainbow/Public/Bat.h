#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ObjectInterface.h"
#include "Bat.generated.h"

class UWidgetComponent;
class USphereComponent;
UCLASS()
class RAINBOW_API ABat : public APawn, public IObjectInterface
{
	GENERATED_BODY()

public:
	ABat();

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

protected:
	// 처치시 얻는 점수
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Bat")
	int32 PointValue;
	// 피격시 데미지
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Bat")
	float Damage;
	// Bat의 체력
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Bat")
	float MaxHealth;
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat|Component")
	USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat|Component")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Bat|Movement");
	float RotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat|Movement");
	FVector Direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat|Movement");
	float MoveSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat|Movement");
	float TimeElapsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat|Movement");
	float SwitchInterval;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Bat|Movement");
	bool bIsMovingForward;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat|Movement");
	FRotator CurrentRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat|Type")
	FName ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat|Type")
	FName ColorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bHasDealtDamage;

	UFUNCTION()
	virtual void OnObjectOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void OnObjectEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual FName GetObjectType() const override;

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser);

	virtual void OnDeath();
	void UpdateOverheadHP();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void MoveBat(float DeltaTime);
	void RotateBat(float DeltaTime);
};
