// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Term2GameWidget.h"
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

	UFUNCTION(BlueprintCallable)
	EGameState GetCurrentGameState() const;

	void PlayerReachedEnd();

private:

	// VARS

	// Create and set CurrentGameState to NONE. This will be tracked in the code file.
	UPROPERTY(VisibleAnywhere, Category="States")
	EGameState CurrentGameState = EGameState::NONE;
	//Countdown before gameplay
	UPROPERTY(EditAnywhere, Category="Game Details")
	float GameCountdownDuration = 4.0f;

	FTimerHandle TimerHandle;

	UPROPERTY()
	UTerm2GameWidget* GameWidget; //object we'll be creating and adding to viewport

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UTerm2GameWidget> GameWidgetClass; //exposed class to check the type of widget

	APlayerController* PC = nullptr;

	//FUNCTIONS
	
	void DisplayCountdown();
	void StartGame();
};
