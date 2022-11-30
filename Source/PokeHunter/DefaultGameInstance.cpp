// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "Item/ItemDatabase.h"

void UDefaultGameInstance::Init()
{
	Super::Init();
	// UE_LOG(LogClass, Warning, TEXT("%s"), TEXT("Game Instance Init!"));
}

UDefaultGameInstance::UDefaultGameInstance()
{
	// IOnlineSession::CreateSession(1, "TestSesion", );
	
	
}

void UDefaultGameInstance::Init()
{
	UE_LOG(LogTemp, Display, TEXT("d"));

	Super::Init();

	if (Database != NULL)
	{
		for (auto& Data : Database->DataClass)
		{
			DataArray.Add(NewObject<UItemData>(Data));
		}
	}
}

