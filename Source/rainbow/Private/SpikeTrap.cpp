#include "SpikeTrap.h"
#include "Components/SphereComponent.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameState.h"

ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	Direction = FVector::UpVector;
	CurrentTime = 0.0f;
	IntervalTime = 1.2f;
	MovingSpeed = 100.0f;
	Damage = 20.0f;
}

void ASpikeTrap::Tick(float DeltaTime)
{
	FVector Offset;
	CurrentTime += DeltaTime;
	if (CurrentTime > IntervalTime)
	{
		CurrentTime = 0;
		Direction *= -1;
	}
	Offset = Direction * MovingSpeed * DeltaTime;
	AddActorLocalOffset(Offset);
}

void ASpikeTrap::OnObjectOverlap(
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
			// 플레이어와 충돌시 데미지
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Spike Trap Damaged!!")));
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

FName ASpikeTrap::GetObjectType() const
{
	return ItemType;
}
