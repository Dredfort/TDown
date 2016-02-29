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

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Collision"));
	CollisionComp->InitSphereRadius(5);
	RootComponent = CollisionComp;
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ATDownBullet::OnCollision);
 

	PSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("PS"));
	PSystem->AttachTo(CollisionComp);

	BulletMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Bulletmesh"));
	BulletMesh->AttachParent = CollisionComp;;


	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UTDownProjectileMovementComponent>(this, TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000;
	ProjectileMovement->MaxSpeed = 3500;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	
	

	//ProjectileMovement->Velocity = FVector::ZeroVector;
	/** How long this Actor lives before dying**/
	InitialLifeSpan = 5;
}

void ATDownBullet::OnCollision(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	ATDownCharacter* Enemy = Cast<ATDownCharacter>(OtherActor);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, SweepResult.Location);

	if ((OtherActor != NULL) && (OtherActor !=this) && (OtherComp != NULL) )
	{
		if (Enemy)
		{
			if (Enemy!=GetOwner())
			{
				GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("HIT!!!"));
				OtherComp->AddImpulseAtLocation(GetVelocity() * 100, GetActorLocation());
				//Enemy->SetAliveState(false);
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



