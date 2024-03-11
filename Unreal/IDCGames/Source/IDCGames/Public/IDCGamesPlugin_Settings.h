// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IDCGamesPlugin_Settings.generated.h"

/**
 * 
 */
UCLASS(config = MySetting)
class UIDCGamesPlugin_Settings : public UObject
{
	GENERATED_BODY()
public:
	UIDCGamesPlugin_Settings(const FObjectInitializer& obj);
	UPROPERTY(Config, EditAnywhere, Category = "IDCGames")
	FString GameID;
	UPROPERTY(Config, EditAnywhere, Category = "IDCGames")
	FString GameSecret;
	UPROPERTY(Config, EditAnywhere, Category = "IDCGames")
	FString email;
};
