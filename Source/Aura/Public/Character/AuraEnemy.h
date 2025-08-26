// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/TargetableInterface.h"
#include "AuraEnemy.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public ITargetableInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
};
