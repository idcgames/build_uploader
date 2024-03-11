// Copyright Epic Games, Inc. All Rights Reserved.
#include "IDCGamesBPLibrary.h"

#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GenericPlatform/GenericPlatformHttp.h"

#include "Engine/ObjectLibrary.h"

#include "IDCGames.h"

FString UIDCGamesBPLibrary::BaseURL = "https://admin.idcgames.com";

UObjectLibrary* UIDCGamesBPLibrary::ObjectLibrary;
UIDCGamesPlugin_Settings* UIDCGamesBPLibrary::Data;


UIDCGamesBPLibrary::UIDCGamesBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Data = GetMutableDefault<UIDCGamesPlugin_Settings>();
}

void UIDCGamesBPLibrary::MakeRequest(FString uri, FString body, TFunction<void(TSharedPtr<FJsonObject> JsonObject)> OnSuccess, TFunction<void(int errorCode, FString message)> OnError) {
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	FString url = BaseURL + uri;
	FString verb;
	if (body.IsEmpty()) verb = "GET";
	else {
		verb = "POST";
		HttpRequest->SetContentAsString(*body);
	}
	HttpRequest->SetURL(url);
	HttpRequest->SetVerb(verb);
	HttpRequest->SetHeader("content-type", TEXT("application/json"));
	HttpRequest->SetHeader("GameID", Data->GameID);
	HttpRequest->SetHeader("GameSecret", Data->GameSecret);
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Display, TEXT("[IDCGames] %s Request: URL(%s)"), *verb, *url);
#endif
	HttpRequest->OnProcessRequestComplete().BindLambda([OnSuccess, OnError, url](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
		{
			TSharedPtr<FJsonObject> JsonObject;
			if (bSuccess) {
				FString data = Response->GetContentAsString();
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(data);
				if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
					if (JsonObject->HasField("error")) {
						UE_LOG(LogTemp, Display, TEXT("[IDCGames] Error on request (%s): %s"), *url, *data);
						TSharedPtr<FJsonObject> errorObject;
						errorObject = JsonObject->GetObjectField("error");
						int errorCode = FCString::Atoi(*errorObject->GetStringField("code"));
						FString errorMessage = errorObject->GetStringField("message");
						OnError(errorCode, errorMessage);
						return;
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("[IDCGames] Error JSON deserialization (%s): %s"), *url, *Response->GetContentAsString());
					OnError(-2, TEXT("JSON deserialization error."));
					return;
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("[IDCGames] HTTP generic error (%s)"), *url);
				OnError(-1, TEXT("HTTP generic error."));
				return;
			}
#if !UE_BUILD_SHIPPING
			UE_LOG(LogTemp, Display, TEXT("[IDCGames] OnSuccess (%s): %s"), *url, *Response->GetContentAsString());
#endif
			OnSuccess(JsonObject); });
	HttpRequest->ProcessRequest();
}


