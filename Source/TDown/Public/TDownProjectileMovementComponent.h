// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/ProjectileMovementComponent.h"
#include "TDownProjectileMovementComponent.generated.h"

class ATDownCharacter;
/**
 * 
 */
UCLASS()
class TDOWN_API UTDownProjectileMovementComponent : public UProjectileMovementComponent
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JavelinMode")
		bool bEnableJavelinMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JavelinMode")
		float JavelinAttackHeight = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JavelinMode")
		float JavelinAttackRadius = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket")
		float RocketMobility = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket")
		float RocketAcceleration = 10000;

	UCurveVector* cTraectory;
	void SetTraectoryCurve(UCurveVector* VectorCurve);

	
	void SetTarget(ATDownCharacter*);	
	void SetTargetDirection(FVector InTargetPoint, FVector inTargetDirection);

	FVector ComputeVelocity(FVector InitialVelocity, float DeltaTime) const override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override; 

private:
	UPROPERTY()
		ATDownCharacter* pTarget;

	FVector TargetPoint;
	FVector TargetDirection;

	void FollowTarget(FVector& NewVelocity, float DeltaTime) const;
	void FollowDirection(FVector& NewVelocity, float DeltaTime) const;
	void FollowPoint(FVector& NewVelocity, float DeltaTime, const FVector& FolowPoint) const;
};