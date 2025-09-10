// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h" 
#include "TDS_StateEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TDS_API UTDS_StateEffect : public UObject
{
	GENERATED_BODY()
	
public:

	virtual bool InitObject(AActor* Actor);
	
	virtual void DestroyObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TArray<TEnumAsByte<EPhysicalSurface>> PossibleInteractSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	bool bIsStakable = false;

	AActor* myActor = nullptr;
};


UCLASS()
class TDS_API UTDS_StateEffect_ExecuteOnce : public UTDS_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void ExecuteOnce();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Once")
	float Power = 20.0f;


};

UCLASS()
class TDS_API UTDS_StateEffect_ExecuteTimer : public UTDS_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void Execute();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Timer")
	float Power = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Timer")
	float Timer = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Timer")
	float RateTime = 1.0f;

	FTimerHandle TimerHandle_ExecuteTimer;
	FTimerHandle TimerHandle_EffectTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Timer")
	UNiagaraSystem* NiagaraEffect = nullptr;

	UNiagaraComponent* NiagaraEmmiter = nullptr;
};