// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "TDownUserWidget.h"
#define LOCTEXT_NAMESPACE "Your Namespace"

FText TestHUDText = LOCTEXT("Your Key", "Your Text");

UTDownUserWidget::UTDownUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	

	WText = ObjectInitializer.CreateDefaultSubobject<UTextBlock>(this,TEXT("T"));
	WText->SetText(TestHUDText);
}

#undef LOCTEXT_NAMESPACE 
