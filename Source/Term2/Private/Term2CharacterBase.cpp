// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2CharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Term2PlayerController.h"
#include "ThrowableActor.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Term2GameInstance.h"
#include "Term2PlayerState.h"


constexpr int CVSphereCastPlayerView = 0;
constexpr int CVSphereCastActorTransform = 1;
constexpr int CVLineCastActorTransform = 2;

//add cvars for debug
static TAutoConsoleVariable<int> CVarTraceMode(
	TEXT("Term2.Character.Debug.TraceMode"),
	0,
	TEXT("    0: Sphere cast PlayerView is used for direction/rotation (default).\n")
	TEXT("    1: Sphere cast using ActorTransform \n")
	TEXT("    2: Line cast using ActorTransform \n"),
	ECVF_Default);


static TAutoConsoleVariable<bool> CVarDisplayTrace(
	TEXT("Term2.Character.Debug.DisplayTrace"),
	false,
	TEXT("Display Trace"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarDisplayThrowVelocity(
	TEXT("Term2.Character.Debug.DisplayThrowVelocity"),
	false,
	TEXT("Display Throw Velocity"),
	ECVF_Default);

//need a debug to display the launch vector
//make sure collisions are disabled...

// Sets default values
ATerm2CharacterBase::ATerm2CharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ATerm2CharacterBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	SharedParams.Condition = COND_SkipOwner;

	DOREPLIFETIME_WITH_PARAMS_FAST(ATerm2CharacterBase, CharacterThrowState, SharedParams);

	//DOREPLIFETIME(ATerm2CharacterBase, CharacterThrowState);
}

// Called when the game starts or when spawned
void ATerm2CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	EffectCooldown = DefautlEffectCooldown;
	if (GetCharacterMovement())
	{
		MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}
}

// Called every frame
void ATerm2CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
	{
		return;
	}
	/*if (!HasAuthority())
	{
		return;
	}*/
	//this only needs to be done on the owner
	//the pull needs to be done on the server
	//as that's the owner of the objects we are pulling
	UpdateStun();
	if (bIsStunned)
	{
		return;
	}

	if (bIsPlayerBeingRescued)
	{
		UpdateRescue(DeltaTime);
		return;
	}

	if (bIsUnderEffect)
	{
		if (EffectCooldown > 0)
		{
			EffectCooldown -= DeltaTime;
		}
		else
		{
			bIsUnderEffect = false;
			EffectCooldown = DefautlEffectCooldown;
			EndEffect();
		}
	}


	if (CharacterThrowState == ECharacterThrowState::Throwing)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (UAnimMontage* CurrentAnimMontage = AnimInstance->GetCurrentActiveMontage())
			{
				//speed up the playrate when at the throwing part of the animation, as the initial interaction animation wasn't intended as a throw so it's rather slow
				const float PlayRate = AnimInstance->GetCurveValue(TEXT("ThrowCurve"));
				AnimInstance->Montage_SetPlayRate(CurrentAnimMontage, PlayRate);
			}
		}
	}
	//make sure we are in a state that allows to pick up objects
	else if (CharacterThrowState == ECharacterThrowState::None || CharacterThrowState == ECharacterThrowState::RequestingPull)
	{
		switch (CVarTraceMode->GetInt())
		{
		case CVSphereCastPlayerView:
			SphereCastPlayerView();
			break;
		case CVSphereCastActorTransform:
			SphereCastActorTransform();
			break;
		case CVLineCastActorTransform:
			LineCastActorTransform();
			break;
		default:
			SphereCastPlayerView();
			break;
		}
	}
}

// Called to bind functionality to input
void ATerm2CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATerm2CharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//custom landed code
	ATerm2PlayerController* Term2PlayerController = GetController<ATerm2PlayerController>();
	if (Term2PlayerController)
	{
		const float FallImpactSpeed = FMath::Abs(GetVelocity().Z);
		if (FallImpactSpeed < MinImpactSpeed)
		{
			//nothing to do, very light fall
			return;
		}
		else
		{
			//SoundCue Triggers
			if (HeavyLandSound && GetOwner())
			{
				FVector CharacterLocation = GetOwner()->GetActorLocation();
				UGameplayStatics::PlaySoundAtLocation(this, HeavyLandSound, CharacterLocation);
			}
		}

		const float DeltaImpact = MaxImpactSpeed - MinImpactSpeed;
		const float FallRatio = FMath::Clamp((FallImpactSpeed - MinImpactSpeed) / DeltaImpact, 0.0f, 1.0f);
		const bool bAffectSmall = FallRatio <= 0.5;
		const bool bAffectLarge = FallRatio > 0.5;
		Term2PlayerController->PlayDynamicForceFeedback(FallRatio, 0.5f, bAffectLarge, bAffectSmall, bAffectLarge, bAffectSmall);

		if (bAffectLarge)
		{
			OnStunBegin(FallRatio);
		}
	}
}

