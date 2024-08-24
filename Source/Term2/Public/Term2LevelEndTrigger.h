// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "Term2LevelEndTrigger.generated.h"

class ATerm2GameModeBase;

UCLASS(Blueprintable, BlueprintType)
class TERM2_API ATerm2LevelEndTrigger : public ATriggerVolume
{
	GENERATED_BODY()

public:

	ATerm2LevelEndTrigger();

protected:

	virtual void BeginPlay() override;

private:

	// Custom Overlap function to override the Actor BeginOverlap version. 
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	ATerm2GameModeBase* GameModeRef;

};
