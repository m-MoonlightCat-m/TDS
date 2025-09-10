// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interface/TDS_IntrfcGameActor.h"
#include "../StateEffects/TDS_StateEffect.h"
#include "TDS_EnvironmentStructure.generated.h"

UCLASS()
class TDS_API ATDS_EnvironmentStructure : public AActor, public ITDS_IntrfcGameActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATDS_EnvironmentStructure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EPhysicalSurface GetSurfaceType() override;

	TArray<UTDS_StateEffect*> GetAllCurrentEffects() override;
	void AddEffect(UTDS_StateEffect* newEffect) override;
	void RemoveEffect(UTDS_StateEffect* RemoveEffect) override;

	//Effect
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<UTDS_StateEffect*> Effects;
	
};
