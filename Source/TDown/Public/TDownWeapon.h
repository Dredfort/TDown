// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TDownCharacter.h"
#include "GameFramework/Actor.h"
#include "TDownWeapon.generated.h"



#define TRACE_WEAPON ECC_GameTraceChannel1

UENUM(BlueprintType)
enum class EWeaponProjectile : uint8
{
	EP_Bullet			UMETA(DisplayName="Bullet"),
	EP_Spread			UMETA(DisplayName = "Spread"),
	EP_Projectile		UMETA(DisplayName = "Projectile")
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		int32 MaxClip;
	//for automatic weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		float TimeBetweenShots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 ShotCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		float WeaponSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		int32 WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		UTexture2D* SplashArt;

	// numbet of slot in inventory array
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		USkeletalMesh * WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		UStaticMesh* BulletMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		UParticleSystem* MuzzleParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
		UParticleSystem* HitPatricle;


};
UCLASS()
class TDOWN_API ATDownWeapon : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ATDownWeapon(const FObjectInitializer& ObjectInitializer);

	//					Fire
	UFUNCTION()
		virtual void Fire();
	UFUNCTION()
		virtual void ProjectileFire();
	UFUNCTION()
		virtual void InstantFire();

	void SetTarget(FVector Point) { Taraget = Point; };
	FVector GetTarget() { return Taraget; };
	FTimerHandle FireTimeHandle;
	FVector Taraget;
	//					endFire

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-WeaponConfig")
		EWeaponProjectile ProjectileType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-WeaponConfig")
		FWeaponData WeaponConfig;
		

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-WeaponConfig")
		TSubclassOf<class ATDownBullet> ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-WeaponConfig")
		UBoxComponent* CollisionComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-WeaponConfig")
		USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-WeaponConfig")
		UParticleSystemComponent* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-WeaponConfig")
		int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-WeaponConfig")
		int32 CurrentClip;

																/** MECHANICS **/
	void SetOwningPawn(ATDownCharacter*);

	void OnEqup();
	void OnUnEquip();

	void AttachToPlayer();
	void DetachFromPlayer();

	void ReloadAmmo();
																		/*_____*/
protected:
	ATDownCharacter* OwnerPawn;	
	void PostInitializeComponents() override;
	FName WeaponSocket = FName(TEXT("Weapon"));
	int32 ShotNumber=0;

	FHitResult WeaponTrace(UWorld* World, ECollisionChannel CollChannel, const FVector& StartTrace, const FVector& EndTrace, bool DrawTestLine, FColor TestColorLine, TArray<AActor*> IgnorActors); //(const FVector&TraceFrom, const FVector& TraceTo) const
	void ProcessInstantHit(const FHitResult &Impact, const FVector &StartTrace, const FVector &ShootDir, int32 RandomSeed, float ReticleSpread);
};
