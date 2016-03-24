// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Heatmap.generated.h"

class ATDownCharacter;

UENUM(BlueprintType)
enum class ESplineDataSwitcher : uint8
{
	ES_StringData		UMETA(DisplayName = "StringData"),
	ES_ArrayData		UMETA(DisplayName ="ArrayData")
};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		ESplineDataSwitcher SplineDataSwitcher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
	UBillboardComponent* Billboard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		UParticleSystem * BeamParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		UTextRenderComponent* TextComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
	class USplineComponent* PathSpline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		float TimeBetweenChek = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		bool bBuildSpline = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		bool bCollectData = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		bool bCollectCharacters = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		FLinearColor TeamColor;
	// character that be used for collect data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		uint8 CharNumberInWorld = 0;
	ATDownCharacter* CharCurrentPtr;
	UFUNCTION(BlueprintCallable, Category = "_HeatMap")
		void CollectCharacters();

	UFUNCTION(BlueprintCallable, Category = "_HeatMap")
		bool BuildSplinePath( uint8 CharNumberIn = 0 );
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
	FString SaveDirectoryPath = "E:/UE4_Projects/TDown/HeatmapLogFolder";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
	FString LogFileName = "LocationLogs";

	int64 it;
	void CollectData();

	bool SaveArrayToFile(const TArray<int16>& Array, const TCHAR* Filename, IFileManager * FileManager = &IFileManager::Get(), uint32 WriteFlags = 0) const;
	bool LoadFileToArray(TArray<int16>& Result, const TCHAR* Filename, uint32 Flags);
	TArray<int16> ArrayFromFile;
	
	//void OnConstruction(const FTransform& Transform) override;
	void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)override;
	void BeginDestroy() override;

	TArray<UParticleSystemComponent*> ParticlesArr;
	void SetCharacter(ATDownCharacter* newCharacter);
private:
	FString StringOfCoords;
	TArray<FVector> SplineCoordArr;
	TArray<ATDownCharacter*> CharactersArr;
	UWorld* cWorld;
	FTimerHandle ChekTimeHandler;
};
