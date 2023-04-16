// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class POKEHUNTER_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UEnemyAnimInstance();

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	float MovementSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
	bool bPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
	bool bJumping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = Movement)
	FVector TargetLoc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Movement")
	class AEnemy* Enemy;

	//Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Combat")
	UAnimMontage* CombatMontage;

public:
	void StopCombatMontage(float BlendTime);
	bool PlayCombatMontage(FName Section, bool interuupt = false);
};
