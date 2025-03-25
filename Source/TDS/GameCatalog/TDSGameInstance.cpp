// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameInstance.h"

bool UTDSGameInstance::GetWeaponInfoByName(FName NameWaepon, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	if (weaponInfoTable)
	{
		WeaponInfoRow = weaponInfoTable->FindRow<FWeaponInfo>(NameWaepon, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSGameInstance::GetWeaponInfoByName - WeaponTable"));
	}
	

	return bIsFind;
}
