// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

#include "Enemy.h"

AEnemyController::AEnemyController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Enemy/BB_Enemy.BB_Enemy"));
	if (BBObject.Succeeded())
	{
		BBEnemy = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Enemy/BT_Enemy.BT_Enemy"));
	if (BTObject.Succeeded())
	{
		BTEnemy = BTObject.Object;
	}

	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));

	ConfigSight = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	ConfigSight->DetectionByAffiliation.bDetectEnemies = true;
	ConfigSight->DetectionByAffiliation.bDetectFriendlies = true;
	ConfigSight->DetectionByAffiliation.bDetectNeutrals = true;

	ConfigHearing = CreateOptionalDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	ConfigHearing->DetectionByAffiliation.bDetectEnemies = true;
	ConfigHearing->DetectionByAffiliation.bDetectFriendlies = true;
	ConfigHearing->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*ConfigSight->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*ConfigSight);
	GetPerceptionComponent()->ConfigureSense(*ConfigHearing);

}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();


}

void AEnemyController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);
	Enemy = Cast<AEnemy>(pawn);
	GetAIPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnPerception);
	RunAI();
	
}

void AEnemyController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{

	switch (Stimulus.Type)
	{
	case 0:
		Enemy->SeeNewTarget(Actor);
		break;
	case 1:
		FVector SoundLoc = Stimulus.StimulusLocation;
		Enemy->HearSound(SoundLoc);
		break;

	}
}


void AEnemyController::RunAI()
{
	//upgrade5
	//if (UseBlackboard(BBEnemy, Blackboard)) RunBehaviorTree(BTEnemy);
	RunBehaviorTree(BTEnemy);
}

void AEnemyController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent == nullptr) return;
	BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
}