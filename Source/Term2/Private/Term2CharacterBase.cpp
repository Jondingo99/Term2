// Fill out your copyright notice in the Description page of Project Settings.


#include "Term2CharacterBase.h"


// Sets default values
ATerm2CharacterBase::ATerm2CharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATerm2CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATerm2CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATerm2CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

