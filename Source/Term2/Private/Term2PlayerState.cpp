// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

void ATerm2PlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ATerm2PlayerState, CurrentState, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ATerm2PlayerState, bIsWinner, SharedParams);
}