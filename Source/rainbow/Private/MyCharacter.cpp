#include "MyCharacter.h"
#include "MyPlayerController.h"
#include "MyGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Bat.h"
#include "BlackholeTrap.h"

enum class EEffectType
{
	None,
	Slow,
	Speed,
	AttackBoost
};

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	SkeletalMesh = GetMesh();
	CurrentColor = 0;
	CurrentColorType = "Ice";

	MaxHealth = 100.0f;
	Health = MaxHealth;

	CurrentAttack = 10.0f;

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	bCanSprint = true;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	EffectTimerHandle = { SlowTimerHandle, SpeedTimerHandle , AttackBoostTimerHandle , ReverseTimerHandle };
	EffectDuration = { 4.0f,5.0f,10.0f ,2.0f};


	ElapsedTime = 0.0f;
}

FName AMyCharacter::GetCurrentColorType() const
{
	return CurrentColorType;
}

float AMyCharacter::GetHealth() const
{
	return Health;
}

float AMyCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

void AMyCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 공격 중일 때만 충돌 체크
	if (bIsAttacking && !bHasHitMonster)
	{
		FVector CurrentSocketLocation = GetMesh()->GetSocketLocation(SwordSocketName);

		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);  // 자기 자신 무시

		FVector StartLocation = PreviousSocketLocation;
		FVector EndLocation = CurrentSocketLocation;

		bool bHit = false;

		for (int32 i = -2; i < 2; ++i) // 여러 개의 라인 트레이스 수행
		{
			FVector Offset = FVector(i * 20.0f, 0, 30.0f); // 공격판정 조절
			FVector Start = StartLocation + Offset;
			FVector End = EndLocation + Offset;

			TArray<FHitResult> TempHitResults; // 각 라인별 결과 저장
			bool bLineHit = GetWorld()->LineTraceMultiByChannel(
				TempHitResults,
				Start,
				End,
				ECC_Pawn,
				QueryParams
			);

			if (bLineHit)
			{
				bHit = true;
				HitResults.Append(TempHitResults); // 여러 개의 라인 트레이스 결과를 누적 저장
			}
		}

		// 충돌이 감지되면
		if (bHit)
		{
			bHasHitMonster = true;

			for (const FHitResult& Hit : HitResults) // 안전한 반복
			{
				AActor* HitActor = Hit.GetActor();
				if (HitActor && HitActor->ActorHasTag("Bat"))
				{
					// 피해 처리
					UGameplayStatics::ApplyDamage(
						HitActor,
						CurrentAttack,
						nullptr,
						this,
						UDamageType::StaticClass()
					);
				}
				break;
			}
		}

		PreviousSocketLocation = CurrentSocketLocation;  // 현재 위치 저장
	}


	ElapsedTime += DeltaTime;
	if (ElapsedTime > 8.0f)
	{
		if (AMyGameState* MyGameState = GetWorld()->GetGameState<AMyGameState>())
		{
			if (MyGameState)
			{
				if (MyGameState->CurrentWaveIndex == 2)
				{
					SpawnBlackhole();
				}
			}
		}
		ElapsedTime = 0.0f;
	}
}

float AMyCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);


	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void AMyCharacter::ApplySlow(float Duration)
{
	if (!bIsSlowed)  // 처음 슬로우가 적용될 때만 실행
	{
		bIsSlowed = true;
		bCanSprint = false;
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
		EffectTypes.Remove(2);
		EffectTypes.Add(1);
	}
	// 기존 타이머 제거 후 새로운 타이머 설정
	GetWorldTimerManager().ClearTimer(EffectTimerHandle[0]);
	GetWorldTimerManager().SetTimer(
		EffectTimerHandle[0],
		this,
		&AMyCharacter::RemoveSlow,
		Duration,
		false);

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowEffect();
		}
	}
}

void AMyCharacter::RemoveSlow()
{
	EffectTypes.Remove(1);
	bIsSlowed = false;
	bCanSprint = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowEffect();
		}
	}
}

void AMyCharacter::ApplySpeed(float Duration)
{
	if (!bIsSpeeded) 
	{
		bIsSpeeded = true;
		GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
		EffectTypes.Remove(1);
		EffectTypes.Add(2);
	}
	GetWorldTimerManager().ClearTimer(EffectTimerHandle[1]);
	GetWorldTimerManager().SetTimer(
		EffectTimerHandle[1],
		this,
		&AMyCharacter::RemoveSpeed,
		Duration,
		false);

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowEffect();
		}
	}
}

void AMyCharacter::RemoveSpeed()
{
	bIsSpeeded = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	EffectTypes.Remove(2);
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowEffect();
		}
	}
}

void AMyCharacter::ApplyAttackEffect(float Duration)
{
	if (!bIsAttackEffected)  
	{
		bIsAttackEffected = true;
		CurrentAttack *= 5.0f;
		EffectTypes.Add(3);
	}
	GetWorldTimerManager().ClearTimer(EffectTimerHandle[2]);
	GetWorldTimerManager().SetTimer(
		EffectTimerHandle[2],
		this,
		&AMyCharacter::RemoveAttackEffect,
		Duration,
		false);
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowEffect();
		}
	}
}