void ATerm2CharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (!bIsPlayerBeingRescued && (PrevMovementMode == MOVE_Walking && GetCharacterMovement()->MovementMode == MOVE_Falling))
	{
		LastGroundPosition = GetActorLocation() + (GetActorForwardVector() * -100.0f) + (GetActorUpVector() * 100.0f);
	}

	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void ATerm2CharacterBase::FellOutOfWorld(const class UDamageType& dmgType)
{
	FallOutOfWorldPosition = GetActorLocation();
	StartRescue();
}

void ATerm2CharacterBase::RequestSprintStart()
{
	if (!bIsStunned)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}

}
void ATerm2CharacterBase::RequestSprintEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ATerm2CharacterBase::RequestThrowObject()
{
	if (CanThrowObject())
	{
		//to give a responsive feel start playing on the locally owned Actor
		if (PlayThrowMontage())
		{
			CharacterThrowState = ECharacterThrowState::Throwing;
			//now play on all clients
			ServerRequestThrowObject();

		}
		else
		{
			ResetThrowableObject();
		}
	}
}

void ATerm2CharacterBase::ServerRequestThrowObject_Implementation()
{
	//server needs to call the multicast
	MulticastRequestThrowObject();
}

void ATerm2CharacterBase::MulticastRequestThrowObject_Implementation()
{
	//locally controlled actor has already set up binding and played montage
	if (IsLocallyControlled())
	{
		return;
	}

	PlayThrowMontage();
	CharacterThrowState = ECharacterThrowState::Throwing;
}

void ATerm2CharacterBase::RequestPullObject()
{
	//make sure we are in idle
	if (!bIsStunned && CharacterThrowState == ECharacterThrowState::None)
	{
		CharacterThrowState = ECharacterThrowState::RequestingPull;
		ServerRequestPullObject(true);

	}
}

void ATerm2CharacterBase::RequestAim()
{
	if (!bIsStunned && CharacterThrowState == ECharacterThrowState::Attached)
	{
		CharacterThrowState = ECharacterThrowState::Aiming;
		//ServerRequestToggleAim(true);
	}
}

void ATerm2CharacterBase::RequestStopAim()
{
	if (CharacterThrowState == ECharacterThrowState::Aiming)
	{
		CharacterThrowState = ECharacterThrowState::Attached;
		//ServerRequestToggleAim(false);
	}
}

bool ATerm2CharacterBase::AttemptPullObjectAtLocation(const FVector& InLocation)
{
	if (CharacterThrowState != ECharacterThrowState::None && CharacterThrowState != ECharacterThrowState::RequestingPull)
	{
		return false;
	}

	FVector StartPos = GetActorLocation();
	FVector EndPos = InLocation;
	FHitResult HitResult;
	GetWorld() ? GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECollisionChannel::ECC_Visibility) : false;
#if ENABLE_DRAW_DEBUG
	if (CVarDisplayTrace->GetBool())
	{
		DrawDebugLine(GetWorld(), StartPos, EndPos, HitResult.bBlockingHit ? FColor::Red : FColor::White, false);
	}
#endif
	CharacterThrowState = ECharacterThrowState::RequestingPull;
	ProcessTraceResult(HitResult);
	if (CharacterThrowState == ECharacterThrowState::Pulling)
	{
		return true;
	}
	CharacterThrowState = ECharacterThrowState::None;
	return false;
}

void ATerm2CharacterBase::RequestStopPullObject()
{
	//if was pulling an object, drop it
	if (CharacterThrowState == ECharacterThrowState::RequestingPull)
	{
		CharacterThrowState = ECharacterThrowState::None;
		ServerRequestPullObject(false);
		//ResetThrowableObject();
	}
}

void ATerm2CharacterBase::ServerRequestPullObject_Implementation(bool bIsPulling)
{
	CharacterThrowState = bIsPulling ? ECharacterThrowState::RequestingPull : ECharacterThrowState::None;
}

