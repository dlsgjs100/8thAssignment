#include "AttackItem.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AAttackItem::AAttackItem()
{
	SpeededPlayer = nullptr;
	BuffDuration = 10.0f;
}

void AAttackItem::ActivateObject(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Activator))
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				BuffDuration,
				FColor::Green,
				TEXT("Player speeded!!"));

			PlayerCharacter->ApplyAttackEffect(BuffDuration);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
		}
	}
}