// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSInventoryComponent.h"
#include "../GameCatalog/TDSGameInstance.h"

// Sets default values for this component's properties
UTDSInventoryComponent::UTDSInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTDSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	for (int8 i = 0; i < WeaponSlot.Num(); i++)
	{
		UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
		if (myGI)
		{
			if (!WeaponSlot[i].NameItem.IsNone())
			{
				FWeaponInfo Info;
				if (myGI->GetWeaponInfoByName(WeaponSlot[i].NameItem, Info))
					WeaponSlot[i].AdditionalInfo.Round = Info.MaxRound;
				/*else
				{
					WeaponSlot.RemoveAt(i);
					i--;
				}*/
			}
		}
	}

	MaxSlotWeapon = WeaponSlot.Num();

	if (WeaponSlot.IsValidIndex(0))
	{
		if (!WeaponSlot[0].NameItem.IsNone())
			OnSwitchWeapon.Broadcast(WeaponSlot[0].NameItem, WeaponSlot[0].AdditionalInfo, 0);
	}
	
}


// Called every frame
void UTDSInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UTDSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	int8 NumSlots = WeaponSlot.Num();
	if (NumSlots == 0)
		return false;

	int8 StartIndex = ChangeToIndex;
	int8 CorrectIndex = ChangeToIndex;

	if (ChangeToIndex >= NumSlots) 
		CorrectIndex = 0;
	else if (ChangeToIndex < 0) 
			CorrectIndex = NumSlots - 1;

	if (WeaponSlot[CorrectIndex].NameItem.IsNone() || WeaponSlot[CorrectIndex].AdditionalInfo.Round == 0)
	{
		int8 CheckedSlots = 0;
		bool bFoundSuitableWeapon = false;

		while (CheckedSlots < NumSlots)
		{
			if (bIsForward)
			{
				StartIndex = (StartIndex + 1) % NumSlots;
			
			}
			else 
			{
				StartIndex = (StartIndex - 1 ) % NumSlots;
			}

			if (!WeaponSlot[StartIndex].NameItem.IsNone() && WeaponSlot[StartIndex].AdditionalInfo.Round > 0)
			{
				CorrectIndex = StartIndex;
				bFoundSuitableWeapon = true;
				break;
			}

			CheckedSlots++;
		}

		if (!bFoundSuitableWeapon)
			return false;
	}

	FName NewIdWeapon = WeaponSlot[CorrectIndex].NameItem;
	FAdditionalWeaponInfo NewAdditionalInfo = WeaponSlot[CorrectIndex].AdditionalInfo;

	bool bHasAmmoOrRound = false;

	// Check if weapon has rounds
	if (NewAdditionalInfo.Round > 0)
	{
		bHasAmmoOrRound = true;
	}
	else
	{
		// Check ammo availability
		FWeaponInfo Info;
		UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
		if (GI && GI->GetWeaponInfoByName(NewIdWeapon, Info))
		{
			for (auto& Ammo : AmmoSlots)
			{
				if (Ammo.WeaponType == Info.WeaponType && Ammo.Cout > 0)
				{
					bHasAmmoOrRound = true;
					break;
				}
			}
		}
	}

	if (bHasAmmoOrRound)
	{
		// Proceed with switching
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, CorrectIndex);
		return true;
	}

	// No ammo or rounds available
	return false;
}


FAdditionalWeaponInfo UTDSInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	FAdditionalWeaponInfo result;
	if (WeaponSlot.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlot.Num() && !bIsFind)
		{
			if (/*WeaponSlot[i].IndexSlot*/i == IndexWeapon)
			{
				result = WeaponSlot[i].AdditionalInfo;
				bIsFind = true;
			}
			i++;
		}
		if (!bIsFind)
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon - Not Found Weapon with index: %d"), IndexWeapon);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon - Not Correct Index Weapon: %d"), IndexWeapon);

	return result;
}

int32 UTDSInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 result = -1;
	int8 i = 0;
	bool bIsFind = false;
	while (i < WeaponSlot.Num() && !bIsFind)
	{
		if (WeaponSlot[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			result = i/*WeaponSlot[i].IndexSlot*/;
		}
		i++;
	}

	return result;
}

FName UTDSInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName result;
	
	if (WeaponSlot.IsValidIndex(IndexSlot))
	{
		result = WeaponSlot[IndexSlot].NameItem;
	}

	return result;
}

void UTDSInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if (WeaponSlot.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlot.Num() && !bIsFind)
		{
			if (/*WeaponSlot[i].IndexSlot*/i == IndexWeapon)
			{
				WeaponSlot[i].AdditionalInfo = NewInfo;
				bIsFind = true;

				OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, NewInfo);
			}
			i++;
		}
		if (!bIsFind)
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Not Found Weapon with index"), IndexWeapon);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Not Correct Index Weapon"), IndexWeapon);
}

void UTDSInventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Cout += CoutChangeAmmo;
			if (AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
				AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;

			OnAmmoChange.Broadcast(TypeWeapon, AmmoSlots[i].Cout);

			bIsFind = true;
		}
		i++;
	}
}

bool UTDSInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8 &AviableAmmoForWeapon)
{
	AviableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;

	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			bIsFind = true;
			AviableAmmoForWeapon = AmmoSlots[i].Cout;

			if (AmmoSlots[i].Cout > 0)
			{
				return true;
			}
		}
		i++;
	}

	OnWeaponAmmoEmpty.Broadcast(TypeWeapon);

	return false;
}

bool UTDSInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !result)
	{
		if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Cout < AmmoSlots[i].MaxCout)
			result = true;
		i++;
	}
	return result;
}

bool UTDSInventoryComponent::CheckCanTakeWeapon(int32 FreeSlot)
{
	bool bIsFreeSlot = false;
	int8 i = 0;
	while (i < WeaponSlot.Num() && !bIsFreeSlot)
	{
		if (WeaponSlot[i].NameItem.IsNone())
		{
			bIsFreeSlot = true;
			FreeSlot = i;
		}
		i++;
	}
	return bIsFreeSlot;
}

bool UTDSInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo)
{
	bool result = false;

	if (WeaponSlot.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
	{
		WeaponSlot[IndexSlot] = NewWeapon;
		
		SwitchWeaponToIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);
		OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);

		result = true;
	}

	return result;
}

bool UTDSInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
	int8 indexSlot = -1;
	if (CheckCanTakeWeapon(indexSlot))
	{
		if (WeaponSlot.IsValidIndex(indexSlot))
		{
			WeaponSlot[indexSlot] = NewWeapon;
			OnUpdateWeaponSlots.Broadcast(indexSlot, NewWeapon);
			return true;
		}
	}
	
	return false;
}

bool UTDSInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool result = false;
	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

	UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		result = myGI->GetDropItemInfoByName(DropItemName, DropItemInfo);

		if (WeaponSlot.IsValidIndex(IndexSlot))
		{
			DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlot[IndexSlot].AdditionalInfo;
		}
		
	}

	return result;
}
