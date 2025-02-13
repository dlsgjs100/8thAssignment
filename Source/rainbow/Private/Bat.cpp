#include "Bat.h"
#include "Components/SphereComponent.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameState.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"

ABat::ABat()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("Pawn"));
	Collision->InitSphereRadius(6.0f);
	Collision->SetupAttachment(Scene);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/bat/source/rpg_monster_bat_model_001.rpg_monster_bat_model_001"));
	if (MeshAsset.Succeeded())
	{
		SkeletalMesh->SetSkeletalMesh(MeshAsset.Object);
		SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> AnimAsset(TEXT("/Game/bat/source/rpg_monster_bat_model_001_Anim.rpg_monster_bat_model_001_Anim"));
	if (AnimAsset.Succeeded())
	{
		SkeletalMesh->SetAnimation(AnimAsset.Object);
		SkeletalMesh->Play(true);
	}
	SkeletalMesh->SetupAttachment(Collision);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(SkeletalMesh);
	OverheadWidget->SetWidgetSpace(EWidgetSpace::World);

	bHasDealtDamage = false;
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABat::OnObjectOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABat::OnObjectEndOverlap);

	Direction = FVector::ZeroVector;
	MoveSpeed = 100.f;
	RotationSpeed = 90.0f;
	SwitchInterval = 2.5f;
	TimeElapsed = 0.0f;
	bIsMovingForward = true;

	Health = MaxHealth;
	Damage = 10.0f;

	PointValue = 0;
	ItemType = "DefaultBat";
	ColorType = "DefaultColor";
}

float ABat::GetHealth() const
{
	return Health;
}

float ABat::GetMaxHealth() const
{
	return MaxHealth;
}

void ABat::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

void ABat::OnObjectOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	TArray<AActor*> OverlappingActors;
	Collision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (OtherActor && OtherActor->ActorHasTag("Player") && !bHasDealtDamage)
		{
			// 플레이어와 충돌시 데미지
			if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(OtherActor))
			{
				UGameplayStatics::ApplyDamage(
					PlayerCharacter,
					Damage,
					nullptr,
					this,
					UDamageType::StaticClass()
				);
				bHasDealtDamage = true;
			}
		}
	}
}

void ABat::OnObjectEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	bHasDealtDamage = false;
}

FName ABat::GetObjectType() const
{
	return ItemType;
}

float ABat::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ABat::OnDeath()
{
	if (AMyCharacter* Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UWorld* World = GetWorld())
		{
			// 색의 타입이 같으면 점수 획득
			if (Player->GetCurrentColorType() == ColorType || this->ActorHasTag("Rainbow"))
			{
				if (AMyGameState* GameState = World->GetGameState<AMyGameState>())
				{
					GameState->AddScore(PointValue);
					GameState->OnBatKilled();
				}
			}
		}
		Destroy();
	}
}

void ABat::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
	Direction = GetActorForwardVector();
}

void ABat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeElapsed += DeltaTime;
	if (bIsMovingForward)
	{
		MoveBat(DeltaTime);

		if (TimeElapsed >= SwitchInterval)
		{
			TimeElapsed = 0.0f;
			bIsMovingForward = false;
			Direction *= -1;
		}
	}
	else
	{
		RotateBat(DeltaTime);

		if (TimeElapsed >= 180.0f / RotationSpeed)
		{
			TimeElapsed = 0.0f;
			bIsMovingForward = true;
		}
	}

	if (OverheadWidget)
	{
		AMyCharacter* MyCharacter = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if(MyCharacter)
		{
				FVector BatLocation = GetActorLocation();
				FVector DirectionToRotate = MyCharacter->GetActorLocation() - BatLocation;
				FRotator NewRotation = DirectionToRotate.Rotation();
				OverheadWidget->SetWorldRotation(NewRotation);
		}
	}
}

void ABat::MoveBat(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = CurrentLocation + MoveSpeed * Direction * DeltaTime;
	SetActorLocation(NewLocation);
}

void ABat::RotateBat(float DeltaTime)
{
	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
}

void ABat::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UProgressBar* HealthBar = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		float HealthPercent = GetHealth() / GetMaxHealth();
		HealthBar->SetPercent(HealthPercent);
	}
}