// Fill out your copyright notice in the Description page of Project Settings.

#include "TDown.h"
#include "TDownProjectileMovementComponent.h"
#include "TDownCharacter.h"

void UTDownProjectileMovementComponent::SetTraectoryCurve(UCurveVector * VectorCurve)
{
	cTraectory = VectorCurve;
}

void UTDownProjectileMovementComponent::SetTarget(ATDownCharacter * NewTarget)
{
	pTarget = NewTarget;
}

void UTDownProjectileMovementComponent::SetTargetDirection(FVector InTargetPoint, FVector InTargetDirection)
{
	TargetPoint = InTargetPoint;
	TargetDirection = InTargetDirection;
}

FVector UTDownProjectileMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	FVector NewVelocity = Super::ComputeVelocity(InitialVelocity,DeltaTime);

	NewVelocity += NewVelocity.GetSafeNormal()*RocketAcceleration*DeltaTime;
	if (pTarget && pTarget->bIsAlive)
		FollowTarget(NewVelocity, DeltaTime);
	else
		FollowDirection(NewVelocity, DeltaTime);

	return NewVelocity;
}

void UTDownProjectileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTDownProjectileMovementComponent::FollowTarget(FVector & NewVelocity, float DeltaTime) const
{
	FVector TargetPosition = pTarget->GetActorLocation();

	if (bEnableJavelinMode)
	{
		FVector RocketLocation = GetOwner()->GetActorLocation();
		auto len = (TargetPosition - RocketLocation).Size2D();
		if (len > JavelinAttackRadius)
		{
			TargetPosition.Z += JavelinAttackHeight;
		}
	}
	FollowPoint(NewVelocity, DeltaTime, TargetPosition);
}

void UTDownProjectileMovementComponent::FollowDirection(FVector & NewVelocity, float DeltaTime) const
{
	FVector TargetPosition=TargetPoint;
	//const FVector StartPoint = GetOwner()->GetActorLocation();
	FVector RocketLocation = GetOwner()->GetActorLocation();
	if (bEnableJavelinMode)
	{
		
		auto len = (TargetPosition - RocketLocation).Size2D();
		if (len > JavelinAttackRadius)
		{
			TargetPosition.Z += JavelinAttackHeight;
		}

		/*if (cTraectory!=NULL)
		{
			float currentTime;
			currentTime =(TargetPosition - RocketLocation).Size2D();
			float l = FMath::Clamp(currentTime, 0.0f, 1.0f);

			auto zCurvature = (cTraectory->GetVectorValue(currentTime)).Z;

			TargetPosition.Z = (TargetPosition.Z)*zCurvature;
		}*/
	}

	FollowPoint(NewVelocity, DeltaTime, TargetPosition);
}

void UTDownProjectileMovementComponent::FollowPoint(FVector & NewVelocity, float DeltaTime, const FVector & FolowPoint) const
{
	FVector CL = GetOwner()->GetActorLocation();
	auto TargetDir = (FolowPoint - CL).SafeNormal();
	auto CurrentDir = NewVelocity.GetSafeNormal();

	float l = FMath::Clamp(RocketMobility * DeltaTime, 0.0f, 1.0f);

	CurrentDir = CurrentDir*(1 - l) + TargetDir*l;
	NewVelocity = CurrentDir * NewVelocity.Size();
}
