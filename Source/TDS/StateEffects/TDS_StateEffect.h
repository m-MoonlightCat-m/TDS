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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Once")
	float Timer = 1.0f;

	FTimerHandle TimerHandle_ExecuteOnceTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Execute Once")
	UNiagaraSystem* NiagaraHealthEffect = nullptr;

	UNiagaraComponent* NiagaraHealthEmmiter = nullptr;
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

UCLASS()
class TDS_API UTDS_StateEffect_HealthBoost : public UTDS_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void Boosted();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Health Boost")
	float Power = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Health Boost")
	float Timer = 15.0f;
	
	FTimerHandle TimerHandle_HealthBoostTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Health Boost")
	UNiagaraSystem* NiagaraEffectHealthBoost = nullptr;

	UNiagaraComponent* NiagaraEmmiterHealthBoost = nullptr;
};

UCLASS()
class TDS_API UTDS_StateEffect_Immunity : public UTDS_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	void EndImmunity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Immunity")
	float TimerStartImmunity = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Immunity")
	float TimerEndImmunity = 1.0f;

	FTimerHandle TimerHandle_ImmunityTimer;
	FTimerHandle TimerHandle_EndImmunityTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Immunity")
	UNiagaraSystem* NiagaraEffectStartImmunity = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Immunity")
	UNiagaraSystem* NiagaraEffectEndImmunity = nullptr;

	UNiagaraComponent* NiagaraEmmiterImmunity = nullptr;
};

UCLASS()
class TDS_API UTDS_StateEffect_Stun : public UTDS_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	void EndStun();

	AController* Controller;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Stun")
	float TimerStartStun = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Stun")
	float TimerEffectStun = 1.0f;

	FTimerHandle TimerHandle_StunTimer;
	FTimerHandle TimerHandle_EffectStunTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Stun")
	UNiagaraSystem* NiagaraEffectStun = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Stun")
	UNiagaraSystem* NiagaraEffectEndStun = nullptr;

	UNiagaraComponent* NiagaraEmmiterStun = nullptr;
};

UCLASS()
class TDS_API UTDS_StateEffect_AuraDamage : public UTDS_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	void DealDamage();
	void SpawnLighting(AActor* TargetActor);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Aura Damage")
	float AuraRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Aura Damage")
	float DamagePerTick = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Aura Damage")
	float TimerAuraDamage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Aura Damage")
	float TimerDamageLighting = 1.0f;

	FTimerHandle TimerHandle_AuraDamageTimer;
	FTimerHandle TimerHandle_DamageLightingTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Aura Damage")
	UMaterialInterface* SphereMaterial;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Aura Damage")
	UNiagaraSystem* NiagaraLightingEffect = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting Aura Damage")
	UNiagaraSystem* NiagaraSphereEffect = nullptr;

	UNiagaraComponent* NiagaraEmmiterSphere = nullptr;
	UNiagaraComponent* NiagaraEmmiterLight = nullptr;
};