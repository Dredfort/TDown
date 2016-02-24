// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TDownCharacter.h"
#include "GameFramework/Actor.h"
#include "TDownArmor.generated.h"

class ATDownCharacter;

USTRUCT(BlueprintType)
struct FArmorConfig
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ArmorSettings")
		int32 ArmorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ArmorSettings")
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ArmorSettings")
	FName ArmorSocket = FName(TEXT("Armor"));
};

UCLASS()
class TDOWN_API ATDownArmor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDownArmor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ArmorSettings")
		UStaticMeshComponent* ArmorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ArmorSettings")
		FArmorConfig ArmorConfig;


	void OnEquip();

	void SetOwningPawn(ATDownCharacter*);
	
	ATDownCharacter* OwnerChar;
};
