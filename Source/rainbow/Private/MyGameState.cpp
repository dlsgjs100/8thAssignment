#include "MyGameState.h"
#include "MyGameInstance.h"
#include "MyCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "MyPlayerController.h"
#include "SpawnVolume.h"
#include "Bat.h"
#include "WallActor.h"
#include "SpikeTrap.h"
#include "SpikeSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"
#include "Components/LightComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

AMyGameState::AMyGameState()
{
	//Score = 0;
	SpawnedBatCount = 0;
	KilledBatCount = 0;

	WaveDurations = { 30.0f ,40.0f,50.0f };
	CurrentWaveIndex = 0;
	MaxWaves = 3;

	SpikeInterval = 2.0f;

	LevelDuration = 0.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance && MyGameInstance->bGameStarted)
		{
			CurrentLevelIndex = MyGameInstance->CurrentLevelIndex; 
			StartLevel();
		}
	}

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&AMyGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 AMyGameState::GetScore() const
{
	return Score;
}

void AMyGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			MyGameInstance->AddToScore(Amount);
		}
	}
}

void AMyGameState::StartWave()
{
	bWaveAnimPlayed = false;

	// Change WallActor Color each Wave
	for (TActorIterator<AWallActor> It(GetWorld()); It; ++It)
	{
		AWallActor* Wall = *It;
		if (Wall)
		{
			Wall->ChangeColor(CurrentWaveIndex);
		}
	}
	// Change Directional Light Color each Wave
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		ADirectionalLight* DirLight = *It;
		if (DirLight)
		{
			ULightComponent* LightComp = DirLight->GetLightComponent();
			if (LightComp)
			{
				FLinearColor NewLightColor = FLinearColor::White;
				float NewIntensity;
				FRotator NewRotation;

				switch (CurrentWaveIndex)
				{
				case 0:
					NewLightColor = FLinearColor(1.0f, 0.95f, 0.8f);
					NewIntensity = 1.0f;
					NewRotation = FRotator(-45, 0, 0);
					break;
				case 1:
					NewLightColor = FLinearColor(1.0f, 0.5f, 0.3f);
					NewIntensity = 1.5f;
					NewRotation = FRotator(-15, 90, 0);
					break;
				case 2:
					NewLightColor = FLinearColor(0.1f, 0.1f, 0.3f);
					NewIntensity = 1.0f;
					NewRotation = FRotator(-45, 0, 0);
					break;
				default:
					return;
				}

				LightComp->SetLightColor(NewLightColor);
				LightComp->SetIntensity(NewIntensity);
				DirLight->SetActorRotation(NewRotation);
			}
		}
	}

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowGameHUD();
		}
	}
	// Spawn Bats according to data table for each wave
	SpawnedBatCount = 0;
	KilledBatCount = 0;
	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			SpawnVolume->SetCurrentObjectDataTable(ObjectDataTables[CurrentWaveIndex]);

			const TArray<int32> ItemToSpawns = { 15,20,25 };
			for (int32 i = 0; i < ItemToSpawns[CurrentWaveIndex]; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomObject();
				if (SpawnedActor && SpawnedActor->IsA(ABat::StaticClass()))
				{
					SpawnedBatCount++;
				}
			}
		}
	}
	// Wave duration based on WaveDuration
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&AMyGameState::OnWaveTimeUp,
		WaveDurations[CurrentWaveIndex],
		false
	);
}

void AMyGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	CurrentWaveIndex++;
	if (CurrentWaveIndex < MaxWaves)
	{
		StartWave();
	}
}

void AMyGameState::OnWaveTimeUp()
{
	if (CurrentWaveIndex < MaxWaves)
	{
		EndWave();
	}
}

void AMyGameState::StartLevel()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	bLevelAnimPlayed = false;
	for (float WaveDuration : WaveDurations)
	{
		LevelDuration += WaveDuration;
	}

	StartWave();

	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&AMyGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);
}

void AMyGameState::OnLevelTimeUp()
{
	EndLevel();
}

void AMyGameState::OnBatKilled()
{
	KilledBatCount++;
	UpdateHUD();

	if (SpawnedBatCount > 0 && KilledBatCount >= SpawnedBatCount && CurrentWaveIndex == MaxWaves - 1)
	{
		EndLevel();
	}
}

void AMyGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			MyGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void AMyGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = MyPlayerController->GetHUDWidget())
			{
				// Health Bar & Apply Current Color Image
				if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(PlayerController->GetPawn()))
				{
					if (UProgressBar* HealthBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HealthBar"))))
					{
						float HealthPercent = MyCharacter->GetHealth() / MyCharacter->GetMaxHealth();
						HealthBar->SetPercent(HealthPercent);
					}
					if (UImage* ColorImage = Cast<UImage>(HUDWidget->GetWidgetFromName(TEXT("CurrentColor"))))
					{
						if (ColorImage)
						{
							ColorImage->SetBrushFromMaterial(MyPlayerController->ColorSphereImages[MyCharacter->CurrentColor]);
						}
					}
				}
				// Remaining Time
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainingTime)));
				}
				// Total Score
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
						if (MyGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), MyGameInstance->TotalScore)));
						}
					}
				}
				// Wave Start
				if (!bWaveAnimPlayed)
				{
					UFunction* PlayAnimWave = HUDWidget->FindFunction(FName("PlayWaveStartAnim"));
					if (PlayAnimWave)
					{
						HUDWidget->ProcessEvent(PlayAnimWave, nullptr);
					}
					if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveStartText"))))
					{
						if (CurrentWaveIndex == 0)
						{
							LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d Start!!"), CurrentWaveIndex + 1)));
						}
						else
						{
							LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d Start!!\nNew Weapon Unlocked!!"), CurrentWaveIndex + 1)));
						}
					}
					if (CurrentWaveIndex == 1)
					{
						UFunction* PlayAnimSpike = HUDWidget->FindFunction(FName("PlaySpikeNotiAnim"));
						if (PlayAnimSpike)
						{
							HUDWidget->ProcessEvent(PlayAnimSpike, nullptr);
						}
						if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("SpikeNotiText"))))
						{
							LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Spike Trap Activated!!"))));
						}
					}
					else if (CurrentWaveIndex == 2)
					{
						UFunction* PlayAnimBlackhole = HUDWidget->FindFunction(FName("PlayBlackholeNotiAnim"));
						if (PlayAnimBlackhole)
						{
							HUDWidget->ProcessEvent(PlayAnimBlackhole, nullptr);
						}
						if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("BlackholeNotiText"))))
						{
							LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Blackhole Activated!!"))));
						}
					}
					bWaveAnimPlayed = true;
				}
				// Level Start
				if (!bLevelAnimPlayed)
				{
					// Play Anim
					UFunction* PlayAnimLevel = HUDWidget->FindFunction(FName("PlayLevelStartAnim"));
					if (PlayAnimLevel)
					{
						HUDWidget->ProcessEvent(PlayAnimLevel, nullptr);
					}
					if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("LevelStartText"))))
					{
						LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level %d Start!!"), CurrentLevelIndex + 1)));
					}
					bLevelAnimPlayed = true;
				}
			}
		}
	}
}

void AMyGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->SetPause(true);
			MyPlayerController->ShowMainMenu(true);
		}
	}
}