// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAnimInstance.h"
#include "EnemyProjectile.h"
#include "Net/UnrealNetwork.h"
#include "components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PokeHunter/Item/Item.h"
#include "PokeHunter/Base/ItemDropActor.h"
#include "PokeHunter/Hunter/Hunter.h"
#include "PokeHunter/Partner/Partner.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;



	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCapsuleComponent()->SetCapsuleRadius(90.f);
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);


	TeamID = FGenericTeamId(1);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	if (EnemyAnim) EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemy::OnMontageEnded);

	BaseLocation = GetActorLocation();
	ComeBackHome();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurTime = GetWorld()->TimeSeconds;
	if (bBinding)
	{
		float ElapsedTime = CurTime - StartBindingTime;
		float TimeLeft = BindingTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bBinding = false;
			CurState = EEnemyState::Chase;
		}

	}

	//독 데미지 
	if (bPoisoned)
	{
		float ElapsedTime = CurTime - StartPoisonedTime;
		int CurSecond = FMath::FloorToInt(ElapsedTime);
		float TimeLeft = PoisonedTime - ElapsedTime;
		if (TimeLeft <= 0.0f)
		{
			bPoisoned = false;
			CurSecond = 0;
		}
		else
		{
			if (CurSecond == PoisonSaveTime)
			{
				//
			}
			else
			{
				PoisonSaveTime = CurSecond;
				FDamageEvent PoisonDamage;
				float DamageAmount = 1.f;
				TakeDamage(DamageAmount, PoisonDamage, NULL, NULL);

				//Damage UI Print Event
				OnDamage.Broadcast(DamageAmount, GetActorLocation());
			}
		}
	}


	//화상 데미지
	if (bBurning)
	{
		int CurSecond = FMath::FloorToInt(BurningLimitTime);
		BurningLimitTime -= DeltaTime;
		if (CurSecond != FMath::FloorToInt(BurningLimitTime))
		{
			FDamageEvent BurningDamage;
			float DamageAmount = 1.f;
			UE_LOG(LogTemp, Warning, TEXT("BurningDamage"));
			if (HasAuthority())
			{
				UGameplayStatics::ApplyDamage(this, DamageAmount, nullptr, nullptr, UDamageType::StaticClass());
			}

			else
			{
				ServerApplyDamage(this, DamageAmount, nullptr, nullptr, UDamageType::StaticClass());
			}
		}
		if (BurningLimitTime <= 0.0f)
		{
			bBurning = false;
		}
	}

	if (bDied)
	{
		FVector CurScale = GetActorScale();
		SetActorScale3D(CurScale - DeltaTime * FVector(1.f, 1.f, 1.f));
		if (GetActorScale().Length() < 0.2f)
		{
			Die();
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetMesh()->GetSocketLocation("HeadSocket");
	OutRotation = GetMesh()->GetSocketRotation("HeadSocket");
}

FGenericTeamId AEnemy::GetGenericTeamId() const
{
	return TeamID;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HP <= 0) return 0;

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	HP -= DamageAmount;
	SavedDamage += DamageAmount;
	if (GrogyDamage <= SavedDamage)
	{
		bGrogy = true;
		CurState = EEnemyState::Grogy;
		ServerPlayMontage(this, FName("Grogy"));
	}
	//Item Hit
	AItem* HitItem = Cast<AItem>(DamageCauser);
	FVector HitLoc;


	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitLoc = PointDamageEvent.HitInfo.Location;
		FVector DirectionVec = -1.f * PointDamageEvent.HitInfo.ImpactNormal;
		//DirectionVec.Z = 0.f;
		//DirectionVec.Normalize();
		//GetCharacterMovement()->Launch(DirectionVec * 1000.f);
		//LaunchCharacter(DirectionVec * 1000000.f, true,true);
	}
	else
	{
		HitLoc = GetActorLocation();
	}

	//피격 애니메이션 처리
	if (HitItem)
	{
		if (AHunter* Hunter = Cast<AHunter>(HitItem->ThisOwner))
		{
			Hunter->SetPartnerTarget(this);
		}
	}

	if (HP <= 0)
	{
		//사망 애니메이션
		CurState = EEnemyState::Die;
		ServerPlayMontage(this, FName("Die"));
	}
	else
	{
		if (bGrogy)
		{
			//그로기 시 애니메이션 생략
		}
		else
		{
			ServerPlayMontage(this, FName("Hit"));
			if (Target == NULL)
			{
				Target = HitItem->ThisOwner;
				CurState = EEnemyState::Hit;
			}
		}
	}

	//Damage and UI
	ServerShowDamage(DamageAmount, HitLoc);

	return DamageAmount;
}

bool AEnemy::IsJumping()
{
	return GetCharacterMovement()->IsFalling();
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemy, HP);
	DOREPLIFETIME(AEnemy, CurState);
	DOREPLIFETIME(AEnemy, Target);
	DOREPLIFETIME(AEnemy, bDied);
	DOREPLIFETIME(AEnemy, bBurning);
	DOREPLIFETIME(AEnemy, BurningLimitTime);

}

