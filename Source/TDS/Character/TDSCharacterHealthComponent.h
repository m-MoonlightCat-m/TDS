// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDSHealthComponent.h"
#include "TDSCharacterHealthComponent.generated.h"

/**
 * 
 */
UCLASS()
class TDS_API UTDSCharacterHealthComponent : public UTDSHealthComponent
{
	GENERATED_BODY()
	
public:

	void ChangeCurrentHealth(float ChangeValue) override;
};