void AMyCharacter::RemoveAttackEffect()
{
	bIsAttackEffected = false;
	CurrentAttack = 10.0f;
	EffectTypes.Remove(3);

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowEffect();
		}
	}
}

void AMyCharacter::ApplyReverseControl(float Duration)
{
	if (!bIsReverseEffected)
	{
		bIsReverseEffected = true;
		EffectTypes.Add(4);
	}
	GetWorldTimerManager().ClearTimer(EffectTimerHandle[3]);
	GetWorldTimerManager().SetTimer(
		EffectTimerHandle[3],
		this,
		&AMyCharacter::RemoveReverseControl,
		Duration,
		false);
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowEffect();
		}
	}
}

void AMyCharacter::RemoveReverseControl()
{
	bIsReverseEffected = false;
	EffectTypes.Remove(4);

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowEffect();
		}
	}
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AMyCharacter::StopJump
				);
			}

			if (PlayerController->AttackAction)
			{
				EnhancedInput->BindAction(
					PlayerController->AttackAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::Attack
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&AMyCharacter::StopSprint
				);
			}

			if (PlayerController->SwapAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SwapAction,
					ETriggerEvent::Triggered,
					this,
					&AMyCharacter::SwapColor
				);
			}
		}
	}
}

void AMyCharacter::SpawnBlackhole()
{
	if (BlackholeClass)
	{
		FVector PlayerLocation = GetActorLocation();
		FVector SpawnLocation;
		SpawnLocation.X = FMath::RandRange(-100.0f, 100.0f);
		SpawnLocation.Y = FMath::RandRange(-100.0f, 100.0f);
		SpawnLocation.Z = FMath::RandRange(0.0f, 180.0f);
		// Blackhole 액터 스폰
		GetWorld()->SpawnActor<ABlackholeTrap>(
			BlackholeClass, 
			PlayerLocation + SpawnLocation,
			FRotator::ZeroRotator);
	}
}

void AMyCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	FVector2D MoveInput = value.Get<FVector2D>();

	if (bIsReverseEffected)
	{
		MoveInput *= -1;
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AMyCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		if (bIsAttacking) return;
		Jump();
	}
}

void AMyCharacter::StopJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		StopJumping();
	}
}

void AMyCharacter::Attack(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		if (bIsAttacking || !CanJump()) return;

		if (GetMesh())
		{
			PreviousSocketLocation = GetMesh()->GetSocketLocation(SwordSocketName);
		}

		ComboCount = (ComboCount + 1) % 4;
		if (ComboCount == 0) ComboCount = 1;

		bIsAttacking = true;
		PlayAttackMontage();

		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			ComboResetTimerHandle,
			this,
			&AMyCharacter::ResetCombo,
			5.0f,
			false);
	}
}

void AMyCharacter::PlayAttackMontage()
{
	UAnimMontage* SelectedMontage = nullptr;

	switch (ComboCount)
	{
	case 1:
		SelectedMontage = AttackMontage1;
		break;
	case 2:
		SelectedMontage = AttackMontage2;
		break;
	case 3:
		SelectedMontage = AttackMontage3;
		break;
	}

	if (SelectedMontage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(SelectedMontage);

		SetActorEnableCollision(false);

		float CanComboTime = SelectedMontage->GetPlayLength() * 0.3f;

		FTimerHandle InputDisableTimer;
		GetWorld()->GetTimerManager().SetTimer(
			InputDisableTimer,
			this,
			&AMyCharacter::EnableInputAfterAttack,
			CanComboTime,
			false);

		bHasHitMonster = false;
	}
}

void AMyCharacter::EnableInputAfterAttack()
{
	SetActorEnableCollision(true);
	bIsAttacking = false;
}

void AMyCharacter::ResetCombo()
{
	ComboCount = 0;
}

void AMyCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AMyCharacter::StartSprint(const FInputActionValue& value)
{
	if (!bCanSprint) return;
	if (bIsSpeeded)
	{
		SprintSpeed = 800.0f * SprintSpeedMultiplier;
	}
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void AMyCharacter::StopSprint(const FInputActionValue& value)
{
	if (!bCanSprint) return;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void AMyCharacter::SwapColor(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		if (ColorsCandidates[CurrentColor])
		{
			ChangeColor();
		}
	}
}

void AMyCharacter::ChangeColor()
{
	CurrentColor++;

	if (AMyGameState* MyGameState = GetWorld()->GetGameState<AMyGameState>())
	{
		if (MyGameState)
		{
			if (MyGameState->CurrentWaveIndex == 0)
			{
				MaxColorCount = 2;
			}
			else if (MyGameState->CurrentWaveIndex == 1)
			{
				MaxColorCount = 4;
			}
			else if (MyGameState->CurrentWaveIndex == 2)
			{
				MaxColorCount = 6;
			}
		}
	}

	if (CurrentColor > MaxColorCount - 1)
	{
		CurrentColor = 0;
	}
	SkeletalMesh->SetMaterial(13, ColorsCandidates[CurrentColor]);
	CurrentColorType = ColorsTypes[CurrentColor];
}

void AMyCharacter::OnDeath()
{
	AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
	if (MyGameState)
	{

		MyGameState->OnGameOver();
	}
}
