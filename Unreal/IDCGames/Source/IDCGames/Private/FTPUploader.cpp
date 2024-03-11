// Fill out your copyright notice in the Description page of Project Settings.


#include "FTPUploader.h"
#include "UploaderUtilityWidget.h"
#include "IDCGamesBPLibrary.h"

#include "HAL/FileManager.h"
#include "Kismet/KismetStringLibrary.h"

#if ENGINE_MINOR_VERSION >= 2
#include "Settings/PlatformsMenuSettings.h"
#else
#include "Settings/ProjectPackagingSettings.h"
#endif


#define WIN32_LEAN_AND_MEAN
#include "curl/curl.h"
// 
//#include "IDCGamesBPLibrary.h"


bool FTPUploader::IsUploading = false;
FString FTPUploader::Credentials;
FString FTPUploader::ReleaseID;

FTPUploader::FTPUploader() {
}

void FTPUploader::DoWork() {
	if (FTPUploader::IsUploading) return;
	FTPUploader::IsUploading = true;

#if ENGINE_MINOR_VERSION >= 2
	UPlatformsMenuSettings* PlatformsSettings = GetMutableDefault<UPlatformsMenuSettings>();
#else
	UProjectPackagingSettings* PlatformsSettings = GetMutableDefault<UProjectPackagingSettings>();
#endif

	if (PlatformsSettings->StagingDirectory.Path.IsEmpty()) {
		FTPUploader::IsUploading = false;
		return;
	}
	FString buildPath = PlatformsSettings->StagingDirectory.Path + L"/Windows";

	UUploaderUtilityWidget::ShowMessage(TEXT("Uploading..."));
	curl_global_init(CURL_GLOBAL_DEFAULT);

	IFileManager& MyFileManager = IFileManager::Get();
	TArray<FString> FileNames;
	MyFileManager.FindFilesRecursive(FileNames, *buildPath, L"*.*", true, false, true);
	int files = FileNames.Num();
	for (int i = 0; i < files; i++) {
		FString temp = FileNames[i];
		FString remoteFileName = UKismetStringLibrary::GetSubstring(temp, buildPath.Len(), temp.Len() - buildPath.Len());
		FString progress = FString::Format(TEXT("Uploading {0}/{1}"), { i + 1, files });

		UUploaderUtilityWidget::ShowMessage(progress);
		bool res = UploadFile(buildPath + remoteFileName, remoteFileName, L"ftp://admin.idcgames.com" + remoteFileName, Credentials);
		if (!res) break;
	}

	UUploaderUtilityWidget::HideMessage();

	FTPUploader::IsUploading = false;
	curl_global_cleanup();

	FString uri = FString::Format(TEXT("/api/release/publish/{0}"), { *ReleaseID });

	UIDCGamesBPLibrary::MakeRequest(uri, "{}",
		[this](TSharedPtr<FJsonObject> JsonObject) { UUploaderUtilityWidget::Instance->Refresh(); },
		[this](int errorCode, FString message) {});
}

bool FTPUploader::UploadFile(FString localFilePath, FString remoteFilePath, FString host, FString userpass) {

	CURL* curl = curl_easy_init();
	if (curl) {
		// Configura la URL de destino FTP
		curl_easy_setopt(curl, CURLOPT_URL, TCHAR_TO_ANSI(*host));

		// Configura el usuario y la contrase�a
		curl_easy_setopt(curl, CURLOPT_USERPWD, TCHAR_TO_ANSI(*userpass));

		// Habilita la transferencia FTP
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);

		// Establece la ruta del archivo local
		struct stat file_info;
		unsigned long fsize;
		if (stat(TCHAR_TO_ANSI(*localFilePath), &file_info)) {
			return false;
		}
		fsize = (unsigned long)file_info.st_size;
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);
		FILE* hd_src;
		fopen_s(&hd_src, TCHAR_TO_ANSI(*localFilePath), "rb");
		curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		// Realiza la transferencia
		CURLcode res = curl_easy_perform(curl);

		fclose(hd_src);

		// Verifica si la transferencia fue exitosa
		if (res != CURLE_OK) {
			UE_LOG(LogTemp, Error, TEXT("Error al subir el archivo por FTP: %s"), *FString(curl_easy_strerror(res)));
		}
		else {
			// now extract transfer info
			UE_LOG(LogTemp, Warning, TEXT("Fichero subido ok"));
		}

		// Limpia
		curl_easy_cleanup(curl);

		return (res == CURLE_OK);
	}	
	return false;
}
