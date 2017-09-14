// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FighterState.h"
#include "Fighter.h"
#include "FighterWorld.h"
#include "Moveset.h"
#include "InputParser.h"
#include "FighterStateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UFighterStateComponent : public UActorComponent, public IFighterState
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFighterStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
