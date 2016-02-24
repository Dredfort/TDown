// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TDownPickUpObject.generated.h"

class ATDownCharacter;
class ATDownWeapon;

USTRUCT(BlueprintType)
struct FPickUpConfig
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-PickUp")
		int32 AmmoQuality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-PickUp")
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "-PickUp")
		int32 Priority;
};

UCLASS()
class TDOWN_API ATDownPickUpObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDownPickUpObject(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleDefaultsOnly, Category = "-pickUper")
		UStaticMeshComponent* PickUpMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "-pickUper")
		USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, Category = "-pickUper")
		FPickUpConfig PickupConfig;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void OnOverlap(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	
	ATDownCharacter* OwnerChar;
	ATDownWeapon* OwnerWeapon;
};
