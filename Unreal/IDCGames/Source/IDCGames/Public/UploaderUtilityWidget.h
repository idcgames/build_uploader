// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "UploaderUtilityWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UUploaderUtilityWidget : public UEditorUtilityWidget {
	GENERATED_BODY()
public:
	static UUploaderUtilityWidget* Instance;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* Interfaz;

	UPROPERTY(meta = (BindWidget))
	class UButton* RefreshButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GameID;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GameSecret;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* eMail;
	UPROPERTY(meta = (BindWidget))
	class UButton* NewReleaseButton;
//	UPROPERTY(meta = (BindWidget))
//	class UVerticalBox* Releases;
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Releases;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ReleasesContainer;
	UPROPERTY(EditDefaultsOnly, Category=Default)
	TSubclassOf<class UUploaderUtilityItem> EntryClass;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Message;

	static void ShowMessage(FString _message);
	static void HideMessage();

	UFUNCTION()
	void Refresh();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UFUNCTION()
	void NewRelease();
	static bool bDirty;
	static bool bShowMessage;
	static FString sTextMessage;
};
