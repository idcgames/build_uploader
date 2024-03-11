// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IDCGamesPlugin_Settings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IDCGamesBPLibrary.generated.h"


UCLASS()
class UIDCGamesBPLibrary : public UBlueprintFunctionLibrary {
	GENERATED_UCLASS_BODY()
	static class UObjectLibrary* ObjectLibrary;
	static FString BaseURL;
	static void MakeRequest(FString uri, FString body, TFunction<void(TSharedPtr<FJsonObject> JsonObject)> OnSuccess, TFunction<void(int errorCode, FString message)> OnError);
public:
	static UIDCGamesPlugin_Settings* Data;
	static bool UploadFile(FString localFilePath, FString remoteFilePath, FString host, FString userpass);
};

USTRUCT(BlueprintType, Category = "IDCGames")
struct FRelease {
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString exe;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString size;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString published;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	bool active;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	int sync_status;
};


USTRUCT(BlueprintType, Category = "IDCGames")
struct FUpdateRelease {
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString rel_name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString exe;
};

USTRUCT(BlueprintType, Category = "IDCGames")
struct FGetFTPAccess {
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString rel_dir;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	int hours;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Basic)
	FString email;	
};