// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Term2CharacterBase.h"
#include "Term2PlayerController.h"
#include "Term2PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
//only ever called by the authority
void ATerm2GameStateBase::OnPlayerReachedEnd(ATerm2CharacterBase* Term2Character)
{
	ensureMsgf(HasAuthority(), TEXT("ATerm2GameStateBase::OnPlayerReachedEnd being called from Non Authority!"));
	if (ATerm2PlayerController* Term2PlayerController = Term2Character->GetController<ATerm2PlayerController>())
	{

		//Term2PlayerController->ClientReachedEnd();
		Term2Character->GetCharacterMovement()->DisableMovement();

		ATerm2PlayerState* PlayerState = Term2PlayerController->GetPlayerState<ATerm2PlayerState>();
		if (PlayerState)
		{
			const bool IsWinner = Results.Num() == 0;
			PlayerState->SetIsWinner(IsWinner);
			PlayerState->SetCurrentState(EPlayerGameState::Finished);
		}

		FGameResult Result;
		Result.Name = Term2Character->GetName();
		//TODO get the actual time it took in order to post to a leaderboard/results widget
		Result.Time = 5.0f;
		Results.Add(Result);

		//TODO this will not work once JIP(Join In Progress) is enabled
		if (Results.Num() == PlayerArray.Num())
		{
			GameState = EGameState::GameOver;
		}
	}
}

void ATerm2GameStateBase::ClearResults()
{
	Results.Empty();
}

void ATerm2GameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	//SharedParams.Condition = COND_SkipOwner;

	DOREPLIFETIME_WITH_PARAMS_FAST(ATerm2GameStateBase, GameState, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ATerm2GameStateBase, Results, SharedParams);

	//DOREPLIFETIME(ATerm2CharacterBase, CharacterThrowState);
}

void ATerm2GameStateBase::OnRep_GameState(const EGameState& OldGameState)
{
	UE_LOG(LogTemp, Warning, TEXT("OldGameState: %s"), *UEnum::GetDisplayValueAsText(OldGameState).ToString());
	UE_LOG(LogTemp, Warning, TEXT("GameState: %s"), *UEnum::GetDisplayValueAsText(GameState).ToString());
}



