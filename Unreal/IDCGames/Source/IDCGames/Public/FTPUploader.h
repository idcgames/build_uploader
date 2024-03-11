// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

/**
 * 
 */
class FTPUploader : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FTPUploader>;

public:
	static bool IsUploading;
	static FString Credentials;
	static FString ReleaseID;

	FTPUploader();

protected:
	void DoWork();

	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}

	bool UploadFile(FString localFilePath, FString remoteFilePath, FString host, FString userpass);
};