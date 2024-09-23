// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Term2GameInstance.h"
#include "Term2GameStateBase.h"
#include "Term2PlayerController.h"
#include "Term2PlayerState.h"
#include "Term2AIController.h"

ATerm2GameModeBase::ATerm2GameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATerm2GameModeBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentGameState = EGameModeState::Waiting;
	//DisplayCountdown(); // For testing purposes
}

void ATerm2GameModeBase::ReceivePlayer(APlayerController* PlayerController)
{
	UE_LOG(LogTemp, Display, TEXT("ATerm2GameModeBase::ReceivePlayer(APlayerController* PlayerController)"));
	AttemptStartGame();
}

EGameModeState ATerm2GameModeBase::GetCurrentGameState() const
{
	return CurrentGameState;
}

void ATerm2GameModeBase::PlayerReachedEnd(APlayerController* PlayerController)
{
	//one gamemode base is shared between players in local mp
	CurrentGameState = EGameModeState::GameOver;
	UTerm2GameWidget** GameWidget = GameWidgets.Find(PlayerController);
	if (GameWidget)
	{
		//(*GameWidget)->LevelComplete();
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
	if (ATerm2GameStateBase* Term2GameState = GetGameState<ATerm2GameStateBase>())
	{
		Term2GameState->SetGameState(EGameState::Waiting);
	}
	//if (GetNumPlayers() == NumExpectedPlayers)
	//{
		//this needs to be replicated, call a function on game instance and replicate
		DisplayCountdown();
		if (GameCountdownDuration > SMALL_NUMBER)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATerm2GameModeBase::StartGame, GameCountdownDuration, false);
		}
		else
		{
			//this is always called from the authority, aka here
			StartGame();
		}

	//}
}

void ATerm2GameModeBase::DisplayCountdown()
{
	UE_LOG(LogTemp, Display, TEXT("ATerm2GameModeBase::DisplayCountdown()"));

	if (!GameWidgetClass)
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		UE_LOG(LogTemp, Display, TEXT("ATerm2GameModeBase::DisplayCountdown(): we are in our for loop"));
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController)// && PlayerController->PlayerState && !MustSpectate(PlayerController))
		{
			UE_LOG(LogTemp, Display, TEXT("ATerm2GameModeBase::DisplayCountdown(): we have playerstate and are not spectating?"));
			if (UTerm2GameWidget* GameWidget = CreateWidget<UTerm2GameWidget>(PlayerController, GameWidgetClass))
			{
				UE_LOG(LogTemp, Display, TEXT("ATerm2GameModeBase::DisplayCountdown(): we have a widget and we intend to use it!"));
				//GameWidget->AddToViewport();
				GameWidget->AddToPlayerScreen();
				//GameWidget->StartCountdown(GameCountdownDuration, this);
				GameWidgets.Add(PlayerController, GameWidget);
			}
		}
	}
}

void ATerm2GameModeBase::StartGame()
{
	CurrentGameState = EGameModeState::Playing;
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

void ATerm2GameModeBase::RestartGame()
{
	//destroy actor
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		ATerm2AIController* Term2AIController = Cast<ATerm2AIController>(Iterator->Get());
		if (Term2AIController && Term2AIController->GetPawn())
		{
			Term2AIController->Destroy(true);

		}

	}

	ResetLevel();
	//RestartGame();
	//GetWorld()->ServerTravel(TEXT("?Restart"), false);
	//ProcessServerTravel("?Restart");
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->PlayerState && !MustSpectate(PlayerController))
		{
			if (ATerm2PlayerController* Term2PlayerController = Cast<ATerm2PlayerController>(PlayerController))
			{
				//Term2PlayerController->ClientRestartGame();
			}
			RestartPlayer(PlayerController);
		}
	}
}
