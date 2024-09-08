// Fill out your copyright notice in the Description page of Project Settings.


#include "TBT_TaskAttemptPullObject.h"
#include "Term2AIController.h"
#include "Term2CharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UTBT_TaskAttemptPullObject::UTBT_TaskAttemptPullObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Attempt Pull Object";
	bNotifyTick = false;
	bNotifyTaskFinished = true;

	//accept only vectors
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UTBT_TaskAttemptPullObject, BlackboardKey));
}

EBTNodeResult::Type UTBT_TaskAttemptPullObject::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const ATerm2AIController* Term2AIController = Cast<ATerm2AIController>(OwnerComp.GetOwner());
	ATerm2CharacterBase* Term2CharacterBase = Term2AIController ? Term2AIController->GetPawn<ATerm2CharacterBase>() : nullptr;
	if (Term2CharacterBase)
	{
		if (const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent())
		{
			const FVector TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
			if (Term2CharacterBase->AttemptPullObjectAtLocation(TargetLocation))
			{
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}