void AEnemy::ServerPlayMontage_Implementation(AEnemy* Enemy, FName Section)
{
	MultiPlayMontage(this, Section);
}


void AEnemy::MultiPlayMontage_Implementation(AEnemy* Enemy, FName Section)
{
	EnemyAnim->PlayCombatMontage(Section, true);
}

void AEnemy::ServerStartBinding_Implementation()
{
	MultiStartBinding_Implementation();
}

void AEnemy::MultiStartBinding_Implementation()
{
	bBinding = true;
	StartBindingTime = GetWorld()->TimeSeconds;
}

void AEnemy::ServerApplyPointDamage_Implementation(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult)
{
	MultiApplyPointDamage(OtherActor, DamageAmount, HitDirection, DamageCauser, SweepResult);
}

void AEnemy::MultiApplyPointDamage_Implementation(AActor* OtherActor, float DamageAmount, FVector HitDirection, AActor* DamageCauser, const FHitResult& SweepResult)
{
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, SweepResult.Normal, SweepResult, NULL, DamageCauser, UDamageType::StaticClass());

}

void AEnemy::StartBinding()
{
	ServerStartBinding();
}

void AEnemy::StartPoison()
{
	bPoisoned = true;
	StartPoisonedTime = GetWorld()->TimeSeconds;
}

void AEnemy::OnRep_Burning_Implementation()
{
	if (bBurning) BurningLimitTime = BurningTime;
}

void AEnemy::ServerApplyDamage_Implementation(AActor* OtherActor, float DamageAmount, AController* DamageInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
	MultiApplyDamage(OtherActor, DamageAmount, DamageInstigator, DamageCauser, DamageType);
}

void AEnemy::MultiApplyDamage_Implementation(AActor* OtherActor, float DamageAmount, AController* DamageInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
	UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, DamageInstigator, DamageCauser, DamageType);
}

void AEnemy::ServerShowDamage_Implementation(float DamageAmount, const FVector& ShowLoc)
{
	MultiShowDamage(DamageAmount, ShowLoc);
}

void AEnemy::MultiShowDamage_Implementation(float DamageAmount, const FVector& ShowLoc)
{
	OnDamage.Broadcast(DamageAmount, ShowLoc);
}

void AEnemy::SeeNewTarget(AActor* Actor)
{
	TargetArray.AddUnique(Actor);


	if (Target == NULL)
	{
		SetTarget(Actor);
		CurState = EEnemyState::Chase;
		EnemyAnim->StopCombatMontage(0.2f);
	}

}

void AEnemy::HearSound(FVector SoundLoc, AActor* AgroActor)
{
	TargetPos = SoundLoc;
	AgroTarget = AgroActor;
	bWaitingAgro = true;
}

void AEnemy::ComeBackHome()
{
	Target = NULL;
	TargetPos = BaseLocation;
	CurState = EEnemyState::Patrol;
}

bool AEnemy::CheckInMoveRange()
{
	float Distance = FVector::Dist2D(BaseLocation, GetActorLocation());
	bool bResult = Distance <= MoveRange;
	if (bResult) return true;
	else
	{
		ComeBackHome();
		return false;
	}
}

void AEnemy::LeaveTarget(AActor* KilledTarget)
{
	for (int i = 0; i < TargetArray.Num(); ++i)
	{
		if (TargetArray[i] == KilledTarget)
		{
			TargetArray.RemoveAt(i);
		}
	}

	if (KilledTarget == Target)
	{
		Target = NULL;
		ChangeTarget();
	}

}

void AEnemy::ChangeTarget()
{
	int NearestTargetNum = 0;
	int ShortestDistance = 100000000;
	for (int i = 0; i < TargetArray.Num(); ++i)
	{
		FVector DirectionVec = TargetArray[i]->GetActorLocation() - GetActorLocation();
		DirectionVec.Z = 0;
		float Distance = DirectionVec.Size();

		if (Distance < ShortestDistance)
		{
			ShortestDistance = Distance;
			NearestTargetNum = i;
		}
	}
	if (!TargetArray.IsEmpty()) Target = TargetArray[NearestTargetNum];
}

int AEnemy::CheckInRange()
{
	return 0;
}

int AEnemy::CheckPattern()
{
	return 0;
}

void AEnemy::Attack(int AttackPattern)
{
	if (Target != NULL)
	{
		CurState = EEnemyState::NormalAttack;
		ServerPlayMontage(this, FName("Attack"));
		//EnemyAnim->PlayCombatMontage(TEXT("Attack"));
	}
}

void AEnemy::PatternAttack(int AttackPattern)
{
}

void AEnemy::LongAttack()
{
	if (EnemyAnim == NULL || Target == NULL) return;
	CurState = EEnemyState::LongAttack;
	FVector InitialPos = GetMesh()->GetSocketLocation(FName("LeftHand")) + GetActorForwardVector() * 300.f;
	FVector EndPos = Target->GetActorLocation();
	FVector DirectionVec = EndPos - GetActorLocation();
}

