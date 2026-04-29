// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../StateEffects/TDS_StateEffect.h"
#include "../Interface/TDS_IntrfcGameActor.h"
#include "TDS_EnemyCharacter.generated.h"

UCLASS()
class TDS_API ATDS_EnemyCharacter : public ACharacter, public ITDS_IntrfcGameActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDS_EnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlag) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddEffect(UTDS_StateEffect* newEffect);
	void AddEffect_Implementation(UTDS_StateEffect* newEffect) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveEffect(UTDS_StateEffect* RemoveEffect);
	void RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect) override;

	//Effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	TArray<UNiagaraComponent*> NiagaraSystemEffects;

	UPROPERTY(Replicated, BlueprintReadOnly, EditDefaultsOnly, Category = "Setting")
	TArray<UTDS_StateEffect*> Effects;
	UPROPERTY(ReplicatedUsing = EffectAdd_OnRep)
	UTDS_StateEffect* EffectAdd = nullptr;
	UPROPERTY(ReplicatedUsing = EffectRemove_OnRep)
	UTDS_StateEffect* EffectRemove = nullptr;

	UFUNCTION()
	void EffectAdd_OnRep();
	UFUNCTION()
	void EffectRemove_OnRep();

	UFUNCTION(Server, Reliable)
	void ExecuteEffectAdded_OnServer(UNiagaraSystem* ExecuteFX);
	UFUNCTION(NetMulticast, Reliable)
	void ExecuteEffectAdded_Multicast(UNiagaraSystem* ExecuteFX);

	UFUNCTION()
	void SwitchEffect(UTDS_StateEffect* Effect, bool bIsAdd);
};
