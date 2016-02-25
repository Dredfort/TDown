// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TDown.h"
#include "TDownCharacter.h"
#include "Engine.h"
#include "TDownWeapon.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "TDownPickUpObject.h"


ATDownCharacter::ATDownCharacter()
{
	bFire = false;
	bIsAlive = true;
	Inventory.SetNum(InventoryNum, false);
	CurrentWeapon = NULL;

	HP = 100;

	PrimaryActorTick.bCanEverTick = true;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this,&ATDownCharacter::OnCollision);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDownCharacter::FireWeapon(FVector Taraget)
{
	if (CurrentWeapon != NULL)
	{
		CurrentWeapon->SetTarget(Taraget);
		//FireState(true);
		CurrentWeapon->Fire();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("No WEPON In Hands!"));
	}
}

void ATDownCharacter::NextWeapon()
{
	if (Inventory[CurrentWeapon->WeaponConfig.Priority]->WeaponConfig.Priority != 2)
	{
		if (Inventory[CurrentWeapon->WeaponConfig.Priority + 1] == NULL)
		{
			for (int32 i = CurrentWeapon->WeaponConfig.Priority + 1; i < Inventory.Num();i++)
			{
				if (Inventory[i] && Inventory[i]->IsA(ATDownWeapon::StaticClass()))
				{
					EquipWeapon(Inventory[i]);
				}
			}
		} 
		else
		{
			EquipWeapon(Inventory[CurrentWeapon->WeaponConfig.Priority + 1]);
		}
	}
	else
	{
		EquipWeapon(Inventory[CurrentWeapon->WeaponConfig.Priority]);
	}
}

void ATDownCharacter::PrevWeapon()
{
		if (Inventory[CurrentWeapon->WeaponConfig.Priority]->WeaponConfig.Priority != 0)
		{
			if (Inventory[CurrentWeapon->WeaponConfig.Priority - 1] == NULL)
			{
				for (int32 i = CurrentWeapon->WeaponConfig.Priority - 1; i >= 0; i--)
				{
					if (Inventory[i] && Inventory[i]->IsA(ATDownWeapon::StaticClass()))
					{
						EquipWeapon(Inventory[i]);
					}
				}
			}
			else
			{
				EquipWeapon(Inventory[CurrentWeapon->WeaponConfig.Priority - 1]);
			}
		}
		else
		{
			EquipWeapon(Inventory[CurrentWeapon->WeaponConfig.Priority]);
		}

}

void ATDownCharacter::OnCollision(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	ATDownWeapon* Weapon = Cast<ATDownWeapon>(OtherActor);
	if (Weapon)
	{
		ProcessWeaponPickUp(Weapon);
	}

	/*ATDownPickUpObject* PickUper = Cast<ATDownPickUpObject>(OtherActor);
	if (PickUper)
	{
		ProcessActorPickUp(PickUper);
	}*/
	ProcessActorPickUp(OtherActor);
}

void ATDownCharacter::RefreshHP_Implementation()
{
}



void ATDownCharacter::ProcessWeaponPickUp(AActor* Weapon)
{
	if (Weapon != NULL)
	{
		ATDownWeapon* Spawner = GetWorld()->SpawnActor<ATDownWeapon>(Weapon->GetClass());
			if (Spawner)
			{
				Inventory[Spawner->WeaponConfig.Priority] = Spawner;
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "You Just picked up a " + Inventory[Spawner->WeaponConfig.Priority]->WeaponConfig.Name);

			}
			Weapon->Destroy();
			
	}

	

	/*	if (Inventory[Weapon->WeaponConfig.Priority]==NULL)
		{
			ATDownWeapon *Spawner = GetWorld()->SpawnActor<ATDownWeapon>(Weapon->GetClass());
			if (Spawner)
			{
				Inventory[Spawner->WeaponConfig.Priority] = Spawner;
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "You Just picked up a " + Inventory[Spawner->WeaponConfig.Priority]->WeaponConfig.Name);
			}
			Weapon->Destroy();
		}
	}
	else
	{
		if (Inventory[Weapon->WeaponConfig.Priority]->CurrentAmmo >= 0 && Weapon->CurrentAmmo <= (Inventory[Weapon->WeaponConfig.Priority]->WeaponConfig.MaxAmmo 
			- Inventory[Weapon->WeaponConfig.Priority]->CurrentAmmo ))
		{
			Inventory[Weapon->WeaponConfig.Priority]->CurrentAmmo += Weapon->CurrentAmmo;
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Added " + Weapon->CurrentAmmo);
			Weapon->Destroy();
		}
		else
		{
			if (Inventory[Weapon->WeaponConfig.Priority]->CurrentAmmo > Inventory[Weapon->WeaponConfig.Priority]->WeaponConfig.MaxAmmo)
			{
				Inventory[Weapon->WeaponConfig.Priority]->CurrentAmmo = Inventory[Weapon->WeaponConfig.Priority]->WeaponConfig.MaxAmmo;
			}
		}
	}*/
}

