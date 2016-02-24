// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "TDownUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TDOWN_API UTDownUserWidget : public UUserWidget
{
	GENERATED_BODY()

	UTDownUserWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	//FText* WText;
	UTextBlock* WText;
};
