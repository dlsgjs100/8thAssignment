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

    // ���� ���� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    USceneComponent* Scene;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    UBoxComponent* SpawningBox;
    // ����� ������ DataTable 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    UDataTable* CurrentObjectDataTable;
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SetCurrentObjectDataTable(UDataTable* SetObjectDataTable);
    // ���� ������Ʈ ����
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    AActor* SpawnRandomObject();
    // ���� ��������
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    FVector GetRandomPointInVolume() const;
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    FRotator GetRandomRotation() const;
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    AActor* SpawnObject(TSubclassOf<AActor> ObjectClass);
    FObjectSpawnRow* GetRandomObject() const;
};
