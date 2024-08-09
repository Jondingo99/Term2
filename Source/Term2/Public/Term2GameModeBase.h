// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Term2GameModeBase.generated.h"

//Enum to track the state of the game
UENUM(BlueprintType)
enum class EGameState : uint8
{
	NONE		UMETA(DisplayName = "NONE"),
	Waiting		UMETA(DisplayName = "Waiting"),
	Playing		UMETA(DisplayName = "Playing"),
	Paused		UMETA(DisplayName = "Paused"),
	GameOver	UMETA(DisplayName = "GameOver"),
};

/**
 * 
 */
UCLASS()
class TERM2_API ATerm2GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	//Functions
	ATerm2GameModeBase();

	virtual void BeginPlay() override;

	EGameState GetCurrentGameState();
	void PlayerReachedEnd();

private:

	// VARS

	// Create and set CurrentGameState to NONE. This will be tracked in the code file.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="States", meta = (AllowPrivateAccess = "true"))
	EGameState CurrentGameState = EGameState::NONE;
	//Countdown before gameplay
	UPROPERTY(EditAnywhere, Category="Game Details")
	float GameCountdownDuration = 4.0f;

	FTimerHandle TimerHandle;
	
	//Return to add widget details

	//FUNCTIONS
	
	void DisplayCountdown();
	void StartGame();
};
