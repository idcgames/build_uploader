// Fill out your copyright notice in the Description page of Project Settings.


#include "UploaderUtilityItem.h"

#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"

#include "JsonObjectConverter.h"

#include "FTPUploader.h"


void UUploaderUtilityItem::Setup(FRelease* _release) {
	release = *_release;
	if (ID) ID->SetText(FText::FromString(_release->id));
	if (Name) {
		Name->SetText(FText::FromString(_release->name));
		Name->OnTextChanged.AddDynamic(this, &UUploaderUtilityItem::OnChanged);
	}
	if (Exe) {
		Exe->SetText(FText::FromString(_release->exe));
		Exe->OnTextChanged.AddDynamic(this, &UUploaderUtilityItem::OnChanged);
	}
	switch (_release->sync_status) {
		case 0: // Unpublished
			if(UnpublishedArea) UnpublishedArea->SetVisibility(ESlateVisibility::Visible);
			if (PublishingArea) PublishingArea->SetVisibility(ESlateVisibility::Collapsed);
			if (PublishedArea) PublishedArea->SetVisibility(ESlateVisibility::Collapsed);
			break;
		case 1:
			if (UnpublishedArea) UnpublishedArea->SetVisibility(ESlateVisibility::Collapsed);
			if (PublishingArea) PublishingArea->SetVisibility(ESlateVisibility::Collapsed);
			if (PublishedArea) PublishedArea->SetVisibility(ESlateVisibility::Visible);
			break;
		case 2:
			if (UnpublishedArea) UnpublishedArea->SetVisibility(ESlateVisibility::Collapsed);
			if (PublishingArea) PublishingArea->SetVisibility(ESlateVisibility::Visible);
			if (PublishedArea) PublishedArea->SetVisibility(ESlateVisibility::Collapsed);
			break;
	}
	if (Size) Size->SetText(FText::FromString(_release->size));
	if (Published) Published->SetText(FText::FromString(_release->published));

	if (UpdateButton) {
		UpdateButton->OnClicked.AddDynamic(this, &UUploaderUtilityItem::UpdateRelease);
		UpdateButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Active) Active->SetText(FText::FromString(_release->active?L"Yes":L"No"));
	if (ActivateButton) {
		ActivateButtonText->SetText(FText::FromString(_release->active ? L"Deactivate" : L"Activate"));
		ActivateButton->OnClicked.AddDynamic(this, &UUploaderUtilityItem::ActivateRelease);
	}

	if (GetFTPAccessButton) GetFTPAccessButton->OnClicked.AddDynamic(this, &UUploaderUtilityItem::GetFTPAccess);

	// if(FTPArea) FTPArea->SetVisibility(ESlateVisibility::Collapsed);
	if (User) {
		//User->SetText(FText::FromString(release->name));
		User->OnTextChanged.AddDynamic(this, &UUploaderUtilityItem::OnFTPChanged);
	}
	if (Password) {
		//Password->SetText(FText::FromString(release->exe));
		Password->OnTextChanged.AddDynamic(this, &UUploaderUtilityItem::OnFTPChanged);
	}
	if (UploadBuildButton) {
		UploadBuildButton->SetIsEnabled(false);
		UploadBuildButton->OnClicked.AddDynamic(this, &UUploaderUtilityItem::UploadRelease);
	}
}

void UUploaderUtilityItem::UpdateRelease() {
	if (ID && Name && Exe) {
		FUpdateRelease updateData;
		updateData.rel_name = *Name->GetText().ToString();
		updateData.exe = *Exe->GetText().ToString();

		FString body;
		FJsonObjectConverter::UStructToJsonObjectString(updateData, body, 0, 0);

		FString uri = FString::Format(TEXT("/api/release/update/{0}"), { *ID->GetText().ToString() });
		UIDCGamesBPLibrary::MakeRequest(uri, body, [this](TSharedPtr<FJsonObject> JsonObject) {
			if (UpdateButton) UpdateButton->SetVisibility(ESlateVisibility::Hidden);
			}, [](int errorCode, FString message) {
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Format(TEXT("Error({0}) {1}"), { errorCode, message }));
				});
	}
}

void UUploaderUtilityItem::ActivateRelease() {
	if (ActivateButton) ActivateButton->SetIsEnabled(false);
	FString uri = FString::Format(TEXT("/api/release/activate/{0}?active={1}"), { *ID->GetText().ToString(), release.active ? 0:1 });
	
	UIDCGamesBPLibrary::MakeRequest(uri, "{}", [this](TSharedPtr<FJsonObject> JsonObject) {
		if (ActivateButton) ActivateButton->SetIsEnabled(true);
		release.active = !release.active;
		if (Active) Active->SetText(FText::FromString(release.active ? L"Yes" : L"No"));
		if (ActivateButton) ActivateButtonText->SetText(FText::FromString(release.active ? L"Deactivate" : L"Activate"));
	}, [this](int errorCode, FString message) {
		if (ActivateButton) ActivateButton->SetIsEnabled(true);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Format(TEXT("Error({0}) {1}"), { errorCode, message }));
	});
}

void UUploaderUtilityItem::OnChanged(const FText& NewText) {
	if (UpdateButton) UpdateButton->SetVisibility(ESlateVisibility::Visible);
}

void UUploaderUtilityItem::GetFTPAccess() {
	FGetFTPAccess accessData;
	accessData.rel_dir = L"common";	
	accessData.hours = 1;
	accessData.email = *UIDCGamesBPLibrary::Data->email;

	FString body;
	FJsonObjectConverter::UStructToJsonObjectString(accessData, body, 0, 0);

	FString uri = FString::Format(TEXT("/api/release/ftp/{0}"), { *ID->GetText().ToString() });
	UIDCGamesBPLibrary::MakeRequest(uri, body, [this](TSharedPtr<FJsonObject> JsonObject) {
		if (GetFTPAccessButton) GetFTPAccessButton->SetVisibility(ESlateVisibility::Collapsed);
		if (FTPArea) FTPArea->SetVisibility(ESlateVisibility::Visible);
	}, [](int errorCode, FString message) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Format(TEXT("Error({0}) {1}"), { errorCode, message }));
	});
}

void UUploaderUtilityItem::OnFTPChanged(const FText& NewText) {
	if (User && Password && UploadBuildButton) {
		UploadBuildButton->SetIsEnabled(User->GetText().ToString().Len() > 5 && Password->GetText().ToString().Len() > 5);
	}
}

void UUploaderUtilityItem::UploadRelease() {
	FTPUploader::ReleaseID = release.id;
	FTPUploader::Credentials = FString(User->GetText().ToString() + L":" + Password->GetText().ToString());
	(new FAutoDeleteAsyncTask<FTPUploader>())->StartBackgroundTask();
}