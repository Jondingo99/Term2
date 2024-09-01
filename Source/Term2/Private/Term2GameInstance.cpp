// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2GameInstance.h"
#include "Term2GameWidget.h"
#include "Term2GameStateBase.h"
#include "Term2PlayerController.h"

void UTerm2GameInstance::DisplayCountdown(float GameCountdownDuration, ATerm2PlayerController* Term2PlayerController)
{
	if (GetWorld())
	{
		UTerm2GameWidget* PlayerGameWidget = nullptr;
		UTerm2GameWidget** GameWidget = GameWidgets.Find(Term2PlayerController);
		if (!GameWidget)
		{
			PlayerGameWidget = CreateWidget<UTerm2GameWidget>(Term2PlayerController, GameWidgetClass);
			if (PlayerGameWidget)
			{
				//only do this once
				//atm we never remove this as it has the race time
				PlayerGameWidget->AddToPlayerScreen();
				GameWidgets.Add(Term2PlayerController, PlayerGameWidget);
			}
		}
		else
		{
			PlayerGameWidget = *GameWidget;
		}

		if (PlayerGameWidget)
		{
			//PlayerGameWidget->StartCountdown(GameCountdownDuration, Term2PlayerController);
			Term2GameStateBase = GetWorld()->GetGameState<ATerm2GameStateBase>();
			if (!Term2GameStateBase)
			{
				GetWorld()->GameStateSetEvent.AddUObject(this, &UTerm2GameInstance::OnGameStateSet);
			}
		}
	}
}

void UTerm2GameInstance::OnGameStateSet(AGameStateBase* GameStateBase)
{
	Term2GameStateBase = Cast<ATerm2GameStateBase>(GameStateBase);
}

void UTerm2GameInstance::DisplayLevelComplete(ATerm2PlayerController* Term2PlayerController)
{
	UTerm2GameWidget** GameWidget = GameWidgets.Find(Term2PlayerController);
	if (GameWidget)
	{
//		(*GameWidget)->DisplayResults();
	}
}

void UTerm2GameInstance::RestartGame(ATerm2PlayerController* Term2PlayerController)
{
	UTerm2GameWidget** GameWidget = GameWidgets.Find(Term2PlayerController);
	if (GameWidget)
	{
//		(*GameWidget)->RemoveResults();
		//restore game input 
		FInputModeGameOnly InputMode;
		Term2PlayerController->SetInputMode(InputMode);
		Term2PlayerController->SetShowMouseCursor(false);
	}

}

void UTerm2GameInstance::OnRetrySelected(ATerm2PlayerController* Term2PlayerController)
{
	UTerm2GameWidget** GameWidget = GameWidgets.Find(Term2PlayerController);
	if (GameWidget)
	{
		RestartGame(Term2PlayerController);
//		Term2PlayerController->ServerRestartLevel();
	}
}



