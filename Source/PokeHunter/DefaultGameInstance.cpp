// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"

void UDefaultGameInstance::Init()
{
	Super::Init();
	// UE_LOG(LogClass, Warning, TEXT("%s"), TEXT("Game Instance Init!"));
}

UDefaultGameInstance::UDefaultGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance Maker"));
}

void UDefaultGameInstance::SetMyPartner(APartner* partner) {
	My_Partner = partner;
}
