#include "MyPlayerController.h"
#include "MyCharacter.h"
#include "MyGameState.h"
#include "MyGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

AMyPlayerController::AMyPlayerController()
	:InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	AttackAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	SwapAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	EffectSlotClass(nullptr),
	EffectSlotInstances{ nullptr }
{
}

UUserWidget* AMyPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AMyPlayerController::ShowGameHUD()
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
		if (MyGameState)
		{
			MyGameState->UpdateHUD();
		}
	}
}

void AMyPlayerController::ShowMainMenu(bool bIsRestart)
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	// 메뉴 UI 생성
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("ScoreText")))
			{
				if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Score: %d"), MyGameInstance->TotalScore)
					));
				}
			}
			
			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("BatCountText")))
			{
				AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
				if (MyGameState)
				{
					MyGameState->UpdateHUD();
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("You Hunted %d bats"), MyGameState->KilledBatCount)
					));
				}
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("BestScoreText")))
			{
				if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Best Score: %d"), MyGameInstance->BestScore)
					));
				}
			}
		}
	}
}

void AMyPlayerController::ShowEffect()
{
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetPawn());
	if (!MyCharacter || !HUDWidgetInstance) return;

	UVerticalBox* VerticalBox = Cast<UVerticalBox>(HUDWidgetInstance->GetWidgetFromName(TEXT("ListEffectSlot")));
	if (!VerticalBox) return;

	// 기존 UI를 다 삭제하고 다시 추가 (중복 방지)
	for (UUserWidget* EffectWidget : EffectSlotInstances)
	{
		if (EffectWidget)
		{
			VerticalBox->RemoveChild(EffectWidget);
			EffectWidget->RemoveFromParent();
		}
	}
	EffectSlotInstances.Empty(); // 배열 초기화

	// 현재 적용된 효과(EffectType)에 따라 UI 추가
	for (int32 i = 0; i < MyCharacter->EffectTypes.Num(); i++)
	{
		int32 EffectType = MyCharacter->EffectTypes[i];
		if (EffectType > 0 && EffectImages.IsValidIndex(EffectType - 1))
		{
			UUserWidget* NewEffectSlot = CreateWidget<UUserWidget>(this, EffectSlotClass);
			if (NewEffectSlot)
			{
				UImage* EffectImage = Cast<UImage>(NewEffectSlot->GetWidgetFromName(TEXT("EffectImage")));
				if (EffectImage)
				{
					EffectImage->SetBrushFromTexture(EffectImages[EffectType - 1]);
				}
				VerticalBox->AddChildToVerticalBox(NewEffectSlot);
				EffectSlotInstances.Add(NewEffectSlot); // 저장
			}
		}
	}
}

void AMyPlayerController::StartGame()
{
	if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		MyGameInstance->CurrentLevelIndex = 0;
		MyGameInstance->TotalScore = 0;
		MyGameInstance->bGameStarted = true;  // 게임 시작 상태 설정
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem
			= LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}