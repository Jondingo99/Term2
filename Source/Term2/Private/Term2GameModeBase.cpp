// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2GameModeBase.h"

#include "Kismet/GameplayStatics.h"

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

EGameState ATerm2GameModeBase::GetCurrentGameState() const
{
	return CurrentGameState;
}

void ATerm2GameModeBase::PlayerReachedEnd()
{
	CurrentGameState = EGameState::GameOver;
	
	GameWidget->LevelComplete();
	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);

}

void ATerm2GameModeBase::DisplayCountdown()
{
	if (!GameWidgetClass) { return; }

	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	GameWidget = CreateWidget<UTerm2GameWidget>(PC, GameWidgetClass);
	GameWidget->AddToViewport();
	GameWidget->StartCountdown(GameCountdownDuration, this);
}

void ATerm2GameModeBase::StartGame()
{
	CurrentGameState = EGameState::Playing;
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(false);
}