void ATDownCharacter::ProcessActorPickUp(AActor* ActorToPickUp)
{
	if (ActorToPickUp !=NULL)
	{
		ATDownPickUpObject* PickUpAmmo = Cast<ATDownPickUpObject>(ActorToPickUp);
		if (PickUpAmmo)
		{
			auto InvenroryWeapon = Cast<ATDownWeapon>(Inventory[PickUpAmmo->PickupConfig.Priority]);
			auto AmmoToAdd = PickUpAmmo->PickupConfig.AmmoQuality;

			if (InvenroryWeapon)
			{
				if (Inventory[PickUpAmmo->PickupConfig.Priority] != NULL)
				{
					InvenroryWeapon->CurrentAmmo += AmmoToAdd;
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "ammo for " + InvenroryWeapon->WeaponConfig.Name + " added " + FString::FromInt(AmmoToAdd));
					//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, AmmoToAdd);


					PickUpAmmo->Destroy();
				}
			}
		}

		ATDownArmor* PickUpArmor = Cast<ATDownArmor>(ActorToPickUp);
		if (PickUpArmor)
		{
			CurrentArmor = PickUpArmor;
			EquipArmor(CurrentArmor);
		}
	}
}

void ATDownCharacter::EquipWeapon(ATDownWeapon *Weapon)
{
	if (CurrentWeapon != NULL)
	{
		CurrentWeapon = Inventory[CurrentWeapon->WeaponConfig.Priority];
		CurrentWeapon->OnUnEquip();
		CurrentWeapon = Weapon;
		Weapon->SetOwningPawn(this);
		Weapon->OnEqup();
	}
	else
	{
		CurrentWeapon = Weapon;
		CurrentWeapon = Inventory[CurrentWeapon->WeaponConfig.Priority];
		CurrentWeapon->SetOwningPawn(this);
		Weapon->OnEqup();
	}
}

void ATDownCharacter::EquipArmor(ATDownArmor * Armor)
{
	if (Armor !=NULL)
	{
		//CurrentArmor->Destroy();
		CurrentArmor = Armor;

		CurrentArmor->SetOwningPawn(this);
		CurrentArmor->OnEquip();

		
	}
	
}

void ATDownCharacter::GiveDefaultAmmunition()
{
	ATDownWeapon* Weapon = GetWorld()->SpawnActor<ATDownWeapon>(WeaponSpawn);
	if (Weapon)
	{
		Inventory[Weapon->WeaponConfig.Priority] = Weapon;
		CurrentWeapon = Inventory[Weapon->WeaponConfig.Priority];
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->OnEqup();
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Added " + Weapon->CurrentAmmo + Weapon->WeaponConfig.Name);
	}

	ATDownArmor* Armor = GetWorld()->SpawnActor<ATDownArmor>(ArmorSpawn);
	if (Armor)
	{
		CurrentArmor = Armor;
		CurrentArmor->SetOwningPawn(this);
		CurrentArmor->OnEquip();
	}

	
}


void ATDownCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();	
	if (WeaponSpawn ==NULL)
	{

	}
}

void ATDownCharacter::Tick(float DeltaSeconds)
{
	if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Camera, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}
}

void ATDownCharacter::BeginPlay()
{
	Super::BeginPlay();
	GiveDefaultAmmunition();
}


void ATDownCharacter::FireState(bool setState)
{
	bFire = setState;
}

void ATDownCharacter::SetAliveState(bool AliveState)
{
	bIsAlive = AliveState;
}

float ATDownCharacter::CalcReduceDamageFromArmor(float InDamage)
{
	auto ArmorDefence = (InDamage / 100)*(CurrentArmor->ArmorConfig.ArmorClass * 10);
	auto ReducedDamage = InDamage - ArmorDefence;
	return ReducedDamage;
}



void ATDownCharacter::SetDamage(float outDamage)
{
	auto Damage = 0;
	Damage = CalcReduceDamageFromArmor(static_cast <int32>(outDamage));

	if (Damage>=0)
	{
		HP -= Damage;
		RefreshHP();
		if (HP <= 0)
		{
			SetAliveState(false);
			HP = 0;
		}
	}

}
