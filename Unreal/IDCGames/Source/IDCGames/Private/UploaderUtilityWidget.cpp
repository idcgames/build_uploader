// Fill out your copyright notice in the Description page of Project Settings.


#include "UploaderUtilityWidget.h"

#include "IDCGamesBPLibrary.h"
#include "JsonObjectConverter.h"
#include "IDCGamesBPLibrary.h"

#include "UploaderUtilityItem.h"

#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"

bool UUploaderUtilityWidget::bDirty = false;
bool UUploaderUtilityWidget::bShowMessage=false;
FString UUploaderUtilityWidget::sTextMessage;
UUploaderUtilityWidget* UUploaderUtilityWidget::Instance = NULL;

void UUploaderUtilityWidget::NativeConstruct(){
	Super::NativeConstruct();

	if (ReleasesContainer) ReleasesContainer->SetVisibility(ESlateVisibility::Hidden);
	if (RefreshButton) RefreshButton->OnClicked.AddDynamic(this, &UUploaderUtilityWidget::Refresh);
	if (NewReleaseButton) NewReleaseButton->OnClicked.AddDynamic(this, &UUploaderUtilityWidget::NewRelease);

	FString AppVersion;
	GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), AppVersion, GGameIni );

	const TCHAR* name = FApp::GetProjectName();

	HideMessage();

	UUploaderUtilityWidget::Instance = this;
	Refresh();
}

void UUploaderUtilityWidget::NativeTick(const FGeometry & MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bDirty) {
		Interfaz->SetVisibility(bShowMessage ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		Message->SetVisibility(bShowMessage ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		Message->SetText(FText::FromString(sTextMessage));
		bDirty = false;
	}
}


void UUploaderUtilityWidget::NewRelease() {
	FUpdateRelease createData;
	FString AppVersion;
	GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), AppVersion, GGameIni);

	createData.rel_name = FString::Format(TEXT("New-Version-{0}"), { *AppVersion });
	createData.exe = FString::Format(TEXT("\"/{0}.exe\""), { FApp::GetProjectName() });

	FString body;
	FJsonObjectConverter::UStructToJsonObjectString(createData, body, 0, 0);

	UIDCGamesBPLibrary::MakeRequest(L"/api/release", body, [this](TSharedPtr<FJsonObject> JsonObject) {
		Refresh();
	}, [](int errorCode, FString message) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Format(TEXT("Error({0}) {1}"), { errorCode, message }));
	});
}

void UUploaderUtilityWidget::Refresh(){
	if (GameID) GameID->SetText(FText::FromString(UIDCGamesBPLibrary::Data->GameID));
	if (GameSecret) GameSecret->SetText(FText::FromString(UIDCGamesBPLibrary::Data->GameSecret));
	if (eMail) eMail->SetText(FText::FromString(UIDCGamesBPLibrary::Data->email));
	if (ReleasesContainer) ReleasesContainer->SetVisibility(ESlateVisibility::Visible);
	UIDCGamesBPLibrary::MakeRequest("/api/release", "", [this](TSharedPtr<FJsonObject> JsonObject) {
		TArray<FRelease> releases;
#if ENGINE_MAJOR_VERSION >= 5
		FJsonObjectConverter::JsonArrayToUStruct(JsonObject->GetArrayField("data"), &releases, 0, 0, false);
#else
		FJsonObjectConverter::JsonArrayToUStruct(JsonObject->GetArrayField("data"), &releases, 0, 0);
#endif
		// https://benui.ca/unreal/listview/#buiinventoryentry
		
		if (Releases) {
			Releases->ClearChildren();
			for (FRelease release : releases) {
				UUploaderUtilityItem* Entry = CreateWidget<UUploaderUtilityItem>(this, EntryClass);
				if (Entry) {
					// Set up its contents
					Entry->Setup(&release);
					// Add it to the list
					Releases->AddChild(Entry);
				}
			}
		}		
	}, [](int errorCode, FString message) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Format(TEXT("Error({0}) {1}"), { errorCode, message }));
	});
}

void UUploaderUtilityWidget::ShowMessage(FString _message) {
	sTextMessage = _message;
	bShowMessage = true;
	bDirty = true;
}

void UUploaderUtilityWidget::HideMessage() {
	bShowMessage = false;
	bDirty = true;

}