// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "TDownWeapon.h"
#include "Engine.h"
#include "TDownBullet.h"


// Sets default values
ATDownWeapon::ATDownWeapon(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionBox"));
	RootComponent = CollisionComp;

	WeaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeeaponMesh"));
	WeaponMesh->AttachTo(CollisionComp);

	MuzzleFlash = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MuzzleFlash"));
	MuzzleFlash->bAutoActivate = false;
	MuzzleFlash->AttachTo(WeaponMesh, "MuzzleFlash");

}

void ATDownWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (WeaponMesh->SkeletalMesh == NULL)
	{
		if (WeaponConfig.BulletMesh) WeaponMesh->SetSkeletalMesh(WeaponConfig.WeaponMesh);
	}
	if (WeaponConfig.BulletMesh == NULL)
	{
		ATDownBullet* Bullet = Cast<ATDownBullet>(ProjectileClass);
		if (Bullet)
		{
			Bullet->BulletMesh->SetStaticMesh(WeaponConfig.BulletMesh);
		}
	}
	if (MuzzleFlash->Template ==NULL)
	{
		if (WeaponConfig.MuzzleParticle)
		{
			MuzzleFlash->SetTemplate(WeaponConfig.MuzzleParticle);
		}
		MuzzleFlash->DeactivateSystem();
	}
	if (WeaponConfig.WeaponDamage == 0)
	{
		WeaponConfig.WeaponDamage = 10;
	}

}


void ATDownWeapon::Fire()
{
	

	if (CurrentClip > 0)
	{
		if (ProjectileType == EWeaponProjectile::EP_Bullet)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, TEXT("Bullet"));
			InstantFire();
		}
		if (ProjectileType==EWeaponProjectile::EP_Spread)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, TEXT("Spread"));
			InstantFire();

			if (ShotNumber < WeaponConfig.ShotCost)
			{
				OwnerPawn->FireState(true);
				ShotNumber++;
				GetWorldTimerManager().SetTimer(FireTimeHandle, this, &ATDownWeapon::Fire, WeaponConfig.TimeBetweenShots, false);
			}
			else
			{
				ShotNumber = 0;
				OwnerPawn->FireState(false);
			}			
		}
		if (ProjectileType == EWeaponProjectile::EP_Projectile)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, TEXT("Projectile"));
			ProjectileFire();
		}
		MuzzleFlash->ActivateSystem();
	}
	else
	{
		ReloadAmmo();
		OwnerPawn->FireState(false);
	}

}

void ATDownWeapon::SetOwningPawn(ATDownCharacter * NewOwner)
{
	if (OwnerPawn != NewOwner)
	{
		Instigator = NewOwner;
		OwnerPawn = NewOwner;
	}
}

void ATDownWeapon::OnEqup()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToPlayer();
}

void ATDownWeapon::OnUnEquip()
{
	DetachFromPlayer();
}

void ATDownWeapon::AttachToPlayer()
{
	if (OwnerPawn)
	{
		DetachFromPlayer();

		USkeletalMeshComponent* Char = OwnerPawn->GetMesh();
		WeaponMesh->AttachTo(Char, WeaponSocket);
		WeaponMesh->SetHiddenInGame(false);
	}
}

void ATDownWeapon::DetachFromPlayer()
{
	WeaponMesh->DetachFromParent();
	WeaponMesh->SetHiddenInGame(true);
}

void ATDownWeapon::ReloadAmmo()
{
	if (CurrentAmmo > 0)
	{
		CurrentClip = WeaponConfig.MaxClip;
		CurrentAmmo -= WeaponConfig.MaxClip;
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("RELOAD"));
	}
	else GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("NO AMMO!!"));
}

void ATDownWeapon::ProjectileFire()
{

	const FVector StartPoint = WeaponMesh->GetSocketLocation("MuzzleFlash");
	const FVector Direction = GetTarget() - StartPoint;
	const FRotator RotProj = Direction.Rotation();

	UWorld* World = GetWorld();
	if (World)
	{
		FTransform SpawnTM;
		SpawnTM.SetRotation(RotProj.Quaternion());
		SpawnTM.SetLocation(StartPoint);

		ATDownBullet* SpawnProj = Cast<ATDownBullet>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileClass, SpawnTM));

		if (SpawnProj)
		{
			SpawnProj->SetOwner(GetOwner());
			SpawnProj->SetHitParticle(WeaponConfig.HitPatricle);
			SpawnProj->CollisionComp->IgnoreActorWhenMoving(OwnerPawn, true);
			SpawnProj->CurrentDamage = WeaponConfig.WeaponDamage;
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("ProjectileFire"));
		}
		UGameplayStatics::FinishSpawningActor(SpawnProj, SpawnTM);
			
	}

}
void ATDownWeapon::InstantFire()
{
	CurrentClip -= 1;// WeaponConfig.ShotCost;

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("InstantFire"));

	const auto RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);

	const float CurrentSpread = WeaponConfig.WeaponSpread;
	const float SpreadCone = FMath::DegreesToRadians(WeaponConfig.WeaponSpread * 0.5);

	const FVector StartTrace = WeaponMesh->GetSocketLocation("MuzzleFlash");
	const FVector AimDir = GetTarget() - StartTrace;//WeaponMesh->GetSocketRotation("MuzzleFlash").Vector();

	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir,SpreadCone,SpreadCone);

	const FVector EndTrace = StartTrace + ShootDir* WeaponConfig.WeaponRange;

	TArray<AActor*>IgnorActors;
	IgnorActors.Add(OwnerPawn);
	IgnorActors.Add(this);

	FHitResult Impact = WeaponTrace(GetWorld(),TRACE_WEAPON,StartTrace, EndTrace,false,FColor::Blue,IgnorActors);

	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
}


FHitResult ATDownWeapon::WeaponTrace(UWorld* World, ECollisionChannel CollChannel, const FVector & TraceFrom, const FVector & TraceTo, bool DrawTestLine, FColor TestColorLine, TArray<AActor*> IgnorActors)
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult Hit(ForceInit);
	//bool LineTraceSingle(struct FHitResult& OutHit,const FVector& Start,const FVector& End,ECollisionChannel TraceChannel,const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam) const
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, TRACE_WEAPON, TraceParams);

	return Hit;
}

void ATDownWeapon::ProcessInstantHit(const FHitResult & Impact, const FVector & StartTrace, const FVector & ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const FVector EndTrace = StartTrace + ShootDir * WeaponConfig.WeaponRange;

	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

	//DrawDebugLine(this->GetWorld(), StartTrace, EndPoint, FColor::Red, true, 10000, 10);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponConfig.HitPatricle, Impact.ImpactPoint);

	ATDownCharacter* Enemy = Cast<ATDownCharacter>(Impact.GetActor());
	if (Enemy)
	{
		//Enemy->Destroy();
		Enemy->SetDamage(WeaponConfig.WeaponDamage);
	}
	else
	{
	}

}






