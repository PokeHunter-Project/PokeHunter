// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HunterState.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API AHunterState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY()
		FString PlayerSessionId;

	UPROPERTY()
		FString MatchmakingPlayerId;

	UPROPERTY(Replicated)
		FString Team;

	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const;
};