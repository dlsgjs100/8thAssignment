#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;
UCLASS()
class RAINBOW_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();

    // 스폰 가능 지역
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    USceneComponent* Scene;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    UBoxComponent* SpawningBox;
    // 사용할 현재의 DataTable 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    UDataTable* CurrentObjectDataTable;
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SetCurrentObjectDataTable(UDataTable* SetObjectDataTable);
    // 랜덤 오브젝트 스폰
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    AActor* SpawnRandomObject();
    // 랜덤 스폰지점
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    FVector GetRandomPointInVolume() const;
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    FRotator GetRandomRotation() const;
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    AActor* SpawnObject(TSubclassOf<AActor> ObjectClass);
    FObjectSpawnRow* GetRandomObject() const;
};
