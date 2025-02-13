#include "SpikeSpawner.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "MyGameState.h"

ASpikeSpawner::ASpikeSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	bShoulSpawn = false;
	CurrentTime = 0.0f;
	SpawnIntervalTime = 2.0f;
}

FVector ASpikeSpawner::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	// Volume의 안의 랜덤지점을 반환
	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		0
	);
}

void ASpikeSpawner::SpawnSpike()
{
	if (GetWorld() && ActorToSpawn)
	{
		FActorSpawnParameters SpawnParams;
		FVector SpawnLocation = GetRandomPointInVolume();
		FRotator SpawnRotation = GetActorRotation();

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
			ActorToSpawn,
			SpawnLocation,
			SpawnRotation
		);
	}
}

void ASpikeSpawner::Tick(float DeltaTime)
{
	AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
	if (MyGameState)
	{
		bShoulSpawn = MyGameState->CurrentWaveIndex > 0 ? true : false;
	}
	if (bShoulSpawn)
	{
		CurrentTime += DeltaTime;
		if (CurrentTime > SpawnIntervalTime)
		{
			SpawnSpike();
			CurrentTime = 0.0f;
		}
	}
}
