// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../StateEffects/TDS_StateEffect.h"
#include "../FuncLibrary/TDSTypes.h"
#include "TDS_IntrfcGameActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTDS_IntrfcGameActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TDS_API ITDS_IntrfcGameActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual EPhysicalSurface GetSurfaceType();

	virtual TArray<UTDS_StateEffect*> GetAllCurrentEffects();
	virtual void AddEffect(UTDS_StateEffect* newEffect);
	virtual void RemoveEffect(UTDS_StateEffect* RemoveEffect);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DropWeaponToWorld(FDropItem DropItemInfo);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DropAmmoToWorld(EWeaponType TypeAmmo, int32 Cout);
};
