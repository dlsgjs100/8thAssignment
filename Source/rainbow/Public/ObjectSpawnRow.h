#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ObjectSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FObjectSpawnRow : public FTableRowBase
{
	GENERATED_BODY()

public:
    // ������Ʈ �̸�
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ObjectName;
    // � ������Ʈ Ŭ������ ��������
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ObjectClass;
    // �� ������Ʈ�� ���� Ȯ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnChance;
};