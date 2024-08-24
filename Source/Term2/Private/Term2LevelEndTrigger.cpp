// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2LevelEndTrigger.h"
#include "Term2CharacterBase.h"
#include "Term2GameModeBase.h"
#include "Term2PlayerController.h"

ATerm2LevelEndTrigger::ATerm2LevelEndTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ATerm2LevelEndTrigger::OnOverlapBegin);
}

void ATerm2LevelEndTrigger::BeginPlay()
{
	Super::BeginPlay();
	GameModeRef = GetWorld()->GetAuthGameMode<ATerm2GameModeBase>();
}

void ATerm2LevelEndTrigger::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ATerm2CharacterBase* Term2CharacterBase = Cast<ATerm2CharacterBase>(OtherActor))
	{
		APlayerController* PlayerController = Term2CharacterBase->GetController<APlayerController>();
		GameModeRef->PlayerReachedEnd(PlayerController);
	}
}




