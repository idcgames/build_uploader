// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "IDCGamesBPLibrary.h"
#include "UploaderUtilityItem.generated.h"


/**
 * 
 */
UCLASS()
class IDCGAMES_API UUploaderUtilityItem : public UUserWidget {
	GENERATED_BODY()
private:
	UPROPERTY()
	struct FRelease release;
protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ID;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Name;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Exe;

	UPROPERTY(meta = (BindWidget))
	class UButton* UpdateButton;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PublishedArea;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* UnpublishedArea;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PublishingArea;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Published;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Size;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Active;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ActivateButtonText;

	UPROPERTY(meta = (BindWidget))
	class UButton* ActivateButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* GetFTPAccessButton;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* FTPArea;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* User;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Password;
	UPROPERTY(meta = (BindWidget))
	class UButton* UploadBuildButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UploadBuildButtonText;


public:
	void Setup(FRelease* _release);
	UFUNCTION()
	void OnChanged(const FText& NewText);
	UFUNCTION()
	void UpdateRelease();
	UFUNCTION()
	void ActivateRelease();
	UFUNCTION()
	void GetFTPAccess();
	UFUNCTION()
	void OnFTPChanged(const FText& NewText);
	UFUNCTION()
	void UploadRelease();
};