// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Term2AIController.generated.h"

/**
 * 
 */
UCLASS()
class TERM2_API ATerm2AIController : public AAIController
{
	GENERATED_BODY()
public:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION()
	void OnReachedEnd();
	
	
};
