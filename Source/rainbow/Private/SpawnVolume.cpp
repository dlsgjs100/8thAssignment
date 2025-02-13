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

    // Volume�� ���� ���������� ��ȯ
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

    // 1) ��� Row(��) ��������
    TArray<FObjectSpawnRow*> AllRows;
    static const FString ContextString(TEXT("ObjectSpawnContext"));
    CurrentObjectDataTable->GetAllRows(ContextString, AllRows);

    if (AllRows.IsEmpty()) return nullptr;

    // 2) ��ü Ȯ�� �� ���ϱ�
    float TotalChance = 0.0f; // �ʱ�ȭ
    for (const FObjectSpawnRow* Row : AllRows) // AllRows �迭�� �� Row�� ��ȸ
    {
        if (Row) // Row�� ��ȿ���� Ȯ��
        {
            TotalChance += Row->SpawnChance; // SpawnChance ���� TotalChance�� ���ϱ�
        }
    }

    // 3) 0 ~ TotalChance ���� ���� ��
    const float RandValue = FMath::FRandRange(0.0f, TotalChance);
    float AccumulateChance = 0.0f;

    // 4) ���� Ȯ���� ������ ����
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
            // SpawnObject()�� ȣ���ϰ� ������ AActor�����͸� ����
            return SpawnObject(ActualClass);
        }
    }

    return nullptr;
}

AActor* ASpawnVolume::SpawnObject(TSubclassOf<AActor> ObjectClass)
{
    if (!ObjectClass) return nullptr;

    // SpawnActor�� �����ϸ� ������ ������ ������ ��ȯ
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        ObjectClass,
        GetRandomPointInVolume(),
        GetRandomRotation()
    );

    return SpawnedActor;
}