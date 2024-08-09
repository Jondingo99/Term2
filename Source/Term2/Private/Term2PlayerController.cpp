// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Term2CharacterBase.h"

void ATerm2PlayerController::BeginPlay()
{
}

void ATerm2PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ATerm2PlayerController::RequestJump);
		InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestJumpStop);

		InputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ATerm2PlayerController::RequestCrouchStart);
		InputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestCrouchEnd);
		InputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ATerm2PlayerController::RequestSprintStart);
		InputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestSprintEnd);

		InputComponent->BindAction(TEXT("PullObject"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestPullObject);
		InputComponent->BindAction(TEXT("PullObject"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestStopPullObject);


		InputComponent->BindAxis(TEXT("MoveForward"), this, &ATerm2PlayerController::RequestMoveForward);
		InputComponent->BindAxis(TEXT("MoveRight"), this, &ATerm2PlayerController::RequestMoveRight);
		InputComponent->BindAxis(TEXT("LookUp"), this, &ATerm2PlayerController::RequestLookUp);
		InputComponent->BindAxis(TEXT("LookRight"), this, &ATerm2PlayerController::RequestLookRight);
		InputComponent->BindAxis(TEXT("ThrowObjectGP"), this, &ATerm2PlayerController::RequestThrowObject);
	}
}

void ATerm2PlayerController::RequestMoveForward(float AxisValue)
{
	if (AxisValue != 0.f)
	{
		FRotator const ControlSpaceRot = GetControlRotation();
		//transform world space and add it
		GetPawn()->AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), AxisValue);
	}
}

void ATerm2PlayerController::RequestMoveRight(float AxisValue)
{
	if (AxisValue != 0.f)
	{
		FRotator const ControlSpaceRot = GetControlRotation();
		//transform world space and add it
		GetPawn()->AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), AxisValue);
	}
}

void ATerm2PlayerController::RequestLookUp(float AxisValue)
{
	AddPitchInput(AxisValue * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATerm2PlayerController::RequestLookRight(float AxisValue)
{
	AddYawInput(AxisValue * BaseLookRightRate * GetWorld()->GetDeltaSeconds());
}

void ATerm2PlayerController::RequestJump()
{
	if (GetCharacter())
	{
		GetCharacter()->Jump();
	}
}

void ATerm2PlayerController::RequestJumpStop()
{
	if (GetCharacter())
	{
		GetCharacter()->StopJumping();
	}
}

void ATerm2PlayerController::RequestCrouchStart()
{
	if(!GetCharacter()->GetCharacterMovement()->IsMovingOnGround()) {return;}
	if (GetCharacter())
	{
		GetCharacter()->Crouch();
	}
}

void ATerm2PlayerController::RequestCrouchEnd()
{
	if (GetCharacter())
	{
		GetCharacter()->UnCrouch();
	}
}

void ATerm2PlayerController::RequestSprintStart()
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		Term2CharacterBase->RequestSprintStart();
	}
}

void ATerm2PlayerController::RequestSprintEnd()
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		Term2CharacterBase->RequestSprintEnd();
	}
}

void ATerm2PlayerController::RequestThrowObject(float AxisValue)
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		if (Term2CharacterBase->CanThrowObject())
		{
			float currentDelta = AxisValue - LastAxis;

			// Investigate the CVar declaration, it's probably at the top of this file (if the video ever shows that... if not check the master repo, or refer to Abstraction (Term 1 project))
			////debug
			//if (CVarDisplayLaunchInputDelta->GetBool())
			//{
			//	if (fabs(currentDelta) > 0.0f)
			//	{
			//		UE_LOG(LogTemp, Warning, TEXT("Axis: %f LastAxis: %f currentDelta: %f"), AxisValue, LastAxis, currentDelta);
			//	}
			//}
			LastAxis = AxisValue;
			const bool IsFlick = fabs(currentDelta) > FlickThreshold;
			if (IsFlick)
			{
				Term2CharacterBase->RequestThrowObject();
			}
		}
		else
		{
			LastAxis = 0.0f;
		}
	}
}

void ATerm2PlayerController::RequestPullObject()
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		Term2CharacterBase->RequestPullObject();
	}
}

void ATerm2PlayerController::RequestStopPullObject()
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		Term2CharacterBase->RequestStopPullObject();
	}
}