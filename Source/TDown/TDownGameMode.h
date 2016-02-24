// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameMode.h"
#include "TDownGameMode.generated.h"

UCLASS(minimalapi)
class ATDownGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATDownGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "UMG Game")
		void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY()
		UUserWidget* CurrentWidget;

};



