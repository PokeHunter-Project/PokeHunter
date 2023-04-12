// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PokeHunter/Base/BaseInstance.h"
#include "PokeHunter/Network/PartyServerManager.h"

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "PartyInfoUI.generated.h"

class UWebBrowser;
class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS(BlueprintType)
class POKEHUNTER_API UPartyInfoUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPartyInfoUI(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UBaseInstance* gameinstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		int SelctPartyNumber = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<FName> PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<PLAYER_PET> PlayerPetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		TArray<PLAYER_STATE> PartyMemberState;

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool SendClientState();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool TickSendPartyInfo();

	UFUNCTION(BlueprintCallable, Category = "Party info")
		bool RecvClientJoin();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		UButton* JoinButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party info")
		UTextBlock* JoinEventTextBlock;

	UPROPERTY()
		bool SearchingForGame;
	
	UFUNCTION()
		void SetAveragePlayerLatency();

	UFUNCTION()
		void OnJoinButtonClicked();

	UPROPERTY()
		FTimerHandle PollMatchmakingHandle;

	UPROPERTY()
		FTimerHandle SetAveragePlayerLatencyHandle;

private:
	UPROPERTY()
		UWebBrowser* WebBrowser;

	UPROPERTY()
		float AveragePlayerLatency;

	UFUNCTION()
		void PollMatchmaking();

	void OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
