#include "SlowItem.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ASlowItem::ASlowItem()
{
	SlowedPlayer = nullptr;
	BuffDuration = 2.0f;
}

void ASlowItem::ActivateObject(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(Activator))
		{
			// �÷��̾��� ApplySlow() �Լ� ȣ�� �� ���� Ÿ�̸� ���� & ���ο� ���� �ð� ����
			PlayerCharacter->ApplySlow(BuffDuration);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
		}
	}
}
