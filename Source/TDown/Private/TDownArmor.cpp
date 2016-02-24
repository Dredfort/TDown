// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "TDownArmor.h"


// Sets default values
ATDownArmor::ATDownArmor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ArmorMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Armormesh"));
	ArmorMesh->AttachTo(RootComponent);

}

void ATDownArmor::OnEquip()
{
	if (OwnerChar)
	{
		USkeletalMeshComponent* CharMesh = OwnerChar->GetMesh();
		
		ArmorMesh->AttachTo(CharMesh, ArmorConfig.ArmorSocket);
	}
}

void ATDownArmor::SetOwningPawn(ATDownCharacter * NewOwner)
{
	if (OwnerChar != NewOwner)
	{
		Instigator = NewOwner;
		OwnerChar = NewOwner;
	}
}
