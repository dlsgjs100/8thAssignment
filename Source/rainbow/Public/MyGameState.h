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

	// ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	// ��ȯ�� ������ ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat")
	int32 SpawnedBatCount;
	// ���� ���� ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bat")
	int32 KilledBatCount;

	// ���� ���̺�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	// ���̺� ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;
	// ���̺긶�� �ٸ� DataTable ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<UDataTable*> ObjectDataTables;
	// ���̺꺰 ���ӽð�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<float> WaveDurations;

	// ������ũ ��ȯ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike")
	float SpikeInterval;

	// ������ ���ӽð�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	// ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	// ���� ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	// �����̸�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	FTimerHandle WaveTimerHandle;
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	bool bWaveAnimPlayed  ;
	bool bLevelAnimPlayed ;

	// ���� ���� Get�Լ�
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	// ���� �߰� �Լ�
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