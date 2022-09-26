// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Hunter.generated.h"

UCLASS()
class POKEHUNTER_API AHunter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHunter();

	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* FollowCamera;

	//상호작용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		class ANpc* CurrentNpc;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	//인벤토리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		class UInventoryComponent* Inventory;

	//UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		TSubclassOf <UUserWidget> InventoryUIClass;
	class UUserWidget* InventoryUI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		TSubclassOf <UUserWidget> StorageUIClass;
	class UUserWidget* StorageUI;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void AddControllerPitchInput(float Val);
	virtual void AddControllerYawInput(float Val);
	void MoveRight(float Val);
	void MoveForward(float Val);
	void RMBDown();
	void OpenInventory();


	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
