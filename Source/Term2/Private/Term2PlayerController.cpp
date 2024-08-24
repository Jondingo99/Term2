// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Term2CharacterBase.h"
#include "Term2GameModeBase.h"

static TAutoConsoleVariable<bool> CVarDisplayLaunchInputDelta(
	TEXT("Term2.Character.Debug.DisplayLaunchInputDelta"),
	false,
	TEXT("Display Launch Input Delta"),
	ECVF_Default);

void ATerm2PlayerController::BeginPlay()
{
	Super::BeginPlay();
	//GameModeRef = Cast<ATerm2GameModeBase>(GetWorld()->GetAuthGameMode());

}

void ATerm2PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	GameModeRef = GetWorld()->GetAuthGameMode<ATerm2GameModeBase>();
	if (ensureMsgf(GameModeRef, TEXT("ATerm2PlayerController::ReceivedPlayer missing GameMode Reference")))
	{
		GameModeRef->ReceivePlayer(this);
	}

	if (HUDClass)
	{
		HUDWidget = CreateWidget(this, HUDClass);
		if (HUDWidget)
		{
			//HUDWidget->AddToViewport();
			HUDWidget->AddToPlayerScreen();
		}
	}
}

void ATerm2PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ATerm2PlayerController::RequestJump);
		InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestStopJump);

		InputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ATerm2PlayerController::RequestCrouchStart);
		InputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestCrouchEnd);
		InputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ATerm2PlayerController::RequestSprintStart);
		InputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestSprintEnd);

		InputComponent->BindAction(TEXT("PullObject"), EInputEvent::IE_Pressed, this, &ATerm2PlayerController::RequestPullObject);
		InputComponent->BindAction(TEXT("PullObject"), EInputEvent::IE_Released, this, &ATerm2PlayerController::RequestStopPullObject);

		InputComponent->BindAxis(TEXT("MoveForward"), this, &ATerm2PlayerController::RequestMoveForward);
		InputComponent->BindAxis(TEXT("MoveRight"), this, &ATerm2PlayerController::RequestMoveRight);
		InputComponent->BindAxis(TEXT("LookUp"), this, &ATerm2PlayerController::RequestLookUp);
		InputComponent->BindAxis(TEXT("LookRight"), this, &ATerm2PlayerController::RequestLookRight);
		InputComponent->BindAxis(TEXT("ThrowObject"), this, &ATerm2PlayerController::RequestThrowObject);

	}
}

void ATerm2PlayerController::RequestMoveForward(float AxisValue)
{
	/*if(!GameModeRef || GameModeRef->GetCurrentGameState() != EGameState::Playing)
	{
		return;
	}*/

	if (AxisValue != 0.f)
	{
		FRotator const ControlSpaceRot = GetControlRotation();
		// transform to world space and add it
		GetPawn()->AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), AxisValue);
	}
}

void ATerm2PlayerController::RequestMoveRight(float AxisValue)
{
	/*if(!GameModeRef || GameModeRef->GetCurrentGameState() != EGameState::Playing)
	{
		return;
	}*/

	if (AxisValue != 0.f)
	{
		FRotator const ControlSpaceRot = GetControlRotation();
		// transform to world space and add it
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

void ATerm2PlayerController::RequestThrowObject(float AxisValue)
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		if (Term2CharacterBase->CanThrowObject())
		{
			float currentDelta = AxisValue - LastAxis;

			//debug
			if (CVarDisplayLaunchInputDelta->GetBool())
			{
				if (fabs(currentDelta) > 0.0f)
				{
					UE_LOG(LogTemp, Warning, TEXT("Axis: %f LastAxis: %f currentDelta: %f"), AxisValue, LastAxis, currentDelta);
				}
			}
			LastAxis = AxisValue;
			const bool IsFlick = fabs(currentDelta) > FlickThreshold;
			if (IsFlick)
			{
				if (currentDelta > 0)
				{
					Term2CharacterBase->RequestThrowObject();
				}
				else
				{
					Term2CharacterBase->RequestUseObject();
				}
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

void ATerm2PlayerController::RequestJump()
{
	//create a function for this
	//if(!GameModeRef || GameModeRef->GetCurrentGameState() != EGameState::Playing) 
	//{
	//	return;
	//}
	if (GetCharacter())
	{
		GetCharacter()->Jump();

		//SoundCue Triggers
		if (JumpSound && GetCharacter()->GetCharacterMovement()->IsMovingOnGround())
		{
			FVector CharacterLocation = GetCharacter()->GetActorLocation();
			UGameplayStatics::PlaySoundAtLocation(this, JumpSound, CharacterLocation);
		}
	}
}

void ATerm2PlayerController::RequestStopJump()
{
	if (GetCharacter())
	{
		GetCharacter()->StopJumping();
	}
}

void ATerm2PlayerController::RequestCrouchStart()
{
	/*if(!GameModeRef || GameModeRef->GetCurrentGameState() != EGameState::Playing)
	{
		return;
	}*/

	if (!GetCharacter()->GetCharacterMovement()->IsMovingOnGround()) { return; }
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
	//if(!GameModeRef || GameModeRef->GetCurrentGameState() != EGameState::Playing) 
	//{
	//	return;
	//}
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