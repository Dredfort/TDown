// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "Heatmap.h"
#include "TDownCharacter.h"
#include "EngineGlobals.h"
#include "EngineUtils.h"
#include "Components/SplineComponent.h"
#include "ArchiveBase.h"


// Sets default values
AHeatmap::AHeatmap(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	FString StringOfCoords = "";
	it = 0;

	Billboard = ObjectInitializer.CreateDefaultSubobject<UBillboardComponent>(this, TEXT("Billboard"));
	RootComponent = Billboard;
	static ConstructorHelpers::FObjectFinder<UTexture2D> Spalsh(TEXT("Texture2D'/Engine/EditorResources/S_NavP.S_NavP'"));
	if (Spalsh.Succeeded())
	{
		Billboard->SetSprite(Spalsh.Object);
	}

	TextComponent = ObjectInitializer.CreateDefaultSubobject<UTextRenderComponent>(this, TEXT("TextComponent"));
	TextComponent->AttachTo(RootComponent);
	TextComponent->WorldSize = 92;
	TextComponent->bHiddenInGame = false;

	PathSpline = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("PathSplineComponent"));

	BeamParticle = ObjectInitializer.CreateDefaultSubobject<UParticleSystem>(this, TEXT("beamParticle"));
	//BeamParticle->bAutoDeactivate = false;	
}

// Called when the game starts or when spawned
void AHeatmap::BeginPlay()
{
	Super::BeginPlay();

	cWorld = GetWorld();
	if (cWorld)
	{
		if (bCollectCharacters)
		{
			CollectCharacters();
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Characters on map is: " + FString::FromInt(CharactersArr.Num()));		
			if (CharCurrentPtr)
			{
				GetWorldTimerManager().SetTimer(ChekTimeHandler, this, &AHeatmap::CollectData, TimeBetweenChek, true);
			}
		}
	}
}

// Called every frame
void AHeatmap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHeatmap::CollectCharacters()
{
	if (cWorld)
	{
		for (TActorIterator<ACharacter> ActorItr(cWorld); ActorItr; ++ActorItr)
		{
			ATDownCharacter* foundedCh = Cast<ATDownCharacter>(*ActorItr);
			//auto isChr = foundedCh->GetClass()->IsChildOf(ATDownCharacter::StaticClass());

			if (foundedCh)
			{
				CharactersArr.Add(foundedCh);
			}
		}
	}
	if (CharNumberInWorld<CharactersArr.Num())
	{
		auto selectChar = Cast<ATDownCharacter>(CharactersArr[CharNumberInWorld]);
		if (selectChar)
		{
			if (!CharCurrentPtr)
			{
				SetCharacter(selectChar);																								// Set pointer to Character  fom CharNumberInWorld
			}
		}
	}

}

void AHeatmap::CollectData()
{
	//if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDir))											// Dir exist?
	//{
	//	FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*SaveDir);											// create if it not exist
	//	if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDir))										//still could not make directory?
	//	{
	//		return false;																									//Could not make the specified directory
	//	}
	//	auto FileToSave = SaveDir + "/" + FileName;
	//	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FileToSave))	return false;
	//}
	if (bCollectData)
	{
		if (CharCurrentPtr)
		{
			FVector CALoc = CharCurrentPtr->GetActorLocation();

			/*if (SplineDataSwitcher == ESplineDataSwitcher::ES_ArrayData)
			{
				auto FileToSaveArr = SaveDir + "/" + "ArrayLocationLogs.txt";
				static TArray<int16> ArrayToFile;
				ArrayToFile.Add(CALoc.X);
				ArrayToFile.Add(CALoc.Y);
				ArrayToFile.Add(CALoc.Z);
				SaveArrayToFile(ArrayToFile, *FileToSaveArr);
				return true;
			}*/
			if (SplineDataSwitcher == ESplineDataSwitcher::ES_StringData)
			{				
				auto FileToSaveArr = SaveDirectoryPath + "/" + LogFileName + "_" + FString::FromInt(CharNumberInWorld) + ".txt";

				const FString& loc = CALoc.ToString();
				const TCHAR* StrPtr = *loc;

				StringOfCoords.Append(" line");
				StringOfCoords.Append(FString::FromInt(it));
				StringOfCoords.Append(" ");
				StringOfCoords.Append(StrPtr);
				StringOfCoords.Append("\n");
				it++;
				FFileHelper::SaveStringToFile(*StringOfCoords, *FileToSaveArr);//FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM

			}
		}
	}
	if (CharCurrentPtr)
	{
		GetWorldTimerManager().SetTimer(ChekTimeHandler, this, &AHeatmap::CollectData, TimeBetweenChek, true);
	}
}

