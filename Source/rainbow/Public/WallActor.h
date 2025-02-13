#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallActor.generated.h"

UCLASS()
class RAINBOW_API AWallActor : public AActor
{
    GENERATED_BODY()

public:
    AWallActor();

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
    UStaticMeshComponent* WallMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    TArray<UMaterial*> Materials;

    UFUNCTION()
    void ChangeColor(int32 WaveIndex); // 색 변경 함수
};
