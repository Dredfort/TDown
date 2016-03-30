// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "HeatmapManager.h"
#include "TDownCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "HeatmapDataCollector.h"
#include "HeatmapTileActor.h"


// Sets default values
AHeatmapManager::AHeatmapManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ManagerBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("ManagerBillboard"));
	ManagerBillboard->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UTexture2D> Spalsh(TEXT("Texture2D'/Engine/EditorResources/Spawn_Point.Spawn_Point'"));
	if (Spalsh.Succeeded())
	{
		ManagerBillboard->SetSprite(Spalsh.Object);
	}

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextComponent"));
	TextComponent->AttachTo(RootComponent);
	TextComponent->WorldSize = 92;
	TextComponent->bHiddenInGame = false;
}

// Called when the game starts or when spawned
void AHeatmapManager::BeginPlay()
{
	Super::BeginPlay();

	CollectCharacters();
	CollectHeatOnLevel();
	SetUpHeatmaps();
	StartCollectData(bCollectDataFromHM);
}

void AHeatmapManager::CollectCharacters()
{
	if (GetWorld())
	{
		TArray<AActor*> FoundActorsArr;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), CharacterClassToFind, FoundActorsArr);

		for (auto i = 0; i < FoundActorsArr.Num(); i++)
		{
			ATDownCharacter* FoundActor = Cast<ATDownCharacter>(FoundActorsArr[i]);
			if (FoundActor)
			{
				FoundedCharactersArr.Add(FoundActor);
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Founded by HeatmapManager is " + FString::FromInt(FoundedCharactersArr.Num()) + " characters");
	}
}

void AHeatmapManager::CollectHeatOnLevel()
{
	TArray<AActor*> FoundActorsArr;
	if (GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), HeatmapToSpawn, FoundActorsArr);

		for (auto i = 0; i < FoundActorsArr.Num(); i++)
		{
			AHeatmapDataCollector* FoundActor = Cast<AHeatmapDataCollector>(FoundActorsArr[i]);
			if (FoundActor && !FoundedHeatmapArr.Contains(FoundActor))
			{
				FoundedHeatmapArr.Add(FoundActor);
				FoundActor->SetOwner(this);
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Founded by HeatmapManager is " + FString::FromInt(FoundedHeatmapArr.Num()) + " heatmaps on level");
	}
}

void AHeatmapManager::SpawnHeatmapsOnLevel()
{

	if (FoundedHeatmapArr.Num() < FoundedCharactersArr.Num())
	{
		FTransform SpawnTS;
		//SpawnTS.SetLocation();

		AHeatmapDataCollector* SpawnHM = Cast<AHeatmapDataCollector>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), HeatmapToSpawn, SpawnTS));

		if (SpawnHM)
		{
		}
		UGameplayStatics::FinishSpawningActor(SpawnHM, SpawnTS);
	}
}

void AHeatmapManager::SetUpHeatmaps()
{
	int8 i = 0;
	for (i = 0; i < FoundedHeatmapArr.Num(); i++)
	{
		if (FoundedHeatmapArr[i] != NULL)
		{
			AHeatmapDataCollector* Setuper = Cast<AHeatmapDataCollector>(FoundedHeatmapArr[i]);
			if (Setuper)
			{
				Setuper->bCollectCharacters = false;
				Setuper->UpdateTimeBetweenChek = UpdTime;
				Setuper->bBuildSpline = bBuildSplinesFromHM;
				Setuper->bCollectData = bCollectDataFromHM;
				if (FoundedCharactersArr.Num() != 0 && FoundedCharactersArr.Num() > i)/////
				{
					Setuper->SetCharacter(FoundedCharactersArr[i]);
				}
				Setuper->SetCharNumberInWorld(i);
			}
		}
	}
}

void AHeatmapManager::StartCollectData(bool state)
{
	for (int i = 0; i < FoundedHeatmapArr.Num(); i++)
	{
		auto DataCollector = Cast<AHeatmapDataCollector>(FoundedHeatmapArr[i]);
		DataCollector->CollectData();
	}
}

void AHeatmapManager::BuildSplines(bool state)
{
	for (int i = 0; i < FoundedHeatmapArr.Num(); i++)
	{
		auto SplineBuilder = Cast<AHeatmapDataCollector>(FoundedHeatmapArr[i]);
		if (FoundedCharactersArr.Num() != 0)
		{
			SplineBuilder->SetCharacter(FoundedCharactersArr[i]);
		}
		SplineBuilder->SetCharNumberInWorld(i);
		SplineBuilder->BuildSplinePath(i, state);
	}
}

