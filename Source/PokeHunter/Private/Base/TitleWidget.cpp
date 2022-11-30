// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/TitleWidget.h"
#include "Base/TextReaderComponent.h"
#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"
#include "Base/TextReaderComponent.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "PokeHunter/Public/Base/PokeHunterGameInstance.h"

UTitleWidget::UTitleWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	LoginUrl = TextReader->ReadFile("Urls/LoginUrl.txt");
	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");
	CallbackUrl = TextReader->ReadFile("Urls/CallbackUrl.txt");

	HttpModule = &FHttpModule::Get();
}

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;

	WebBrowser = (UWebBrowser*)(GetWidgetFromName(TEXT("WebBrowser_Login")));

	IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();

	if(WebBrowserSingleton != nullptr) 
	{
		TOptional<FString> DefaultContext;
		TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager(DefaultContext);
		if(CookieManager.IsValid())
		{
			CookieManager->DeleteCookies();
		}
	}

	WebBrowser->LoadURL(LoginUrl);

	FScriptDelegate LoginDelegate;
	LoginDelegate.BindUFunction(this, "HandleLoginUrlChange");
	WebBrowser->OnUrlChanged.Add(LoginDelegate);
}

void UTitleWidget::HandleLoginUrlChange()
{
	FString BrowserUrl = WebBrowser->GetUrl();
	FString Url;
	FString QueryParameters;

	if(BrowserUrl.Split("?", &Url, &QueryParameters))
	{
		if(Url.Equals(CallbackUrl))
		{
			FString ParameterName;
			FString ParameterValue;

			if(QueryParameters.Split("=", &ParameterName, &ParameterValue))
			{
				if(ParameterName.Equals("code"))
				{
					ParameterValue = ParameterValue.Replace(*FString("#"), *FString(""));

					TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
					RequestObj->SetStringField(ParameterName, ParameterValue);

					FString RequestBody;
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

					if(FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
					{
						TSharedRef<IHttpRequest, ESPMode::ThreadSafe> ExchangeCodeForTokensRequest = HttpModule->CreateRequest();
						ExchangeCodeForTokensRequest->OnProcessRequestComplete().BindUObject(this, &UTitleWidget::OnExchangeCodeForTokensResponseReceived);
						ExchangeCodeForTokensRequest->SetURL(ApiUrl + "/exchangecodefortokens");
						ExchangeCodeForTokensRequest->SetVerb("POST");
						ExchangeCodeForTokensRequest->SetHeader("Content-Type", "application/json");
						ExchangeCodeForTokensRequest->SetContentAsString(RequestBody);
						ExchangeCodeForTokensRequest->ProcessRequest();
					}
				}
			}
		}
	}
}

void UTitleWidget::OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if(!JsonObject->HasField("error"))
			{
				UGameInstance* GameInstance = GetGameInstance();
				if(GameInstance != nullptr)
				{
					UPokeHunterGameInstance* PokeHunterGameInstance = Cast<UPokeHunterGameInstance>(GameInstance);
					if(PokeHunterGameInstance != nullptr)
					{
						PokeHunterGameInstance->SetCognitoTokens(JsonObject->GetStringField("access_token"), JsonObject->GetStringField("id_token"), JsonObject->GetStringField("refresh_token"));
					}
				}
			}
		}
	}
}

