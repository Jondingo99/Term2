// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ATerm2GameModeBase::ATerm2GameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATerm2GameModeBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentGameState = EGameState::Waiting;
}

void ATerm2GameModeBase::ReceivePlayer(APlayerController* PlayerController)
{
	AttemptStartGame();
}

EGameState ATerm2GameModeBase::GetCurrentGameState() const
{
	return CurrentGameState;
}

void ATerm2GameModeBase::PlayerReachedEnd(APlayerController* PlayerController)
{
	//one gamemode base is shared between players in local mp
	CurrentGameState = EGameState::GameOver;
	UTerm2GameWidget** GameWidget = GameWidgets.Find(PlayerController);
	if (GameWidget)
	{
		(*GameWidget)->LevelComplete();
		FInputModeUIOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(true);
		if (PlayerController->GetCharacter() && PlayerController->GetCharacter()->GetCharacterMovement())
		{
			PlayerController->GetCharacter()->GetCharacterMovement()->DisableMovement();
		}
	}
}

void ATerm2GameModeBase::AttemptStartGame()
{
	if (GetNumPlayers() == NumExpectedPlayers)
	{
		DisplayCountdown();
		if (GameCountdownDuration > SMALL_NUMBER)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATerm2GameModeBase::StartGame, GameCountdownDuration, false);
		}
		else
		{
			StartGame();
		}

	}
}

void ATerm2GameModeBase::DisplayCountdown()
{
	if (!GameWidgetClass)
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->PlayerState && !MustSpectate(PlayerController))
		{
			if (UTerm2GameWidget* GameWidget = CreateWidget<UTerm2GameWidget>(PlayerController, GameWidgetClass))
			{
				//GameWidget->AddToViewport();
				GameWidget->AddToPlayerScreen();
				GameWidget->StartCountdown(GameCountdownDuration, this);
				GameWidgets.Add(PlayerController, GameWidget);
			}
		}
	}
}

void ATerm2GameModeBase::StartGame()
{
	CurrentGameState = EGameState::Playing;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->PlayerState && !MustSpectate(PlayerController))
		{
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

void ATerm2GameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (APlayerController* PlayerController = Cast<APlayerController>(NewPlayer))
	{
		if (PlayerController->GetCharacter() && PlayerController->GetCharacter()->GetCharacterMovement())
		{
			PlayerController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
}


