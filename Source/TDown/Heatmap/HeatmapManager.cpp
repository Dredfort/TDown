// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "HeatmapManager.h"
#include "TDownCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Heatmap.h"


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
	CollectData(true);
}

void AHeatmapManager::CollectCharacters()
{	
	TArray<AActor*> FoundActorsArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), CharacterClassToFind, FoundActorsArr);

	for (auto i = 0; i < FoundActorsArr.Num();i++)
	{
		auto FoundActor = Cast<ATDownCharacter>(FoundActorsArr[i]);
		if (FoundActor)
		{
			FoundedCharactersArr.Add(FoundActor);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Founded by HeatmapManager is " + FString::FromInt(FoundedCharactersArr.Num())+" characters");
}

void AHeatmapManager::CollectHeatOnLevel()
{
	TArray<AActor*> FoundActorsArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), HeatmapToSpawn, FoundActorsArr);

	for (auto i = 0; i < FoundActorsArr.Num(); i++)
	{
		auto FoundActor = Cast<AHeatmap>(FoundActorsArr[i]);
		if (FoundActor && !FoundedHeatmapArr.Contains(FoundActor))
		{
			FoundedHeatmapArr.Add(FoundActor);
			FoundActor->SetOwner(this);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Founded by HeatmapManager is " + FString::FromInt(FoundedHeatmapArr.Num()) + " heatmaps on level");
}

void AHeatmapManager::SetUpHeatmaps()
{
	int8 i = 0;
	for ( i = 0; i < FoundedHeatmapArr.Num(); i++)
	{
		AHeatmap* Setuper = Cast<AHeatmap>(FoundedHeatmapArr[i]);
		if (Setuper)
		{
			Setuper->bCollectCharacters = false;
			Setuper->UpdateTimeBetweenChek = UpdTime;
			Setuper->bBuildSpline = bBuildSplinesFromHM;
			Setuper->bCollectData = bCollectDataFromHM;
			if (FoundedCharactersArr.Num() !=0)
			{
				Setuper->SetCharacter(FoundedCharactersArr[i]);
			}
			Setuper->SetCharNumberInWorld(i);
		}
	}
}

void AHeatmapManager::SpawnHeatmapsOnLevel()
{

	if (FoundedHeatmapArr.Num()<FoundedCharactersArr.Num())
	{
		FTransform SpawnTS;
		//SpawnTS.SetLocation();

		AHeatmap* SpawnHM = Cast<AHeatmap>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), HeatmapToSpawn, SpawnTS));

		if (SpawnHM)
		{
		}
		UGameplayStatics::FinishSpawningActor(SpawnHM, SpawnTS);
	}
}

void AHeatmapManager::CollectData(bool state)
{
	for (int i = 0; i < FoundedHeatmapArr.Num();i++)
	{
		auto DataCollector = Cast<AHeatmap>(FoundedHeatmapArr[i]);
		DataCollector->CollectData();
	}
}

void AHeatmapManager::BuildSplines(bool state)
{
		for (int i = 0; i < FoundedHeatmapArr.Num(); i++)
		{
			auto SplineBuilder = Cast<AHeatmap>(FoundedHeatmapArr[i]);
			if (FoundedCharactersArr.Num() != 0)
			{
				SplineBuilder->SetCharacter(FoundedCharactersArr[i]);
			}
			SplineBuilder->SetCharNumberInWorld(i);
			SplineBuilder->BuildSplinePath(i, state);

			FromSplineCoordArr.Append(SplineBuilder->GetArrayOfSplineCoords()); /// collecting all spline coordinates to the array.
		}
	if (state)
	{
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
}