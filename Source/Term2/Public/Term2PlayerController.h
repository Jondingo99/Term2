// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundCue.h"
#include "Term2PlayerController.generated.h"

class ATerm2GameModeBase;
class ATerm2GameStateBase;
class ATerm2CharacterBase;
class UUserWidget;

UCLASS()
class TERM2_API ATerm2PlayerController : public APlayerController
{
    GENERATED_BODY()
public:

    virtual void BeginPlay() override;
    //in local mp we need to make sure the controller has received the player in order to correctly set up the hud
    virtual void ReceivedPlayer() override;

    virtual void OnPossess(APawn* aPawn) override;
    virtual void OnUnPossess() override;

    /*UFUNCTION(Client, Reliable)
    void ClientDisplayCountdown(float GameCountdownDuration);*/

    /*UFUNCTION(Client, Reliable)
    void ClientRestartGame();*/

    /*UFUNCTION(Client, Reliable)
    void ClientReachedEnd();*/

    /*UFUNCTION(Server, Reliable)
    void ServerRestartLevel();*/
protected:

    void SetupInputComponent() override;

    //bool CanProcessRequest() const;

    void RequestMoveForward(float AxisValue);
    void RequestMoveRight(float AxisValue);
    void RequestLookUp(float AxisValue);
    void RequestLookRight(float AxisValue);
    void RequestThrowObject(float AxisValue);

    void RequestPullorAimObject();
    void RequestStopPullorAimObject();

    void RequestJump();
    void RequestStopJump();

    void RequestCrouchStart();
    void RequestCrouchEnd();

    void RequestSprintStart();
    void RequestSprintEnd();

    UPROPERTY(EditAnywhere, Category = "HUD")
    TSubclassOf<class UUserWidget> HUDClass;

    UPROPERTY()
    UUserWidget* HUDWidget;

    /** Base lookup rate, in deg/sec. Other scaling may affect final lookup rate. */
    UPROPERTY(EditAnywhere, Category = "Look")
    float BaseLookUpRate = 90.0f;

    /** Base lookright rate, in deg/sec. Other scaling may affect final lookup rate. */
    UPROPERTY(EditAnywhere, Category = "Look")
    float BaseLookRightRate = 90.0f;

    /**Sound Cue for Jumping Sound. */

    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundCue* JumpSound = nullptr;

    UPROPERTY()
    ATerm2GameStateBase* TantrumnGameState;

    //used to determine flick of axis
    //float LastDelta = 0.0f;
    float LastAxis = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Input")
    float FlickThreshold = 0.70f;

};