// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/TitleHUD.h"
#include "Blueprint/UserWidget.h"

ATitleHUD::ATitleHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> TitleObj(TEXT("/Game/Map/Title/WBP_Title.WBP_Title"));
	TitleWidgetClass = TitleObj.Class;
}

void ATitleHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if(PlayerController != nullptr)
	{
		PlayerController->bShowMouseCursor = true;
	}

	if(TitleWidgetClass != nullptr)
	{
		UUserWidget* TitleWidget = CreateWidget<UUserWidget>(GetWorld(), TitleWidgetClass);
		if(TitleWidget != nullptr)
		{
			TitleWidget->AddToViewport();
			TitleWidget->SetFocus();
		}
	}
}
