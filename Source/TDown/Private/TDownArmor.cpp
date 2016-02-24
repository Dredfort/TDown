// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "TDownArmor.h"
#include "TDownCharacter.h"



// Sets default values
ATDownArmor::ATDownArmor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(30);
	RootComponent = CollisionComp;

	ArmorMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Armormesh"));
	ArmorMesh->AttachTo(RootComponent);

}

void ATDownArmor::OnEquip()
{
	if (OwnerChar)
	{

		ATDownArmor* Armor = Cast<ATDownArmor>(OwnerChar->CurrentArmor);
		//Armor->Destroy();

		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		USkeletalMeshComponent* CharMesh = OwnerChar->GetMesh();
		if (CharMesh)
		{
			ArmorMesh->AttachTo(CharMesh, ArmorSocket);
		}
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

void ATDownArmor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ArmorMesh->StaticMesh == NULL)
	{
		ArmorMesh->SetStaticMesh(ArmorConfig.ArmorMesh);
		ArmorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
