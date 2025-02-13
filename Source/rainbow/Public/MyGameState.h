#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

class DirectionalLightActor;
UCLASS()
class RAINBOW_API AMyGameState : public AGameState
{
	GENERATED_BODY()

public:
	AMyGameState();

	virtual void BeginPlay() override;

	TArray<AActor*> FoundVolumes;

	// 전역 점수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	// 소환된 박쥐의 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat")
	int32 SpawnedBatCount;
	// 잡은 박쥐 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat")
	int32 KilledBatCount;

	// 현재 웨이브
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	// 웨이브 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;
	// 웨이브마다 다른 DataTable 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<UDataTable*> ObjectDataTables;
	// 웨이브별 지속시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<float> WaveDurations;

	// 스파이크 소환 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike")
	float SpikeInterval;

	// 레벨별 지속시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	// 현재 레벨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	// 레벨 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	// 레벨이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	FTimerHandle WaveTimerHandle;
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	bool bWaveAnimPlayed  ;
	bool bLevelAnimPlayed ;

	// 전역 점수 Get함수
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	// 점수 추가 함수
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	UFUNCTION(BlueprintCallable)
	void StartWave();
	void EndWave();
	void OnWaveTimeUp();

	UFUNCTION(BlueprintCallable)
	void StartLevel();
	void OnLevelTimeUp();
	void OnBatKilled();
	void EndLevel();

	UFUNCTION(BlueprintCallable)
	void UpdateHUD();
};