#include "SpeedItem.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ASpeedItem::ASpeedItem()
{
	SpeededPlayer = nullptr;
	BuffDuration = 5.0f;
}

void ASpeedItem::ActivateObject(AActor* Activator)
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

			PlayerCharacter->ApplySpeed(BuffDuration);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
		}
	}
}