bool AHeatmap::BuildSplinePath(uint8 CharNumberIn)
{
	if (bBuildSpline)
	{
		if (PathSpline)
		{
			if (PathSpline->GetNumberOfSplinePoints() != 0)
			{
				PathSpline->ClearSplinePoints();
			}
			auto FileFrom = SaveDirectoryPath + "/" + LogFileName + "_" + FString::FromInt(CharNumberIn) + ".txt";
			bool DExists = FPlatformFileManager::Get().GetPlatformFile().FileExists(*FileFrom);//DirectoryExists(*FileFrom);
			if (DExists)///////////////////
			{

				//if (SplineDataSwitcher == ESplineDataSwitcher::ES_ArrayData)
				//{
				//	//TArray<int16> ArrayFromFile;
				//	auto FileFrom = SaveDirectoryPath + "/" + "ArrayLocationLogs.txt";
				//	LoadFileToArray(ArrayFromFile, *FileFrom, 0);
				//	FVector SplinePoint;
				//	SplinePoint.X = 0;
				//	SplinePoint.Y = 0;
				//	SplinePoint.Z = 0;
				//	for (size_t i = 0; i < ArrayFromFile.Num(); i++)
				//	{
				//		SplinePoint.X = (ArrayFromFile[i] < 12800 && ArrayFromFile[i] > -12800 ? ArrayFromFile[i] : GetActorLocation().X);
				//		i++;
				//		SplinePoint.Y = (ArrayFromFile[i] < 12800 && ArrayFromFile[i] > -12800 ? ArrayFromFile[i] : GetActorLocation().Y);
				//		i++;
				//		SplinePoint.Z = (ArrayFromFile[i] < 12800 && ArrayFromFile[i] > -5000 ? ArrayFromFile[i] : GetActorLocation().Z);
				//		PathSpline->AddSplineWorldPoint(SplinePoint);
				//	}
				//}
				if (SplineDataSwitcher == ESplineDataSwitcher::ES_StringData)
				{
					FString StringFromFile;

					FFileHelper::LoadFileToString(StringFromFile, *FileFrom);

					FVector ResultVector;
					ResultVector.X = 0;
					ResultVector.Y = 0;
					ResultVector.Z = 0;

					FString Ptr;
					SplineCoordArr.Empty();

					for (auto itr = 0; itr < StringFromFile.Len(); itr++)
					{
						Ptr = TEXT("line" + FString::FromInt(itr));
						const TCHAR* MatchFind = FCString::Strfind(*StringFromFile, *Ptr);
						if (MatchFind != NULL)
						{
							FParse::Value(MatchFind, TEXT("X="), ResultVector.X);
							FParse::Value(MatchFind, TEXT("Y="), ResultVector.Y);
							FParse::Value(MatchFind, TEXT("Z="), ResultVector.Z);

							PathSpline->AddSplineWorldPoint(ResultVector);
							SplineCoordArr.Add(ResultVector);
						}
						else break;
					}

					if (ParticlesArr.Num() > 0)
					{
						for (auto i = 0; i < ParticlesArr.Num(); i++)
						{

							UParticleSystemComponent* D = Cast<UParticleSystemComponent>(ParticlesArr[i]);
							if (D != NULL)
								D->DeactivateSystem();
						}
						ParticlesArr.Empty();
					}

					int32 numSplinePoints = PathSpline->GetNumberOfSplinePoints();
					for (auto pointIdx = 0; pointIdx < numSplinePoints; pointIdx++)
					{
						const FName Source = FName(TEXT("Source"));
						const FName SourceTan = FName(TEXT("Source_Tan"));
						const FName Target = FName(TEXT("Target"));
						const FName TargetTan = FName(TEXT("Target_Tan"));
						const FName TeamColorParam = FName(TEXT("TeamColor"));

						FVector sourceBeamLocation = PathSpline->GetLocationAtSplinePoint(pointIdx, ESplineCoordinateSpace::World);
						FRotator sourceBeamRot;
						sourceBeamRot.Pitch = 0;
						sourceBeamRot.Yaw = 0;
						sourceBeamRot.Roll = 0;
						float sourceDistanceAlongSpline = PathSpline->GetDistanceAlongSplineAtSplinePoint(pointIdx);
						FVector sourceTangentAtDistance = PathSpline->GetTangentAtDistanceAlongSpline(sourceDistanceAlongSpline, ESplineCoordinateSpace::World);

						FVector targetBeamLocation = PathSpline->GetLocationAtSplinePoint(pointIdx + 1, ESplineCoordinateSpace::World);
						float targetDistanceAlongSpline = PathSpline->GetDistanceAlongSplineAtSplinePoint(pointIdx + 1);
						FVector targetTangentAtDistance = PathSpline->GetTangentAtDistanceAlongSpline(targetDistanceAlongSpline, ESplineCoordinateSpace::World);

						if (GetWorld())
						{
							UParticleSystemComponent* Spawner = UGameplayStatics::SpawnEmitterAtLocation(this, BeamParticle, sourceBeamLocation, sourceBeamRot, true);////ftom world to this

							UParticleSystemComponent* chek = Cast<UParticleSystemComponent>(Spawner);
							if (chek)
							{
								ParticlesArr.Add(Spawner);
								if (Spawner)
								{
									Spawner->SetVectorParameter(Source, sourceBeamLocation);
									Spawner->SetVectorParameter(SourceTan, sourceTangentAtDistance);
									Spawner->SetVectorParameter(Target, targetBeamLocation);
									Spawner->SetVectorParameter(TargetTan, targetTangentAtDistance);
									Spawner->SetColorParameter(TeamColorParam, TeamColor);
								}
							}
						}
						
					}
					return true;//
				}
			}
			
		}
	}
	return false;
}

