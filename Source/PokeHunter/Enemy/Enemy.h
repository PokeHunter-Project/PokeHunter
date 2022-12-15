// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMontageEndDelegate);

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Chase UMETA(DisplayName = "Chase"),
	Hit UMETA(DisplayName = "Hit"),
	Die UMETA(DisplayName = "Die"),
	Roar UMETA(DisplayName = "Roar")
	
};


UCLASS()
class POKEHUNTER_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float HP{30};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APawn* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEnemyAnimInstance* EnemyAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EEnemyState CurState{EEnemyState::Patrol};

	FOnMontageEndDelegate OnMontageEnd;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//CollisionFunction
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void Attack();
	virtual void Roar();

	//Animation Function
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	bool bFirstHit{ true};
};
