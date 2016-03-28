// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HeatmapManager.generated.h"

class AHeatmapDataCollector;
class ATDownCharacter;

UCLASS()
class TDOWN_API AHeatmapManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHeatmapManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
		UBillboardComponent* ManagerBillboard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
		UTextRenderComponent* TextComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
	TSubclassOf<AActor> TileToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
		bool bRefresh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
		TSubclassOf<class AHeatmapDataCollector> HeatmapToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
		TSubclassOf<class ACharacter> CharacterClassToFind;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
		bool bBuildSplinesFromHM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
		bool bCollectDataFromHM;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatmapManager_")
		float UpdTime = 1;


	UPROPERTY(BlueprintReadWrite, Category = "_HeatmapManager_")
	TArray<FVector> CoordFromSplineArr;
	UPROPERTY(BlueprintReadWrite, Category = "_HeatmapManager_")
	TArray<FVector>FixedCoordinatesArr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		FString SaveDirectoryPath = "E:/UE4_Projects/TDown/HeatmapLogFolder";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_HeatMap")
		FString LogFileName = "LocationLogs";

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;		

	void CollectCharacters();
	void CollectHeatOnLevel();
	void SetUpHeatmaps();
	void SpawnHeatmapsOnLevel();
	void StartCollectData(bool state=false);
	void BuildSplines(bool state);

	void CollectDataFromFiles();
	void BuildHeatMapTiles(bool isBuild);

	TArray<ATDownCharacter*> FoundedCharactersArr;
	TArray<AHeatmapDataCollector*> FoundedHeatmapArr;
	//private:
	TArray<AActor*> BuildedTilesArr;
	
	/*void BeginDestroy() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason)override;*/
	void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)override;
};
