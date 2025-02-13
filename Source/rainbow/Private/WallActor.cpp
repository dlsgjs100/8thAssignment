#include "WallActor.h"
#include "Components/StaticMeshComponent.h"
#include "MyGameState.h"

AWallActor::AWallActor()
{
	PrimaryActorTick.bCanEverTick = false;

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	RootComponent = WallMesh;
}

void AWallActor::ChangeColor(int32 WaveIndex)
{
	if (AMyGameState* MyGameState = GetWorld()->GetGameState<AMyGameState>())
	{
		if (MyGameState)
		{
			WaveIndex = MyGameState->CurrentWaveIndex;
			if (Materials.Num())
			{
				WallMesh->SetMaterial(0, Materials[WaveIndex]);
			}
		}
	}
}
