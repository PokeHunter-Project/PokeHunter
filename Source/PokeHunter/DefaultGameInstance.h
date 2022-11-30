// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Partner/Partner.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
	UDefaultGameInstance();

	virtual void Init()override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	class UItemDatabase* Database;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemData")
	TArray<class UItemData*> DataArray;

	//FOnCreateSessionCompleteDelegate CreateSessionCompleteD;
	//FDelegateHandle CreateSessionCompleteDH;
};
