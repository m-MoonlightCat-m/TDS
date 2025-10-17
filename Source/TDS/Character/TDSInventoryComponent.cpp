// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSInventoryComponent.h"
#include "../GameCatalog/TDSGameInstance.h"
#include "../Interface/TDS_IntrfcGameActor.h"

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

bool UTDSInventoryComponent::SwitchWeaponToIndexByNextPreviosIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	int32 NumSlots = WeaponSlot.Num();
	if (NumSlots == 0)
		return false;

	int32 CorrectIndex = (ChangeToIndex + NumSlots) % NumSlots;

	int32 NewIndex = -1;
	bool bFound = false;

	if (WeaponSlot.IsValidIndex(CorrectIndex) && !WeaponSlot[CorrectIndex].NameItem.IsNone())
	{
		bool bCanUseThisSlot = false;

		if (WeaponSlot[CorrectIndex].AdditionalInfo.Round > 0)
		{
			bCanUseThisSlot = true;
		}
		else
		{
			UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
			if (GI)
			{
				FWeaponInfo Info;
				if (GI->GetWeaponInfoByName(WeaponSlot[CorrectIndex].NameItem, Info))
				{
					for (auto& Ammo : AmmoSlots)
					{
						if (Ammo.WeaponType == Info.WeaponType && Ammo.Cout > 0)
						{
							bCanUseThisSlot = true;
							break;
						}
					}
				}
			}
		}

		if (bCanUseThisSlot)
		{
			NewIndex = CorrectIndex;
			bFound = true;
		}
	}

	if (!bFound)
	{
		for (int32 i = 1; i < NumSlots && !bFound; ++i)
		{
			int32 CheckIndex = 0;
			if (bIsForward)
				CheckIndex = (CorrectIndex + i) % NumSlots;
			else
				CheckIndex = (CorrectIndex - i + NumSlots) % NumSlots;

			if (WeaponSlot.IsValidIndex(CheckIndex) && !WeaponSlot[CheckIndex].NameItem.IsNone())
			{
				bool bCanUseThisSlot = false;

				if (WeaponSlot[CheckIndex].AdditionalInfo.Round > 0)
				{
					bCanUseThisSlot = true;
				}
				else
				{
					UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
					if (GI)
					{
						FWeaponInfo Info;
						if (GI->GetWeaponInfoByName(WeaponSlot[CheckIndex].NameItem, Info))
						{
							for (auto& Ammo : AmmoSlots)
							{
								if (Ammo.WeaponType == Info.WeaponType && Ammo.Cout > 0)
								{
									bCanUseThisSlot = true;
									break;
								}
							}
						}
					}
				}

				if (bCanUseThisSlot)
				{
					NewIndex = CheckIndex;
					bFound = true;
				}
			}
		}
	}

	if (!bFound)
		return false;

	SetAdditionalInfoWeapon(OldIndex, OldInfo);
	OnSwitchWeapon.Broadcast(WeaponSlot[NewIndex].NameItem, WeaponSlot[NewIndex].AdditionalInfo, NewIndex);

	return true;
}

bool UTDSInventoryComponent::SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviosIndex, FAdditionalWeaponInfo PreviosWeaponInfo)
{
	bool bIsSuccess = false;
	FName ToSwitchIdWeapon;
	FAdditionalWeaponInfo ToSwitchAdditionalInfo;

	ToSwitchIdWeapon = GetWeaponNameBySlotIndex(IndexWeaponToChange);
	ToSwitchAdditionalInfo = GetAdditionalInfoWeapon(IndexWeaponToChange);

	if (!ToSwitchIdWeapon.IsNone())
	{
		SetAdditionalInfoWeapon(PreviosIndex, PreviosWeaponInfo);
		OnSwitchWeapon.Broadcast(ToSwitchIdWeapon, ToSwitchAdditionalInfo, IndexWeaponToChange);

		EWeaponType ToSwitchWeaponType;
		if (GetWeaponTypeByNameWeapon(ToSwitchIdWeapon, ToSwitchWeaponType))
		{
			int8 AviableAmmoForWeapom = -1;
			if (CheckAmmoForWeapon(ToSwitchWeaponType, AviableAmmoForWeapom))
			{

			}
		}

		bIsSuccess = true;
	}

	return bIsSuccess;
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
			if (i == IndexWeapon)
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
			result = i;
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

bool UTDSInventoryComponent::GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleMKType;
	UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	
	if (myGI)
	{
		if (WeaponSlot.IsValidIndex(IndexSlot))
		{
			myGI->GetWeaponInfoByName(WeaponSlot[IndexSlot].NameItem, OutInfo);
			WeaponType = OutInfo.WeaponType;
			bIsFind = true;
		}
	}

	return bIsFind;
}

bool UTDSInventoryComponent::GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleMKType;
	UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

	if (myGI)
	{
		myGI->GetWeaponInfoByName(IdWeaponName, OutInfo);
		WeaponType = OutInfo.WeaponType;
		bIsFind = true;
	}

	return bIsFind;
}

void UTDSInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if (WeaponSlot.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlot.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
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

	if (AviableAmmoForWeapon <= 0)
		OnWeaponAmmoEmpty.Broadcast(TypeWeapon);
	else
		OnWeaponAmmoAviable.Broadcast(TypeWeapon);

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

bool UTDSInventoryComponent::CheckCanTakeWeapon(int32& FreeSlot)
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
		
		SwitchWeaponToIndexByNextPreviosIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);
		OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);

		result = true;
	}

	return result;
}

bool UTDSInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
	int32 indexSlot = -1;
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

void UTDSInventoryComponent::DropWeaponByIndex(int32 ByIndex, FDropItem& DropItemInfo)
{
	FWeaponSlot EmptyWeaponSlot;

	bool bIsCanDrop = false;
	int8 i = 0;
	int8 AviableWeaponNum = 0;

	while (i < WeaponSlot.Num() && !bIsCanDrop)
	{
		if (!WeaponSlot[i].NameItem.IsNone())
		{
			AviableWeaponNum++;
			if (AviableWeaponNum > 1)
				bIsCanDrop = true;
		}
		i++;
	}

	if (bIsCanDrop && WeaponSlot.IsValidIndex(ByIndex) && GetDropItemInfoFromInventory(ByIndex, DropItemInfo))
	{
		GetDropItemInfoFromInventory(ByIndex, DropItemInfo);

		bool bIsFindWeapon = false;
		int8 j = 0;
		while (j < WeaponSlot.Num() && !bIsFindWeapon)
		{
			if (!WeaponSlot[j].NameItem.IsNone())
			{
				OnSwitchWeapon.Broadcast(WeaponSlot[j].NameItem, WeaponSlot[j].AdditionalInfo, j);
			}
			j++;
		}

		WeaponSlot[ByIndex] = EmptyWeaponSlot;
		if (GetOwner()->GetClass()->ImplementsInterface(UTDS_IntrfcGameActor::StaticClass()))
			ITDS_IntrfcGameActor::Execute_DropWeaponToWorld(GetOwner(), DropItemInfo);

		OnUpdateWeaponSlots.Broadcast(ByIndex, EmptyWeaponSlot);
	}
}

bool UTDSInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool result = false;
	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

	UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		result = myGI->GetDropItemInfoByWeaponName(DropItemName, DropItemInfo);

		if (WeaponSlot.IsValidIndex(IndexSlot))
		{
			DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlot[IndexSlot].AdditionalInfo;
		}
		
	}

	return result;
}
