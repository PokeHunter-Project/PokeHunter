// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// #include "aws/gamelift/internal/network/AuxProxyMessageSender.h"
// #include "aws/gamelift/server/GameLiftServerAPI.h"
#include "GameLiftServerSDK.h"
#include "PokeHunter.h"
#include "GameFramework/GameModeBase.h"
#include "PokeHunterGameModeBase.generated.h"

/**
 * 
 */

USTRUCT()
struct FStartGameSessionState
{
	GENERATED_BODY()

	UPROPERTY()
		bool Status;

	FStartGameSessionState() {
		Status = false;
	}
};

USTRUCT()
struct FUpdateGameSessionState
{
	GENERATED_BODY()

	FUpdateGameSessionState() {

	}
};


USTRUCT()
struct FProcessTerminateState
{
	GENERATED_BODY()

		UPROPERTY()
		bool Status;

	long TerminationTime;

	FProcessTerminateState() {
		Status = false;
	}
};

USTRUCT()
struct FHealthCheckState
{
	GENERATED_BODY()
		bool Status;

	FHealthCheckState() {
		Status = false;
	}
};

UCLASS()
class POKEHUNTER_API APokeHunterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APokeHunterGameModeBase();
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
		FStartGameSessionState StartGameSessionState;

	UPROPERTY()
		FUpdateGameSessionState UpdateGameSessionState;

	UPROPERTY()
		FProcessTerminateState ProcessTerminateState;

	UPROPERTY()
		FHealthCheckState HealthCheckState;
};
