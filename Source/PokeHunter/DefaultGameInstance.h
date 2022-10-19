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

	virtual void Init() override;
	void SetMyPartner(APartner*);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Partner")
	APartner* My_Partner;

	
};