void AEnemy::Roar()
{
	if (EnemyAnim == NULL) return;
	ServerPlayMontage(this, FName("Roar"));
	//EnemyAnim->PlayCombatMontage(TEXT("Roar"));
}

void AEnemy::Patrol()
{
	if (EnemyAnim == NULL) return;
	ServerPlayMontage(this, FName("Patrol"));
	//EnemyAnim->PlayCombatMontage(TEXT("Patrol"));
}

void AEnemy::JumpAttack()
{
	CurState = EEnemyState::JumpAttack;
	ServerPlayMontage(this, FName("Attack_Jump"));

}

void AEnemy::ChargeAttack()
{
	CurState = EEnemyState::ChargeAttack;
	ServerPlayMontage(this, FName("ChargeAttack"));
}


void AEnemy::RushAttack()
{
	ServerPlayMontage(this, FName("Slide_Start"));
	CurState = EEnemyState::RushAttack;
}

void AEnemy::WideAttack()
{
	ServerPlayMontage(this, FName("WideAttack"));
	CurState = EEnemyState::WideAttack;

}

void AEnemy::LaunchToTarget()
{
	if (Target)
	{
		FVector Velocity = FVector::ZeroVector;
		FVector EndPos = Target->GetActorLocation();
		FVector StartPos = GetActorLocation();
		FVector LookVec = GetActorForwardVector();

		//개선 필요
		bool bCantJump = UGameplayStatics::SuggestProjectileVelocity(this, Velocity, StartPos, EndPos,
			3000.f, false, 0.f, GetWorld()->GetGravityZ(), ESuggestProjVelocityTraceOption::OnlyTraceWhileAscending);


		LaunchCharacter(Velocity, true, true);

	}
}
void AEnemy::Block()
{
	bReflecting = true;
}

void AEnemy::Reflect()
{
	//반격 시 변수 초기화
	FVector ExplosionLocation = GetActorLocation();
	float DamageRadius = 500.f;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ReflectDamgeAmount, ExplosionLocation, DamageRadius, UDamageType::StaticClass(), IgnoreActors, GetController(), false);

	UE_LOG(LogTemp, Warning, TEXT("%f"), ReflectDamgeAmount);

	bReflecting = false;
	ReflectDamgeAmount = 0.f;
}

void AEnemy::Die()
{
	SpawnItem();
	Destroy();
}

void AEnemy::SpawnItem()
{
	FVector SpawnLoc = GetActorLocation();
	SpawnLoc.Z += 50.f;
	if (HasAuthority())
	{
		ServerSpawnItemBox(SpawnLoc, DropItemBoxClass, DropItemID_Array);
	}
	/*AItemDropActor* ItemBox = GetWorld()->SpawnActor<AItemDropActor>(DropItemBoxClass, SpawnLoc, GetActorRotation());
	ItemBox->CreateItemArray(DropItemID_Array);*/
}

void AEnemy::ServerSpawnItemBox_Implementation(const FVector& SpawnLoc, TSubclassOf<AInteractActor> SpawnClass, const TArray<FName>& ItemID_Array)
{
	AItemDropActor* ItemBox = GetWorld()->SpawnActor<AItemDropActor>(DropItemBoxClass, SpawnLoc, GetActorRotation());
	ItemBox->CreateItemArray(ItemID_Array);
}

void AEnemy::ServerSpawnProjectile_Implementation(TSubclassOf<class AEnemyProjectile>SpawnClass, const FVector& SpawnLoc, const FVector& EndLoc, const FVector& DirectionVec)
{
	AEnemyProjectile* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(SpawnClass, SpawnLoc, DirectionVec.Rotation());
	Projectile->FirstUse(DirectionVec, SpawnLoc, EndLoc);
}

void AEnemy::ServerDestroy_Implementation()
{
	MultiDestroy();
}

void AEnemy::MultiDestroy_Implementation()
{
	Destroy();
}

void AEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		EnemyAnim->bPlaying = false;
		if (CurState == EEnemyState::Hit) CurState = EEnemyState::Roar;
		OnMontageEnd.Broadcast();
		CurState = EEnemyState::Chase;
	}


}

void AEnemy::InteractIce_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed /= 2;
}

void AEnemy::InteractFire_Implementation(UPrimitiveComponent* HitComponent)
{
	bBurning = true;
	BurningLimitTime = BurningTime;
}

void AEnemy::InteractBindTrap_Implementation()
{
	bBinding = true;
	StartBindingTime = GetWorld()->TimeSeconds;
}

void AEnemy::InteractEarthquake_Implementation()
{
}

void AEnemy::InteractAttack_Implementation(FVector HitDirection, float Damage)
{
	if (Damage <= 0.f)
	{
		return;
	}
	if (HitDirection.Z < 0.f) HitDirection.Z *= -1;

	LaunchCharacter(HitDirection * 1000.f, false, false);
}

void AEnemy::InteractGrabAttack_Implementation()
{
}
