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

	// 스폰 가능 지역
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	// 스파이크 스폰
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AActor> ActorToSpawn;
	UPROPERTY(EditAnywhere, Category = "Spawner")
	bool bShoulSpawn;
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float CurrentTime;
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float SpawnIntervalTime;
	FTimerHandle SpikeTimerHandle;
	// 랜덤 스폰지점
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	FVector GetRandomPointInVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnSpike();

	virtual void Tick(float DeltaTime) override;
};
