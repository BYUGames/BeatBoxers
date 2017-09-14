// Fill out your copyright notice in the Description page of Project Settings.

#include "FighterCharacter.h"


// Sets default values
AFighterCharacter::AFighterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FighterState = CreateDefaultSubobject<UFighterStateComponent>(TEXT("FighterState"));
	Moveset = CreateDefaultSubobject<UMovesetComponent>(TEXT("Moveset"));
	InputParser = CreateDefaultSubobject<UInputParserComponent>(TEXT("InputParser"));
	SoloTracker = CreateDefaultSubobject<USoloTrackerComponent>(TEXT("SoloTracker"));
}

// Called when the game starts or when spawned
void AFighterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

