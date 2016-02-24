// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/Character.h"
#include "TDownArmor.h"
#include "TDownCharacter.generated.h"



UCLASS(Blueprintable)
class ATDownCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATDownCharacter();
	
											/*Weapon*/
	UPROPERTY(EditDefaultsOnly, Category = "-DefaultInventory")
		TSubclassOf<class ATDownWeapon> WeaponSpawn;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "-Weapon")
		ATDownWeapon* CurrentWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "-Weapon")
	 bool bFire;

											/*Armor*/
	UPROPERTY(EditDefaultsOnly, Category = "-DefaultInventory")
		TSubclassOf<class ATDownArmor> ArmorSpawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "-Armor")
		ATDownArmor* CurrentArmor;


	UPROPERTY(EditDefaultsOnly, Category = "-DefaultInventory")
		TArray<class ATDownWeapon*> Inventory;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "-DefaultInventory")
	int32 InventoryNum = 3;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "-CharacterSettings")
	int32 HP;
	UFUNCTION(BlueprintNativeEvent, Category ="-TDCharacter")
		void RefreshHP();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "-CharacterSettings")
	bool bIsAlive;



	UFUNCTION()
	void FireWeapon(FVector Target);
	void SetDamage(int32);
	void FireState(bool);

	int CalcReduceDamageFromArmor(int32 InDamage);// reduce damage from armor
	void EquipArmor(ATDownArmor* Armor);

	void SetAliveState(bool);


	void ProcessWeaponPickUp(AActor*);
	void NextWeapon();
	void PrevWeapon();
	void EquipWeapon(ATDownWeapon*);

	void GiveDefaultAmmunition();

	void ProcessActorPickUp(AActor* ActorToPickUp);

	void PostInitializeComponents() override;
	void BeginPlay() override;
	
	UFUNCTION()
		void OnCollision(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	 
	
private:




	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
};

