// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "TDownProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "TDownBullet.generated.h"

class ATDownWeapon;

UCLASS()
class TDOWN_API ATDownBullet : public AActor
{
	GENERATED_BODY()

		ATDownBullet(const FObjectInitializer& ObjectInitializer);
public:	
	
	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "-Projectile")
		UTDownProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		UParticleSystemComponent* PSystem;

	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		UParticleSystem* HitParticle;

	UFUNCTION()
		void OnCollision(AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void SetHitParticle(UParticleSystem* P) { HitParticle = P; };

	ATDownWeapon* WeaponOwner;

	int32 CurrentDamage;
	
};
