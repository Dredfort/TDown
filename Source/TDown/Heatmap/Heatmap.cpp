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

	PathSpline = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("PathSplineComponent"));
}

// Called when the game starts or when spawned
void AHeatmap::BeginPlay()
{
	Super::BeginPlay();

	cWorld = GetWorld();
	if (cWorld)
	{
		CheckCharacters();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Characters on map is: " + FString::FromInt(CharactersArr.Num()));

		BuildSplinePath();
	}
}

// Called every frame
void AHeatmap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHeatmap::CheckCharacters()
{
	if (cWorld)
	{
		for (TActorIterator<ACharacter> ActorItr(cWorld); ActorItr; ++ActorItr)
		{
			ATDownCharacter* foundedCh = Cast<ATDownCharacter>(*ActorItr);
			auto isChr = foundedCh->GetClass()->IsChildOf(ATDownCharacter::StaticClass());			
			
			if (foundedCh)
			{
				CharactersArr.Add(foundedCh);
			}
		}
	}
	auto selectChar = Cast<ATDownCharacter>(CharactersArr[CharPtrNumber]);
	if (selectChar)
	{
		CharPtr = selectChar;																								// Set pointer to Character 
	}
	if (CharPtr)
	{		
		MakeStringFromCoordinates(SaveDirectoryPath, LogFileName);
		GetWorldTimerManager().SetTimer(ChekTimeHandler, this, &AHeatmap::CheckCharacters, TimeBetweenChek, true);			// check characters recursively
	}

}

bool AHeatmap::MakeStringFromCoordinates(FString& SaveDir, FString& FileName)
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
		FVector CALoc = CharPtr->GetActorLocation();

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
			//static int64 it;
			//static FString StringOfCoords;
			auto FileToSaveArr = SaveDir + "/" + LogFileName + "_" + FString::FromInt(CharPtrNumber) + ".txt";
			
			const FString& loc = CALoc.ToString();
			const TCHAR* StrPtr = *loc;// *StringToSave;

			StringOfCoords.Append(" line");
			StringOfCoords.Append(FString::FromInt(it));
			StringOfCoords.Append(" ");
			StringOfCoords.Append(StrPtr);
			StringOfCoords.Append("\n");
			it++;


			FFileHelper::SaveStringToFile(*StringOfCoords, *FileToSaveArr);//FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM
			return true;
		}
	}
	return false;
}

void AHeatmap::BuildSplinePath()
{
	if (bBuildSpline)
	{
		PathSpline->ClearSplinePoints();

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
			auto FileFrom = SaveDirectoryPath + "/" + LogFileName + "_" + FString::FromInt(CharPtrNumber) + ".txt";
			FString StringFromFile;

			FFileHelper::LoadFileToString(StringFromFile, *FileFrom);

			FVector ResultVector;
			ResultVector.X = 0;
			ResultVector.Y = 0;
			ResultVector.Z = 0;

			FString Ptr;

			for (size_t itr = 0; itr < StringFromFile.Len(); itr++)
			{
				Ptr = TEXT("line" + FString::FromInt(itr));
				const TCHAR* MatchFind = FCString::Strfind(*StringFromFile, *Ptr);

				FParse::Value(MatchFind, TEXT("X="), ResultVector.X);
				FParse::Value(MatchFind, TEXT("Y="), ResultVector.Y);
				FParse::Value(MatchFind, TEXT("Z="), ResultVector.Z);

				PathSpline->AddSplineWorldPoint(ResultVector);
			}
		}
	}
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
	int64 TotalSize = Reader->TotalSize();//int64
	Result.Reset(TotalSize);
	Result.AddUninitialized(TotalSize);
	Reader->Serialize(Result.GetData(), Result.Num());
	bool Success = Reader->Close();
	delete Reader;
	return Success;
}