void ATerm2CharacterBase::ServerPullObject_Implementation(AThrowableActor* InThrowableActor)
{
	if (InThrowableActor && InThrowableActor->Pull(this))
	{
		CharacterThrowState = ECharacterThrowState::Pulling;
		ThrowableActor = InThrowableActor;
		ThrowableActor->ToggleHighlight(false);
	}
}

void ATerm2CharacterBase::ClientThrowableAttached_Implementation(AThrowableActor* InThrowableActor)
{
	CharacterThrowState = ECharacterThrowState::Attached;
	ThrowableActor = InThrowableActor;
	MoveIgnoreActorAdd(ThrowableActor);
}

void ATerm2CharacterBase::ServerBeginThrow_Implementation()
{
	//ignore collisions otherwise the throwable object hits the player capsule and doesn't travel in the desired direction
	if (ThrowableActor->GetRootComponent())
	{
		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(ThrowableActor->GetRootComponent());
		if (RootPrimitiveComponent)
		{
			RootPrimitiveComponent->IgnoreActorWhenMoving(this, true);
		}
	}
	//const FVector& Direction = GetMesh()->GetSocketRotation(TEXT("ObjectAttach")).Vector() * -ThrowSpeed;
	const FVector& Direction = GetActorForwardVector() * ThrowSpeed;
	ThrowableActor->Launch(Direction);

	if (CVarDisplayThrowVelocity->GetBool())
	{
		const FVector& Start = GetMesh()->GetSocketLocation(TEXT("ObjectAttach"));
		DrawDebugLine(GetWorld(), Start, Start + Direction, FColor::Red, false, 5.0f);
	}
}

void ATerm2CharacterBase::ServerFinishThrow_Implementation()
{
	//put all this in a function that runs on the server
	CharacterThrowState = ECharacterThrowState::None;
	//this only happened on the locally controlled actor
	MoveIgnoreActorRemove(ThrowableActor);
	if (ThrowableActor->GetRootComponent())
	{
		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(ThrowableActor->GetRootComponent());
		if (RootPrimitiveComponent)
		{
			RootPrimitiveComponent->IgnoreActorWhenMoving(this, false);
		}
	}
	ThrowableActor = nullptr;
}

void ATerm2CharacterBase::ResetThrowableObject()
{
	//drop object
	if (ThrowableActor)
	{
		ThrowableActor->Drop();
	}
	CharacterThrowState = ECharacterThrowState::None;
	ThrowableActor = nullptr;
}

void ATerm2CharacterBase::RequestUseObject()
{
	ApplyEffect_Implementation(ThrowableActor->GetEffectType(), true);
	ThrowableActor->Destroy();
	ResetThrowableObject();
}

void ATerm2CharacterBase::OnThrowableAttached(AThrowableActor* InThrowableActor)
{
	CharacterThrowState = ECharacterThrowState::Attached;
	ThrowableActor = InThrowableActor;
	MoveIgnoreActorAdd(ThrowableActor);
	ClientThrowableAttached(InThrowableActor);
	//InThrowableActor->ToggleHighlight(false);
}

void ATerm2CharacterBase::SphereCastPlayerView()
{
	FVector Location;
	FRotator Rotation;
	if (!GetController())
	{
		return;
	}
	GetController()->GetPlayerViewPoint(Location, Rotation);
	const FVector PlayerViewForward = Rotation.Vector();
	const float AdditionalDistance = (Location - GetActorLocation()).Size();
	FVector EndPos = Location + (PlayerViewForward * (1000.0f + AdditionalDistance));

	const FVector CharacterForward = GetActorForwardVector();
	const float DotResult = FVector::DotProduct(PlayerViewForward, CharacterForward);
	//prevent picking up objects behind us, this is when the camera is looking directly at the characters front side
	if (DotResult < -0.23f)
	{
		if (ThrowableActor)
		{
			ThrowableActor->ToggleHighlight(false);
			ThrowableActor = nullptr;
		}
		return;
		//UE_LOG(LogTemp, Warning, TEXT("Dot Result: %f"), DotResult);
	}


	FHitResult HitResult;
	EDrawDebugTrace::Type DebugTrace = CVarDisplayTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Location, EndPos, 70.0f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore, DebugTrace, HitResult, true);
	ProcessTraceResult(HitResult);

