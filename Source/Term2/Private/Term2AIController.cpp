// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2AIController.h"
#include "Term2CharacterBase.h"
#include "Term2PlayerState.h"

void ATerm2AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(InPawn))
	{
		if (ATerm2PlayerState* Term2PlayerState = GetPlayerState<ATerm2PlayerState>())
		{
			Term2PlayerState->SetCurrentState(EPlayerGameState::Waiting);
		}
	}
}

void ATerm2AIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ATerm2AIController::OnReachedEnd()
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		//Term2CharacterBase->ServerPlayCelebrateMontage();
	}
}




