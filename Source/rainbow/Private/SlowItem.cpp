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
			// 플레이어의 ApplySlow() 함수 호출 → 기존 타이머 리셋 & 새로운 지속 시간 적용
			PlayerCharacter->ApplySlow(BuffDuration);

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
		}
	}
}