#if ENABLE_DRAW_DEBUG
	if (CVarDisplayTrace->GetBool())
	{
		static float FovDeg = 90.0f;
		DrawDebugCamera(GetWorld(), Location, Rotation, FovDeg);
		DrawDebugLine(GetWorld(), Location, EndPos, HitResult.bBlockingHit ? FColor::Red : FColor::White);
		DrawDebugPoint(GetWorld(), EndPos, 70.0f, HitResult.bBlockingHit ? FColor::Red : FColor::White);
	}
#endif

}

void ATerm2CharacterBase::SphereCastActorTransform()
{
	FVector StartPos = GetActorLocation();
	FVector EndPos = StartPos + (GetActorForwardVector() * 1000.0f);

	//sphere trace
	EDrawDebugTrace::Type DebugTrace = CVarDisplayTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartPos, EndPos, 70.0f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, TArray<AActor*>(), DebugTrace, HitResult, true);
	ProcessTraceResult(HitResult);
}

void ATerm2CharacterBase::LineCastActorTransform()
{

	FVector StartPos = GetActorLocation();
	FVector EndPos = StartPos + (GetActorForwardVector() * 1000.0f);
	FHitResult HitResult;
	GetWorld() ? GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECollisionChannel::ECC_Visibility) : false;
#if ENABLE_DRAW_DEBUG
	if (CVarDisplayTrace->GetBool())
	{
		DrawDebugLine(GetWorld(), StartPos, EndPos, HitResult.bBlockingHit ? FColor::Red : FColor::White, false);
	}
#endif
	ProcessTraceResult(HitResult);
}

void ATerm2CharacterBase::ProcessTraceResult(const FHitResult& HitResult)
{
	//check if there was an existing throwable actor
	//remove the hightlight to avoid wrong feedback 
	AThrowableActor* HitThrowableActor = HitResult.bBlockingHit ? Cast<AThrowableActor>(HitResult.GetActor()) : nullptr;
	const bool IsSameActor = (ThrowableActor == HitThrowableActor);
	const bool IsValidTarget = HitThrowableActor && HitThrowableActor->IsIdle();

	//clean up old actor
	if (ThrowableActor && (!IsValidTarget || !IsSameActor))
	{
		ThrowableActor->ToggleHighlight(false);
		ThrowableActor = nullptr;
	}

	//no target, early out
	if (!IsValidTarget)
	{
		return;
	}

	//new target, set the variable and proceed
	if (!IsSameActor)
	{
		ThrowableActor = HitThrowableActor;
		ThrowableActor->ToggleHighlight(true);
	}

	if (CharacterThrowState == ECharacterThrowState::RequestingPull)
	{
		//don't allow for pulling objects while running/jogging
		if (GetVelocity().SizeSquared() < 100.0f)
		{
			ServerPullObject(ThrowableActor);
			//PullObject(ThrowableActor);
			ThrowableActor->ToggleHighlight(false);
			//ThrowableActor = nullptr;
		}
	}
}

bool ATerm2CharacterBase::PlayThrowMontage()
{
	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = PlayAnimMontage(ThrowMontage, PlayRate) > 0.f;
	if (bPlayedSuccessfully)
	{
		if (IsLocallyControlled())
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (!BlendingOutDelegate.IsBound())
			{
				BlendingOutDelegate.BindUObject(this, &ATerm2CharacterBase::OnMontageBlendingOut);
			}
			AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, ThrowMontage);

			if (!MontageEndedDelegate.IsBound())
			{
				MontageEndedDelegate.BindUObject(this, &ATerm2CharacterBase::OnMontageEnded);
			}
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, ThrowMontage);

			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ATerm2CharacterBase::OnNotifyBeginReceived);
			AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &ATerm2CharacterBase::OnNotifyEndReceived);
		}
	}

	return bPlayedSuccessfully;
}

void ATerm2CharacterBase::UnbindMontage()
{
	if (IsLocallyControlled())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ATerm2CharacterBase::OnNotifyBeginReceived);
			AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &ATerm2CharacterBase::OnNotifyEndReceived);
		}
	}
}

void ATerm2CharacterBase::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{

}

void ATerm2CharacterBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//figure this out
	if (IsLocallyControlled())
	{
		UnbindMontage();
	}

	CharacterThrowState = ECharacterThrowState::None;
	ServerFinishThrow();
	ThrowableActor = nullptr;
}

void ATerm2CharacterBase::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	//do this on server, since server owns the object we are throwing...
	ServerBeginThrow();
}


