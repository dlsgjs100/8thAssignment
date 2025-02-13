#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ABlackholeTrap;
struct FInputActionValue;

UCLASS()
class RAINBOW_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	// Camera & SkeletalMesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh;

	// WeaponColors for Swap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorCandidates")
	int32 CurrentColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorCandidates")
	int32 MaxColorCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorCandidates")
	TArray<UMaterial*> ColorsCandidates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorCandidates")
	TArray<FName> ColorsTypes;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ColorCandidates")
	FName CurrentColorType;
	UFUNCTION(BlueprintPure, Category = "ColorCandidates")
	FName GetCurrentColorType() const;

	// AnimMontages for Combo
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Montage")
	UAnimMontage* AttackMontage1;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Montage")
	UAnimMontage* AttackMontage2;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Montage")
	UAnimMontage* AttackMontage3;
	FTimerHandle ComboResetTimerHandle;

	// Variables for Combo Attack
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 ComboCount;
	bool bHasHitMonster = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FName SwordSocketName = "Sword_Mid";  // 애님 소켓 이름
	FVector PreviousSocketLocation; // 이전 소켓 위치
	FTimerHandle ResetHitTimerHandle;

	// Health Getter & AddHealth
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effect")
	TArray<int32> EffectTypes;
	FTimerHandle SlowTimerHandle;
	FTimerHandle SpeedTimerHandle;
	FTimerHandle AttackBoostTimerHandle;
	FTimerHandle ReverseTimerHandle;

	TArray<FTimerHandle> EffectTimerHandle;
	// Slow Effect
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bCanSprint;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bIsSlowed;
	TArray<float> EffectDuration;
	void ApplySlow(float Duration);
	void RemoveSlow();

	// Speed Effect
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bIsSpeeded;
	void ApplySpeed(float Duration);
	void RemoveSpeed();

	// Attack Effect
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bIsAttackEffected;
	void ApplyAttackEffect(float Duration);
	void RemoveAttackEffect();
	
	// Control Reverse Effect
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bIsReverseEffected;
	void ApplyReverseControl(float Duration);
	void RemoveReverseControl();

	void SpawnBlackhole();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	float Health;

	float ElapsedTime;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack(const FInputActionValue& value);
	UFUNCTION()
	void PlayAttackMontage();
	UFUNCTION()
	void EnableInputAfterAttack();
	UFUNCTION()
	void ResetCombo();
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);
	UFUNCTION()
	void SwapColor(const FInputActionValue& value);
	UFUNCTION()
	void ChangeColor();

	void OnDeath();

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABlackholeTrap> BlackholeClass;
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	float CurrentAttack;
};
