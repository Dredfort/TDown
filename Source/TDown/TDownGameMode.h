// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameMode.h"
#include "TDownGameMode.generated.h"

UENUM(BlueprintType)
enum class EGamePlayerState : uint8
{

	GS_Player			UMETA(DisplayName = "Player"),
	GS_Bot				UMETA(DisplayName="Bot"),
	GS_Unknown			UMETA(DisplayName="Unknown")
};

UCLASS(minimalapi)
class ATDownGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATDownGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "TDown Game")
		void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	UFUNCTION(BlueprintCallable, Category = "TDown Game")
		void SetCurrentState(EGamePlayerState NewState);

	UFUNCTION(BlueprintCallable, Category = "TDown Game")
		EGamePlayerState GetCurrentState() {return CurrentState; }

protected:

	void HandleNewState(EGamePlayerState);

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "TDown Game")
		TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY()
		UUserWidget* CurrentWidget;

private:
	EGamePlayerState CurrentState;

};


