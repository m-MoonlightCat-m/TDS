// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDSHealthComponent.h"
#include "TDSCharacterHealthComponent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChange, float, Shield, float, Damage);

UCLASS()
class TDS_API UTDSCharacterHealthComponent : public UTDSHealthComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnShieldChange OnShieldChange;

	FTimerHandle TimerHandle_CoolDawnShieldTimer;
	FTimerHandle TimerHandle_ShieldRecoveryRateTimer;

protected:

	float Shield = 100.0f;

	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecover = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float CollDownShieldRecoverTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoverValue = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoveryRate = 0.1f;

	void ChangeHealthValue(float ChangeValue) override;

	float GetCurrentShield();

	void ChangeShieldValue(float ChangeValue);

	void CoolDownShieldEnd();

	void RecoveryShield();
};
