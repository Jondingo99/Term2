// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2GameModeBase.h"

ATerm2GameModeBase::ATerm2GameModeBase()
{

}

void ATerm2GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentGameState = EGameState::Waiting;
	DisplayCountdown();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATerm2GameModeBase::StartGame, GameCountdownDuration, false);
}

EGameState ATerm2GameModeBase::GetCurrentGameState()
{
	return CurrentGameState;
}

void ATerm2GameModeBase::PlayerReachedEnd()
{
	CurrentGameState = EGameState::GameOver;
	//To do - Return to update widget

	//To do - Return to control PlayerController Input State

}

void ATerm2GameModeBase::DisplayCountdown()
{
	//To do - Return and create, store, display widget
}

void ATerm2GameModeBase::StartGame()
{
	CurrentGameState = EGameState::Playing;
}



