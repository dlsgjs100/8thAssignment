#include "ReverseControlItem.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AReverseControlItem::AReverseControlItem()
{
	ReversedPlayer = nullptr;
	BuffDuration = 2.0f;
}

void AReverseControlItem::ActivateObject(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Activator))
		{
			PlayerCharacter->ApplyReverseControl(BuffDuration);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
		}
	}
}

