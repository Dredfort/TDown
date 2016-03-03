// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TDown.h"
#include "TDownPlayerController.h"
#include "TDownCharacter.h"
#include "AI/Navigation/NavigationSystem.h"
#include "TDownGameMode.h"

ATDownPlayerController::ATDownPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	bIsCanMoove = false;

}

void ATDownPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	//enum EState =  CurrentGameMode->GetCurrentState();

	switch (CurrentGameMode->GetCurrentState())
	{
	case EGamePlayerState::GS_Player:
	{
		bIsCanMoove = true;
		break;
	}
	case EGamePlayerState::GS_Bot:
	{
		bIsCanMoove = false;
		break;
	}
	case EGamePlayerState::GS_Unknown:
	{
		break;
	}
	default:
		bIsCanMoove = true;
		break;
	}

	/*if (CurrentGameMode->GetCurrentState() == EGamePlayerState::GS_Player)
	{
		bIsCanMoove = true;
	}
	else
	{
		if (CurrentGameMode->GetCurrentState() == EGamePlayerState::GS_Bot)
		{
			bIsCanMoove = false;
		}
		else
		{
			bIsCanMoove = true;
		}
	}*/


	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor && bIsCanMoove)
	{
		MoveToMouseCursor();
	}
}

void ATDownPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATDownPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ATDownPlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("Action", IE_Pressed, this, &ATDownPlayerController::OnActionPressed);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &ATDownPlayerController::NextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ATDownPlayerController::PrevWeapon);


	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATDownPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ATDownPlayerController::MoveToTouchLocation);
}

void ATDownPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CurrentGameMode = Cast<ATDownGameMode>(UGameplayStatics::GetGameMode(this));
}

void ATDownPlayerController::OnActionPressed()
{
	ATDownCharacter* GetChar = Cast<ATDownCharacter>(GetPawn());
	if (GetChar)
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);
			GetChar->FireWeapon(Hit.ImpactPoint);
	
	}
}

void ATDownPlayerController::NextWeapon()
{
	ATDownCharacter* GetChar = Cast<ATDownCharacter>(GetPawn());
	if (GetChar)
	{
		GetChar->NextWeapon();
	}
}

void ATDownPlayerController::PrevWeapon()
{
	ATDownCharacter* GetChar = Cast<ATDownCharacter>(GetPawn());
	if (GetChar)
	{
		GetChar->PrevWeapon();
	}
}



void ATDownPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ATDownPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ATDownPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());


		//NavSys->RegisterNavigationInvoker(Pawn);												/*		testing			  */		

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ATDownPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ATDownPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
