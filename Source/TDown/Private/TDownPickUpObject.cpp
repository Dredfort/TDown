// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "TDownPickUpObject.h"
#include "TDownCharacter.h"
#include "TDownWeapon.h"


// Sets default values
ATDownPickUpObject::ATDownPickUpObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this,TEXT("Collision"));
	RootComponent = CollisionComp;
	CollisionComp->InitSphereRadius(50);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this,&ATDownPickUpObject::OnOverlap);

	PickUpMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("mesh"));
	PickUpMesh->AttachTo(CollisionComp);
	
}

// Called when the game starts or when spawned
void ATDownPickUpObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDownPickUpObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATDownPickUpObject::OnOverlap(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
}

