// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/TitleGameMode.h"
#include "Base/TitleHUD.h"

ATitleGameMode::ATitleGameMode()
{
	HUDClass = ATitleHUD::StaticClass();
}
