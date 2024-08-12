// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Term2GameWidget.generated.h"

/**
 * 
 */
UCLASS()
class TERM2_API UTerm2GameWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BluePrintImplementableEvent)
	void StartCountdown(float CountdownTime, class ATerm2GameModeBase* GameMode);

	UFUNCTION(BlueprintImplementableEvent)
	void LevelComplete();
	
	
};
