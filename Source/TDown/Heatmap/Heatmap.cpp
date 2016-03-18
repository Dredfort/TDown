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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PathSpline = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("Path"));
	//PathSpline->bSplineHasBeenEdited = true;
	//PathSpline->SetVisibility(true);
	//PathSpline->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AHeatmap::BeginPlay()
{
	Super::BeginPlay();
	
	cWorld = GetWorld();
	if (cWorld)
	{
		CheckCharacters();
		PathSpline->ClearSplinePoints();

	}
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::Green,"Characters on map is: "+ FString::FromInt(CharactersArr.Num()));	
}

// Called every frame
void AHeatmap::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );	
}

void AHeatmap::CheckCharacters()
{
	if (cWorld)
	{
		for (TActorIterator<ATDownCharacter> ActorItr(cWorld); ActorItr; ++ActorItr)
		{
			ATDownCharacter* foundedCh = Cast<ATDownCharacter>(*ActorItr);
			auto isChr = foundedCh->GetClass()->IsChildOf(ATDownCharacter::StaticClass());
			if (isChr)
			{
				CharactersArr.Add(foundedCh);				
			}
		}		
	}
	auto Char = Cast<ATDownCharacter>(CharactersArr[0]);
	if (Char)
	{		
		CharPtr = Char;
		auto  CharLoc = (Char->GetActorLocation()).ToString();
		VerifyOrCreateDirectory(SaveDirectoryPath, LogFileName, CharLoc);
		GetWorldTimerManager().SetTimer(ChekTimeHandler, this, &AHeatmap::CheckCharacters, TimeBetweenChek, true);

		PathSpline->AddSplineLocalPoint(Char->GetActorLocation());
	}
	
}

bool AHeatmap::VerifyOrCreateDirectory(const FString& SaveDir, const FString& FileName, const FString& StringToSave) const
{
	if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDir))				// Dir exist?
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*SaveDir);				// create if it not exist
		if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDir))			//still could not make directory?
		{
			return false;																		//Could not make the specified directory
		}
	}
	
	auto FileToSave = SaveDir + "/" + FileName;

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FileToSave))
	{
		
		return false;
	}
	/*________________________________________________*/

	//FFileHelper::SaveArrayToFile( const TArray<uint8>& Array, const TCHAR* Filename, IFileManager* FileManager /*= &IFileManager::Get()*/, uint32 WriteFlags )
	TArray<int32> Array;
	auto FileTo = SaveDir + "/" + "ArrayLocationLogs.txt";
	
	FVector CALoc = CharPtr->GetActorLocation();
	

	Array.Add(CALoc.X);
	Array.Add(CALoc.Y);
	Array.Add(CALoc.Z);	

	SaveArrayToFile(Array, *FileTo);

	/*________________________________________________*/
	static FString StringOfCoords;
	const TCHAR* StrPtr = *StringToSave;	
	StringOfCoords.Append(StrPtr);
	StringOfCoords.Append("\n");	
	return	FFileHelper::SaveStringToFile(*StringOfCoords, *FileToSave, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}



bool AHeatmap::SaveArrayToFile(const TArray<int32>& Array, const TCHAR * Filename, IFileManager * FileManager, uint32 WriteFlags) const
{
	FArchive* Ar = FileManager->CreateFileWriter(Filename, WriteFlags);
	if (!Ar)
	{
		return 0;
	}
	Ar->Serialize(const_cast<int32*>(Array.GetData()), Array.Num());
	delete Ar;
	return true;	
}

bool AHeatmap::LoadFileToArray(TArray<int32>& Result, const TCHAR * Filename, uint32 Flags)
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


