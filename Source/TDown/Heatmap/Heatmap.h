// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Heatmap.generated.h"

class ATDownCharacter;

UCLASS()
class TDOWN_API AHeatmap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHeatmap(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/*_________________________________________________*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_")
	class USplineComponent* PathSpline;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		float TimeBetweenChek = 3;

	UFUNCTION(BlueprintCallable, Category = "_HeatMap")
	void CheckCharacters();
	FTimerHandle ChekTimeHandler;


	UWorld* cWorld;
	ATDownCharacter* CharPtr;
	TArray<ATDownCharacter*> CharactersArr;


	FString SaveDirectoryPath = "E:/UE4_Projects/TDown/T";
	FString LogFileName = "LocationLogs.txt";
	bool VerifyOrCreateDirectory(const FString& SaveDir, const FString& FileName, const FString& StringToSave) const;

	bool SaveArrayToFile(const TArray<int32>& Array, const TCHAR* Filename, IFileManager * FileManager = &IFileManager::Get(), uint32 WriteFlags = 0) const;
	bool LoadFileToArray(TArray<int32>& Result, const TCHAR* Filename, uint32 Flags);
};
