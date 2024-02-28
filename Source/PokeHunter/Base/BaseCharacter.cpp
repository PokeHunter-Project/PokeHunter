// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Invincible
	if (bInvincible)
	{
		CurInvincibleTime += DeltaTime;
		if(InvincibleTime <= CurInvincibleTime)
		{
			CurInvincibleTime = 0.f;
			bInvincible = false;
		}
	}

	if (bNoCollision)
	{
		CurNoCollisionTime += DeltaTime;
		if (NoCollisionTime <=CurNoCollisionTime)
		{
			CurNoCollisionTime = 0.f;
			bNoCollision = false;
			SetActorEnableCollision(true);
		}
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, HP);
}

