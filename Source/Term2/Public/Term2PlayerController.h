// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Term2PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TERM2_API ATerm2PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	void SetupInputComponent() override;

	void RequestMoveForward(float AxisValue);
	void RequestMoveRight(float AxisValue);
	void RequestLookUp(float AxisValue);
	void RequestLookRight(float AxisValue);
	void RequestJump();
	void RequestJumpStop();

	void RequestCrouchStart();
	void RequestCrouchEnd();

	void RequestSprintStart();
	void RequestSprintEnd();

	//Base Look Up
	UPROPERTY(EditAnywhere, Category = "Look")
	float BaseLookUpRate = 90.0f;

	//Base Look Right
	UPROPERTY(EditAnywhere, Category = "Look")
	float BaseLookRightRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 1200.0f;
	
};
