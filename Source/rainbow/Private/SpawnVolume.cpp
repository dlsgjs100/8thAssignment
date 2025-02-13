#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

ASpawnVolume::ASpawnVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);
    SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
    SpawningBox->SetupAttachment(Scene);
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
    FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
    FVector BoxOrigin = SpawningBox->GetComponentLocation();

    // Volume의 안의 랜덤지점을 반환
    return BoxOrigin + FVector(
        FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
        FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
        FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
    );
}

FRotator ASpawnVolume::GetRandomRotation() const
{
    FRotator RandomRotation = FRotator::ZeroRotator;
    RandomRotation.Yaw = FMath::FRandRange(-180.0f, 180.0f);
    return RandomRotation;
}

FObjectSpawnRow* ASpawnVolume::GetRandomObject() const
{
    if (!CurrentObjectDataTable) return nullptr;

    // 1) 모든 Row(행) 가져오기
    TArray<FObjectSpawnRow*> AllRows;
    static const FString ContextString(TEXT("ObjectSpawnContext"));
    CurrentObjectDataTable->GetAllRows(ContextString, AllRows);

    if (AllRows.IsEmpty()) return nullptr;

    // 2) 전체 확률 합 구하기
    float TotalChance = 0.0f; // 초기화
    for (const FObjectSpawnRow* Row : AllRows) // AllRows 배열의 각 Row를 순회
    {
        if (Row) // Row가 유효한지 확인
        {
            TotalChance += Row->SpawnChance; // SpawnChance 값을 TotalChance에 더하기
        }
    }

    // 3) 0 ~ TotalChance 사이 랜덤 값
    const float RandValue = FMath::FRandRange(0.0f, TotalChance);
    float AccumulateChance = 0.0f;

    // 4) 누적 확률로 아이템 선택
    for (FObjectSpawnRow* Row : AllRows)
    {
        AccumulateChance += Row->SpawnChance;
        if (RandValue <= AccumulateChance)
        {
            return Row;
        }
    }

    return nullptr;
}

void ASpawnVolume::SetCurrentObjectDataTable(UDataTable* SetObjectDataTable)
{
    CurrentObjectDataTable = SetObjectDataTable;
}

AActor* ASpawnVolume::SpawnRandomObject()
{
    if (FObjectSpawnRow* SelectedRow = GetRandomObject())
    {
        if (UClass* ActualClass = SelectedRow->ObjectClass.Get())
        {
            // SpawnObject()를 호출하고 스폰된 AActor포인터를 리턴
            return SpawnObject(ActualClass);
        }
    }

    return nullptr;
}

AActor* ASpawnVolume::SpawnObject(TSubclassOf<AActor> ObjectClass)
{
    if (!ObjectClass) return nullptr;

    // SpawnActor가 성공하면 스폰된 액터의 포인터 반환
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        ObjectClass,
        GetRandomPointInVolume(),
        GetRandomRotation()
    );

    return SpawnedActor;
}