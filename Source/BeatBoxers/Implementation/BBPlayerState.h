// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IFighterPlayerState.h"
#include "BBPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API ABBPlayerState : public APlayerState, public IFighterPlayerState
{
	GENERATED_UCLASS_BODY()
	
protected:
	float Special;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxSpecial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn *MyPawn;

	// Begin IBBPlayerStateInterface
	UFUNCTION(BlueprintCallable)
	virtual float GetSpecial() const override;

	UFUNCTION(BlueprintCallable)
	virtual float GetSpecialAsPercent() const override;

	UFUNCTION(BlueprintCallable)
	virtual void AddSpecial(float Amount) override;

	UFUNCTION(BlueprintCallable)
	virtual bool UseSpecial(float Amount) override;
	// End IBBPlayerStateInterface
};
