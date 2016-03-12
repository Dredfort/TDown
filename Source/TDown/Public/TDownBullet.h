// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "TDownProjectileMovementComponent.h"
#include "TDownBullet.generated.h"

class ATDownWeapon;
class ATDownCharacter;

UCLASS()
class TDOWN_API ATDownBullet : public AActor
{
	GENERATED_BODY()

		ATDownBullet(const FObjectInitializer& ObjectInitializer);
public:	
	
	void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		USphereComponent* SeekSphere;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "-Projectile")
		UTDownProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		UParticleSystemComponent* PSystem;

	UPROPERTY(VisibleDefaultsOnly, Category = "-Projectile")
		UParticleSystem* HitParticle;

	UFUNCTION()
		void OnCollision(AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	UFUNCTION()
		void SeekCollision(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void SetHitParticle(UParticleSystem* P) { HitParticle = P; };



	FVector pTargetPoint;
	AActor* TargetActor;
	TArray<AActor*>ActorsToIgnore;
	UFUNCTION()
		void SeTargetActor(AActor* TargetCtor) { TargetActor = TargetCtor; };
	UFUNCTION()
		void SetTargetPoint(FVector pTarPoint) { pTargetPoint = pTarPoint; };	

	void SetCharOwner(ATDownCharacter* NewOwner);

	ATDownWeapon* WeaponOwner;
	ATDownCharacter* WeaponTarget;
	ATDownCharacter* CharOwner;

	int32 CurrentDamage;
	
};