bool AHeatmap::SaveArrayToFile(const TArray<int16>& Array, const TCHAR * Filename, IFileManager * FileManager, uint32 WriteFlags) const
{
	FArchive* Ar = FileManager->CreateFileWriter(Filename, WriteFlags);
	if (!Ar)
	{
		return 0;
	}
	Ar->Serialize(const_cast<int16*>(Array.GetData()), Array.Num());
	delete Ar;
	return true;
}

bool AHeatmap::LoadFileToArray(TArray<int16>& Result, const TCHAR * Filename, uint32 Flags)
{
	FArchive* Reader = IFileManager::Get().CreateFileReader(Filename, Flags);
	if (!Reader)
	{
		if (!(Flags & FILEREAD_Silent))
		{
			UE_LOG(LogStreaming, Warning, TEXT("Failed to read file '%s' error."), Filename);
		}
		return 0;
	}
	int64 TotalSize = Reader->TotalSize();
	Result.Reset(TotalSize);
	Result.AddUninitialized(TotalSize);
	Reader->Serialize(Result.GetData(), Result.Num());
	bool Success = Reader->Close();
	delete Reader;
	return Success;
}

void AHeatmap::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (ParticlesArr.Num() > 0)
	{
		for (auto i = 0; i < ParticlesArr.Num(); i++)
		{

			UParticleSystemComponent* D = Cast<UParticleSystemComponent>(ParticlesArr[i]);
			if (D != NULL)
				D->DeactivateSystem();
		}
		ParticlesArr.Empty();
	}
	FText CharctesP= FText::FromString(FString::FromInt(CharNumberInWorld));// = FString::FromInt(CharNumberInWorld);
	TextComponent->SetText(CharctesP);
	BuildSplinePath(CharNumberInWorld);

}

void AHeatmap::BeginDestroy() //////////////////////
{
	Super::BeginDestroy();

	if (ParticlesArr.Num() > 0)
	{
		for (auto i = 0; i < ParticlesArr.Num(); i++)
		{

			UParticleSystemComponent* D = Cast<UParticleSystemComponent>(ParticlesArr[i]);
			if (D != NULL)
				D->DeactivateSystem();
		}
		ParticlesArr.Empty();
	}
	
}

void AHeatmap::SetCharacter(ATDownCharacter * newCharacter)
{
	CharCurrentPtr = newCharacter;
}





