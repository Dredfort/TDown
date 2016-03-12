// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "TDownBullet.h"
#include "Engine.h"
#include "TDownCharacter.h"
#include "TDownWeapon.h"
#include "TDownProjectileMovementComponent.h"

// Sets default values
ATDownBullet::ATDownBullet(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentDamage = 30;
	//WeaponTarget = NULL;	

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Collision"));
	CollisionComp->InitSphereRadius(5);
	RootComponent = CollisionComp;
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ATDownBullet::OnCollision);

	SeekSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SeekSphere"));
	SeekSphere->AttachTo(CollisionComp);
	SeekSphere->InitSphereRadius(100);
	SeekSphere->OnComponentBeginOverlap.AddDynamic(this, &ATDownBullet::SeekCollision);
 

	PSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("PS"));
	PSystem->AttachTo(CollisionComp);

	BulletMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Bulletmesh"));
	BulletMesh->AttachParent = CollisionComp;;


	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UTDownProjectileMovementComponent>(this, TEXT("ProjectileCustomMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 500;
	ProjectileMovement->MaxSpeed = 1000;
	ProjectileMovement->bRotationFollowsVelocity =false ;
	//ProjectileMovement->Velocity = FVector::ZeroVector;
	ProjectileMovement->ProjectileGravityScale = 0;

	
	

	
	/** How long this Actor lives before dying**/
	InitialLifeSpan = 20;
}

void ATDownBullet::BeginPlay()
{
	Super::BeginPlay();

	ATDownCharacter* TCharacter = Cast<ATDownCharacter>(TargetActor);
	if (TCharacter)
	{
		ProjectileMovement->SetTarget(TCharacter);
	}
	else
	{
		FVector Direction = GetActorLocation() - pTargetPoint;
		ProjectileMovement->SetTargetDirection(pTargetPoint, Direction);
	}
}

void ATDownBullet::OnCollision(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	ATDownCharacter* Enemy = Cast<ATDownCharacter>(OtherActor);


	if ((OtherActor != NULL) && (OtherActor !=this) && (OtherComp != NULL) )
	{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, SweepResult.Location);
		if (Enemy)
		{
			if (Enemy!=GetOwner())
			{
				GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("HIT!!!"));
				OtherComp->AddImpulseAtLocation(GetVelocity() * 100, GetActorLocation());
				Enemy->SetDamage(CurrentDamage);
				Destroy();
			}
		}
		else
		{
			Destroy();
		}
	}	
}

void ATDownBullet::SeekCollision(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	auto CTarget = Cast<ATDownCharacter>(OtherActor);	
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		
	}
}


void ATDownBullet::SetCharOwner(ATDownCharacter * NewOwner)
{
	CharOwner = NewOwner;
}




