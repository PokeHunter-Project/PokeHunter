#include "Base/PokeHunterGameInstance.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Base/TextReaderComponent.h"

UPokeHunterGameInstance::UPokeHunterGameInstance()
{
	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");

	HttpModule = &FHttpModule::Get();
}

void UPokeHunterGameInstance::Shutdown()
{
	Super::Shutdown();

	if(AccessToken.Len() > 0)
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> InvalidateTokensRequest = HttpModule->CreateRequest();
		InvalidateTokensRequest->SetURL(ApiUrl + "/invalidatetokens");
		InvalidateTokensRequest->SetVerb("GET");
		InvalidateTokensRequest->SetHeader("Content-Type", "application/json");
		InvalidateTokensRequest->SetHeader("Authorization", AccessToken);
		InvalidateTokensRequest->ProcessRequest();
	}
}

void UPokeHunterGameInstance::SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken)
{
	AccessToken = NewAccessToken;
	IdToken = NewIdToken;
	RefreshToken = NewRefreshToken;

	// UE_LOG(LogTemp, Warning, TEXT("access token: %s"), *AccessToken);
	// UE_LOG(LogTemp, Warning, TEXT("refresh token: %s"), *RefreshToken);

	GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UPokeHunterGameInstance::RetrieveNewTokens, 1.0f, false, 3300.0f);
}

void UPokeHunterGameInstance::RetrieveNewTokens()
{
	if(AccessToken.Len() > 0 && RefreshToken.Len() > 0)
	{
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("refreshToken", RefreshToken);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) 
		{
			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> RetrieveNewTokensRequest = HttpModule->CreateRequest();
			RetrieveNewTokensRequest->OnProcessRequestComplete().BindUObject(this, &UPokeHunterGameInstance::OnRetrieveNewTokensResponseReceived);
			RetrieveNewTokensRequest->SetURL(ApiUrl + "/retrievenewtokens");
			RetrieveNewTokensRequest->SetVerb("POST");
			RetrieveNewTokensRequest->SetHeader("Content-Type", "application/json");
			RetrieveNewTokensRequest->SetHeader("Authorization", AccessToken);
			RetrieveNewTokensRequest->SetContentAsString(RequestBody);
			RetrieveNewTokensRequest->ProcessRequest();
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UPokeHunterGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
}

void UPokeHunterGameInstance::OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if(!JsonObject->HasField("error"))
			{
				SetCognitoTokens(JsonObject->GetStringField("accessToken"), JsonObject->GetStringField("idToken"), RefreshToken);
			}
			else
			{
				GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UPokeHunterGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
			}
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UPokeHunterGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
	}
}

