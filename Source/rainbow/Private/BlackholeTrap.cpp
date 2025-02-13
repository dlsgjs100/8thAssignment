#include "BlackholeTrap.h"
#include "Components/SphereComponent.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameState.h"

ABlackholeTrap::ABlackholeTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsScalingUp = true;
	bIsScalingDown = false;
	Damage = 20.0f;
}

void ABlackholeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsScalingUp)
	{
		// ũ�� ����
		FVector TargetScale = FVector(3.0f); // 3�� ũ��� ����
		FVector NewScale = FMath::VInterpTo(GetActorScale3D(), TargetScale, DeltaTime, 2.0f);
		SetActorScale3D(NewScale);

		if (GetActorScale3D().Equals(TargetScale, 0.01f))
		{
			bIsScalingUp = false;
			FTimerHandle ScaleTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				ScaleTimerHandle,
				this, 
				&ABlackholeTrap::StartScalingDown,
				1.0f, 
				false);
		}
	}
	else if (bIsScalingDown)
	{
		// ũ�� ����
		FVector TargetScale = FVector::ZeroVector; // ���� ũ��
		FVector NewScale = FMath::VInterpTo(GetActorScale3D(), TargetScale, DeltaTime, 4.0f);
		SetActorScale3D(NewScale);

		if (GetActorScale3D().Equals(TargetScale, 0.01f))
		{
			bIsScalingDown = false;
			DestroyItem();
		}
	}
}

void ABlackholeTrap::StartScalingUp()
{
	bIsScalingUp = true;
}

void ABlackholeTrap::StartScalingDown()
{
	bIsScalingDown = true;
}

void ABlackholeTrap::OnObjectOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, 
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	TArray<AActor*> OverlappingActors;
	Collision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (OtherActor && OtherActor->ActorHasTag("Player"))
		{
			if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(OtherActor))
			{
				UGameplayStatics::ApplyDamage(
					Actor,
					Damage,
					nullptr,
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
}

FName ABlackholeTrap::GetObjectType() const
{
	return ItemType;
}
