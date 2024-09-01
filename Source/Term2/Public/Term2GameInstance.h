// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Term2GameInstance.generated.h"

class ATerm2PlayerController;
class ATerm2GameStateBase;
class UTerm2GameWidget;

UCLASS()
class TERM2_API UTerm2GameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	void DisplayCountdown(float GameCountdownDuration, ATerm2PlayerController* Term2PlayerController);
	void DisplayLevelComplete(ATerm2PlayerController* Term2PlayerController);

	UFUNCTION(BlueprintPure)
	ATerm2GameStateBase* GetGameState() const { return Term2GameStateBase; }

	UFUNCTION(BlueprintCallable)
	void OnRetrySelected(ATerm2PlayerController* Term2PlayerController);

	UFUNCTION()
	void RestartGame(ATerm2PlayerController* Term2PlayerController);
protected:
	UFUNCTION()
	void OnGameStateSet(AGameStateBase* GameStateBase);

private:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UTerm2GameWidget> GameWidgetClass; // Exposed class to check the type of widget to display

	UPROPERTY()
	TMap<APlayerController*, UTerm2GameWidget*> GameWidgets;

	UPROPERTY()
	ATerm2GameStateBase* Term2GameStateBase;
};
