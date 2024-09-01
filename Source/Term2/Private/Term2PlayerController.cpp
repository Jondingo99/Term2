// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Term2CharacterBase.h"
#include "Term2GameModeBase.h"
//#include "Term2GameInstance.h"
#include "Term2GameStateBase.h"
#include "Term2PlayerState.h"

static TAutoConsoleVariable<bool> CVarDisplayLaunchInputDelta(
	TEXT("Term2.Character.Debug.DisplayLaunchInputDelta"),
	false,
	TEXT("Display Launch Input Delta"),
	ECVF_Default);

void ATerm2PlayerController::BeginPlay()
{
	Super::BeginPlay();
	//Term2GameState = GetWorld()->GetGameState<ATerm2GameStateBase>();
	//ensureMsgf(Term2GameState, TEXT("ATerm2PlayerController::BeginPlay Invalid Term2GameState"));
}
//called from gamemode, so only on authority will we get these calls
void ATerm2PlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	UE_LOG(LogTemp, Warning, TEXT("OnPossess: %s"), *GetName());
}

void ATerm2PlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	UE_LOG(LogTemp, Warning, TEXT("OnUnPossess: %s"), *GetName());
}

//void ATerm2PlayerController::ClientDisplayCountdown_Implementation(float GameCountdownDuration)
//{
//	if (UTerm2GameInstance* Term2GameInstance = GetWorld()->GetGameInstance<UTerm2GameInstance>())
//	{
//		Term2GameInstance->DisplayCountdown(GameCountdownDuration, this);
//	}
//}

//void ATerm2PlayerController::ClientRestartGame_Implementation()
//{
//	if (ATerm2PlayerState* Term2PlayerState = GetPlayerState<ATerm2PlayerState>())
//	{
//		if (UTerm2GameInstance* Term2GameInstance = GetWorld()->GetGameInstance<UTerm2GameInstance>())
//		{
//			Term2GameInstance->RestartGame(this);
//		}
//	}
//}

//void ATerm2PlayerController::ClientReachedEnd_Implementation()
//{
//	//this needs to be named better, it's just displaying the end screen
//	//this will be seperate, as it will come after the montage...
//	//client gets hud authority needs to replicate the montage
//
//	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
//	{
//		Term2CharacterBase->ServerPlayCelebrateMontage();
//		Term2CharacterBase->GetCharacterMovement()->DisableMovement();
//	}
//
//	if (UTerm2GameInstance* Term2GameInstance = GetWorld()->GetGameInstance<UTerm2GameInstance>())
//	{
//		//call the level complete event for the widget...
//	}
//
//	FInputModeUIOnly InputMode;
//	SetInputMode(InputMode);
//	SetShowMouseCursor(true);
//}

//void ATerm2PlayerController::ServerRestartLevel_Implementation()
//{
//	//GetWorld()->ServerTravel(TEXT("?restart"));
//	ATerm2GameModeBase* Term2GameMode = GetWorld()->GetAuthGameMode<ATerm2GameModeBase>();
//	if (ensureMsgf(Term2GameMode, TEXT("ATerm2PlayerController::ServerRestartLevel_Implementation Invalid GameMode")))
//	{
//		//Term2GameMode->RestartGame();
//
//	}
//	/*RestartPlayer()
//	GetWorld()->GetCurrentLevel()->GetName()
//	GetWorld()->ServerTravel(TEXT("?restart"));*/
//}

void ATerm2PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
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

//bool ATerm2PlayerController::CanProcessRequest() const
//{
//	if (Term2GameState && Term2GameState->IsPlaying())
//	{
//		if (ATerm2PlayerState* Term2PlayerState = GetPlayerState<ATerm2PlayerState>())
//		{
//			return (Term2PlayerState->GetCurrentState() == EPlayerGameState::Playing);
//		}
//	}
//
//	return false;
//}

void ATerm2PlayerController::RequestMoveForward(float AxisValue)
{
	/*if (!CanProcessRequest())
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
	/*if (!CanProcessRequest())
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
	/*if (!CanProcessRequest())
	{
		return;
	}*/

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
	/*if (!CanProcessRequest())
	{
		return;
	}*/

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
	/*if (!CanProcessRequest())
	{
		return;
	}*/

	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		Term2CharacterBase->Jump();

		//SoundCue Triggers
		if (JumpSound && Term2CharacterBase->GetCharacterMovement()->IsMovingOnGround())
		{
			FVector CharacterLocation = Term2CharacterBase->GetActorLocation();
			UGameplayStatics::PlaySoundAtLocation(this, JumpSound, CharacterLocation);
		}
	}
}

void ATerm2PlayerController::RequestStopJump()
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		Term2CharacterBase->StopJumping();
	}
}

void ATerm2PlayerController::RequestCrouchStart()
{
	/*if (!CanProcessRequest())
	{
		return;
	}*/

	ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter());
	if (!Term2CharacterBase || !Term2CharacterBase->GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}

	Term2CharacterBase->Crouch();
}

void ATerm2PlayerController::RequestCrouchEnd()
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(GetCharacter()))
	{
		Term2CharacterBase->UnCrouch();
	}
}

void ATerm2PlayerController::RequestSprintStart()
{
	/*if (!CanProcessRequest())
	{
		return;
	}*/

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