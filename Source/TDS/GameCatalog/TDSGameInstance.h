// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../FuncLibrary/TDSTypes.h"
#include "../Weapons/WeaponDefault.h"
#include "TDSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TDS_API UTDSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSetting")
	UDataTable* weaponInfoTable = nullptr;
	UFUNCTION(BlueprintCallable)
	bool GetWeaponInfoByName(FName NameWaepon, FWeaponInfo& OutInfo);
};
