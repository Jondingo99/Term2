// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Term2GameWidget.h"
#include "GameFramework/GameModeBase.h"
#include "Term2GameModeBase.generated.h"

// Enum to track the current state of the game 
UENUM(BlueprintType)
enum class EGameModeState : uint8
{
	None		UMETA(DisplayName = "None"),
	Waiting		UMETA(DisplayName = "Waiting"),
	Playing		UMETA(DisplayName = "Playing"),
	Paused		UMETA(DisplayName = "Paused"),
	GameOver	UMETA(DisplayName = "GameOver"),
};

class AController;

UCLASS()
class TERM2_API ATerm2GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	// --- FUNCTIONS --- //
	ATerm2GameModeBase();

	virtual void BeginPlay() override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	EGameModeState GetCurrentGameState() const;
	void PlayerReachedEnd(APlayerController* PlayerController);

	void ReceivePlayer(APlayerController* PlayerController);

private:

	// --- VARS --- //

	// Create and set CurrentGameState to NONE. This will be tracked in the code file. 
	UPROPERTY(VisibleAnywhere, Category = "States")
	EGameModeState CurrentGameState = EGameModeState::None;
	// Countdown before gameplay state begins. Exposed so we can easily change this in BP editor. 
	UPROPERTY(EditAnywhere, Category = "Game Details")
	float GameCountdownDuration = 4.0f;

	UFUNCTION(BlueprintCallable, Category = "Game Details")
	void SetNumExpectedPlayers(uint8 InNumExpectedPlayers) { NumExpectedPlayers = InNumExpectedPlayers; }

	UPROPERTY(EditAnywhere, Category = "Game Details")
	uint8 NumExpectedPlayers = 1u;


	FTimerHandle TimerHandle;

	UPROPERTY()
	TMap<APlayerController*, UTerm2GameWidget*> GameWidgets; // Object we'll be creating and adding to the Viewport
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UTerm2GameWidget> GameWidgetClass; // Exposed class to check the type of widget to display

	// --- FUNCTIONS --- //
	void AttemptStartGame();
	void DisplayCountdown();
	void StartGame();
	void RestartGame();
};
