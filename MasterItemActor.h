// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MasterItemActor.generated.h"


USTRUCT(BlueprintType)
struct FItemInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	UTexture2D* bItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	FString bItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	FText bItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	int32 bItemStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	bool bConsumeable;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	bool bHotbarPlaceable;

	//Constructor
	FItemInfo()
	{

	}
};

/**
 * 
 */
UCLASS()
class REPLICATION_API AMasterItemActor : public AActor
{
	GENERATED_BODY()
	
	
public:
	AMasterItemActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemStructure")
	FItemInfo bIteminfo;
};