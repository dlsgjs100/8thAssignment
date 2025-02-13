#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ObjectSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FObjectSpawnRow : public FTableRowBase
{
	GENERATED_BODY()

public:
    // 오브젝트 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ObjectName;
    // 어떤 오브젝트 클래스를 스폰할지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ObjectClass;
    // 이 오브젝트의 스폰 확률
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnChance;
};