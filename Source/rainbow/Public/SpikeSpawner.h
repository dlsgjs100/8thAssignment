#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeSpawner.generated.h"

class UBoxComponent;
UCLASS()
class RAINBOW_API ASpikeSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpikeSpawner();

	// ���� ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	// ������ũ ����
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AActor> ActorToSpawn;
	UPROPERTY(EditAnywhere, Category = "Spawner")
	bool bShoulSpawn;
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float CurrentTime;
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float SpawnIntervalTime;
	FTimerHandle SpikeTimerHandle;
	// ���� ��������
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	FVector GetRandomPointInVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnSpike();

	virtual void Tick(float DeltaTime) override;
};
