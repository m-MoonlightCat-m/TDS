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

bool UTDSGameInstance::GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;

	if (DropItemInfoTable)
	{
		FDropItem* DropItemInfoRow;
		TArray<FName> RowNames = DropItemInfoTable->GetRowNames();

		int8 i = 0;
		while (i < RowNames.Num() && !bIsFind)
		{
			DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowNames[i], "");
			if (DropItemInfoRow->WeaponInfo.NameItem == NameItem)
			{
				OutInfo = (*DropItemInfoRow);
				bIsFind = true;
			}
			 
			i++;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSGameInstance::GetDropItemInfoByWeaponName - DropItemInfoTable"));
	}


	return bIsFind;
}

bool UTDSGameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;
	FDropItem* DropItemInfoRow;

	if (DropItemInfoTable)
	{
		DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(NameItem, "", false);
		if (DropItemInfoRow)
		{
			bIsFind = true;
			OutInfo = *DropItemInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSGameInstance::GetDropItemInfoByName - WeaponTable"));
	}


	return bIsFind;
}
