// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "PokeHunterGameModeBase.h"
#include "Hunter/Hunter.h"
#include "Hunter/HunterController.h"

APokeHunterGameModeBase::APokeHunterGameModeBase()
{
	DefaultPawnClass = AHunter::StaticClass();
}

void APokeHunterGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
