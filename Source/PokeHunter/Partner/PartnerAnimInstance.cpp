// Fill out your copyright notice in the Description page of Project Settings.


#include "PartnerAnimInstance.h"
#include "Partner.h"

UPartnerAnimInstance::UPartnerAnimInstance()
{
	MovementSpeed = 0.0f;
}

void UPartnerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	::UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		// Partner = Cast<APartner>(TryGetPawnOwner());
		MovementSpeed = Pawn->GetVelocity().Size();
	}
}

void UPartnerAnimInstance::NativeInitializeAnimation()
{
	::UAnimInstance::NativeInitializeAnimation();

	if (Partner == nullptr)
	{
		Partner = Cast<APartner>(TryGetPawnOwner());
	}
}

void UPartnerAnimInstance::UpdateAnimationProperties()
{
	if (Partner != nullptr)
	{
		FVector Speed = Partner->GetVelocity();
		FVector XYspeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = XYspeed.Size();

		UInputComponent* Input = Partner->InputComponent;
		Direction = CalculateDirection(Speed, Partner->GetActorRotation());
	}
}