void AHeatmapManager::CollectDataFromFiles()
{
	CoordFromSplineArr.Empty();

	for (auto i = 0; i < FoundedHeatmapArr.Num(); i++)
	{
		auto CharNumberIn = i;

		auto FileFrom = SaveDirectoryPath + "/" + LogFileName + "_" + FString::FromInt(CharNumberIn) + ".txt";
		bool DExists = FPlatformFileManager::Get().GetPlatformFile().FileExists(*FileFrom);//DirectoryExists(*FileFrom);
		if (DExists)
		{
			FString StringFromFile;
			FFileHelper::LoadFileToString(StringFromFile, *FileFrom);

			FVector ResultVector;
			FVector ResultVectorFix;
			/*ResultVector.X = 0;
			ResultVector.Y = 0;
			ResultVector.Z = 0;*/

			FString Ptr;

			for (auto itr = 0; itr < StringFromFile.Len(); itr++)
			{
				Ptr = TEXT("line" + FString::FromInt(itr));
				const TCHAR* MatchFind = FCString::Strfind(*StringFromFile, *Ptr);
				if (MatchFind != NULL)
				{
					FParse::Value(MatchFind, TEXT("X="), ResultVector.X);
					FParse::Value(MatchFind, TEXT("Y="), ResultVector.Y);
					FParse::Value(MatchFind, TEXT("Z="), ResultVector.Z);

					CoordFromSplineArr.Add(ResultVector);

					/// fix coordinates to make it aliquot to 100
					auto fixX = FMath::Fmod(ResultVector.X, 100);
					auto fixY = FMath::Fmod(ResultVector.Y, 100);
					auto fixZ = FMath::Fmod(ResultVector.Z, 100);

					if (fixX > 50)
					{
						ResultVectorFix.X = (ResultVector.X - fixX) + 100;
					}
					else ResultVectorFix.X = ResultVector.X - fixX;

					if (fixY > 50)
					{
						ResultVectorFix.Y = (ResultVector.Y - fixY) + 100;
					}
					else ResultVectorFix.Y = ResultVector.Y - fixY;

					if (fixZ > 50)
					{
						ResultVectorFix.Z = 0; //(ResultVector.Z - fixZ) + 100;
					}
					else ResultVectorFix.Z = 0;//ResultVector.Z - fixZ;

					FixedCoordinatesArr.Add(ResultVectorFix);
					// start so spawning grid

				}
				else break;
			}
		}
		else break;
	}

}

void AHeatmapManager::BuildHeatMapTiles(bool isBuild)
{

	//destroy all actors builder before
	if (!isBuild && BuildedTilesArr.Num() > 0 && GetWorld())
	{
		TArray<AActor*> FoundActorsArr;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), TileToSpawn, FoundActorsArr);
		if (FoundActorsArr.Num() > 0)
		{
			for (auto i = 0; i < FoundActorsArr.Num(); i++)
			{
				auto FoundActor = Cast<AHeatmapTileActor>(FoundActorsArr[i]);
				if (FoundActor)
				{
					FoundActor->Destroy();;
				}
			}
		}
		for (auto i = 0; i < BuildedTilesArr.Num(); i++)
		{
			AHeatmapTileActor* D = Cast<AHeatmapTileActor>(BuildedTilesArr[i]);
			if (D != NULL)
				D->Destroy();
		}
		BuildedTilesArr.Empty();
	}
	else 
		if (isBuild)
		{

			//TMap<FVector, int>ControllMap;//for debug
			for (auto i = 0; i < FixedCoordinatesArr.Num(); i++)
			{
				if (TileToSpawn)
				{
					FVector SpawnLoc;
					SpawnLoc.X = FixedCoordinatesArr[i].X;
					SpawnLoc.Y = FixedCoordinatesArr[i].Y;
					SpawnLoc.Z = GetActorLocation().Z;//FixedCoordinatesArr[i].Z


					if (ControllMap.Contains(SpawnLoc))
					{
						ControllMap.Emplace(SpawnLoc, ControllMap.FindRef(SpawnLoc) + 1);
						//const FVector* Ptr = ControllMap.FindKey(SpawnLoc);
						auto mapVal = ControllMap.FindRef(SpawnLoc);
						if (mapVal > HMax)
						{
							HMax = mapVal;
						}
					}
					else	ControllMap.Add(SpawnLoc, 1);
				}
			}
			for (auto It = ControllMap.CreateIterator(); It; ++It)
			{
				FTransform SpawnTS;

				FVector LocToSpawnTile = It.Key();

				SpawnTS.SetLocation(LocToSpawnTile);

				AActor* SpawnTile = Cast<AActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TileToSpawn, SpawnTS));
				if (SpawnTile)
				{

					UGameplayStatics::FinishSpawningActor(SpawnTile, SpawnTS);

					AHeatmapTileActor* MComponent = Cast<AHeatmapTileActor>(SpawnTile);

					if (!BuildedTilesArr.Contains(SpawnTile))
					{
						if (MComponent)
						{
							//UMaterialInterface* mOverrider = MComponent->GetMeshTile()->GetMaterial(0);
							UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MatToOverride);
							if (MatToOverride && !MID)
							{
								// Create and set the dynamic material instance.
								FLinearColor mPColor;
								MID = UMaterialInstanceDynamic::Create(MatToOverride, this);
								auto ColorR = It.Value();
								//ColorR = ColorR*0.1;
								//mPColor.R = FMath::Clamp(mPColor.R,0.0f,1.0f);
								mPColor.R = ColorR*0.1;
								MID->SetVectorParameterValue("Color", mPColor);
								MComponent->GetMeshTile()->SetMaterial(0, MID);
								MComponent->GetMeshTile()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
							}
						}
					}
				}
				BuildedTilesArr.Add(SpawnTile);
			}
		}
}

void AHeatmapManager::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//CollectCharacters();
	CollectHeatOnLevel();
	SetUpHeatmaps();
	BuildSplines(bBuildSplinesFromHM);

	FText CharctesP = FText::FromString(FString::FromInt(FoundedHeatmapArr.Num()));// = FString::FromInt(CharNumberInWorld);
	TextComponent->SetText(CharctesP);

	if (bRefreshDta)
	{
		CollectDataFromFiles();
	}
	BuildHeatMapTiles(bRefreshDta);
}