void ATerm2CharacterBase::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void ATerm2CharacterBase::OnStunBegin(float StunRatio)
{
	if (bIsStunned)
	{
		//for now just early exit, alternative option would be to add to the stun time
		return;
	}

	const float StunDelt = MaxStunTime - MinStunTime;
	StunTime = MinStunTime + (StunRatio * StunDelt);
	StunBeginTimestamp = FApp::GetCurrentTime();
	bIsStunned = true;
	if (bIsSprinting)
	{
		RequestSprintEnd();
	}
	GetMesh();
}

void ATerm2CharacterBase::UpdateStun()
{
	if (bIsStunned)
	{
		bIsStunned = (FApp::GetCurrentTime() - StunBeginTimestamp) < StunTime;
		if (!bIsStunned)
		{
			OnStunEnd();
		}
	}
}

void ATerm2CharacterBase::OnStunEnd()
{
	StunBeginTimestamp = 0.0f;
	StunTime = 0.0f;
}

void ATerm2CharacterBase::UpdateRescue(float DeltaTime)
{
	CurrentRescueTime += DeltaTime;
	float Alpha = FMath::Clamp(CurrentRescueTime / TimeToRescuePlayer, 0.0f, 1.0f);
	FVector NewPlayerLocation = FMath::Lerp(FallOutOfWorldPosition, LastGroundPosition, Alpha);
	SetActorLocation(NewPlayerLocation);

	if (Alpha >= 1.0f)
	{
		EndRescue();
	}
}

void ATerm2CharacterBase::StartRescue()
{
	bIsPlayerBeingRescued = true;
	CurrentRescueTime = 0.0f;
	GetCharacterMovement()->Deactivate();
	SetActorEnableCollision(false);
}
void ATerm2CharacterBase::EndRescue()
{
	GetCharacterMovement()->Activate();
	SetActorEnableCollision(true);
	bIsPlayerBeingRescued = false;
	CurrentRescueTime = 0.0f;
}

void ATerm2CharacterBase::OnRep_CharacterThrowState(const ECharacterThrowState& OldCharacterThrowState)
{
	if (CharacterThrowState != OldCharacterThrowState)
	{
		UE_LOG(LogTemp, Warning, TEXT("OldThrowState: %s"), *UEnum::GetDisplayValueAsText(OldCharacterThrowState).ToString());
		UE_LOG(LogTemp, Warning, TEXT("CharacterThrowState: %s"), *UEnum::GetDisplayValueAsText(CharacterThrowState).ToString());
	}
}

void ATerm2CharacterBase::ApplyEffect_Implementation(EEffectType EffectType, bool bIsBuff)
{
	if (bIsUnderEffect) return;

	CurrentEffect = EffectType;
	bIsUnderEffect = true;
	bIsEffectBuff = bIsBuff;

	switch (CurrentEffect)
	{
	case EEffectType::Speed:
		if (bIsEffectBuff)
		{
			SprintSpeed *= 2;
		}
		else
		{
			GetCharacterMovement()->DisableMovement();
		}
		break;
	case EEffectType::Jump:
		// Implement Jump Buff/Debuff
		break;
	case EEffectType::Power:
		// Implement Power Buff/Debuff
		break;
	default:
		break;
	}
}

void ATerm2CharacterBase::EndEffect()
{
	bIsUnderEffect = false;
	switch (CurrentEffect)
	{
	case EEffectType::Speed:
		bIsEffectBuff ? SprintSpeed /= 2, RequestSprintEnd() : GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		break;
	case EEffectType::Jump:
		// Implement Jump Buff/Debuff
		break;
	case EEffectType::Power:
		// Implement Power Buff/Debuff
		break;
	default:
		break;
	}
}

//bool ATerm2CharacterBase::PlayCelebrateMontage()
//{
//	/*const float PlayRate = 1.0f;
//	bool bPlayedSuccessfully = PlayAnimMontage(CelebrateMontage, PlayRate) > 0.f;
//	if (bPlayedSuccessfully)
//	{
//		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//
//		if (!MontageEndedDelegate.IsBound())
//		{
//			MontageEndedDelegate.BindUObject(this, &ATerm2CharacterBase::OnMontageEnded);
//		}
//		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, CelebrateMontage);
//	}
//
//	return bPlayedSuccessfully;*/
//}

//void ATerm2CharacterBase::ServerPlayCelebrateMontage_Implementation()
//{
//	MulticastPlayCelebrateMontage();
//}

//void ATerm2CharacterBase::MulticastPlayCelebrateMontage_Implementation()
//{
//	PlayCelebrateMontage();
//}