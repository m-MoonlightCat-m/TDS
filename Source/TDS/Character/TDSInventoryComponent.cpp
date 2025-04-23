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
	//bool bIsSuccess = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlot.Num() - 1)
		CorrectIndex = 0;
	else
		if (ChangeToIndex < 0)
			CorrectIndex = WeaponSlot.Num() - 1;

	/*FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;
	int32 NewCurrentIndex = 0;*/

	if (WeaponSlot.IsValidIndex(CorrectIndex))
	{
		if (!WeaponSlot[CorrectIndex].NameItem.IsNone())
		{
			if (WeaponSlot[CorrectIndex].AdditionalInfo.Round > 0)
			{
				bIsSuccess = true;
			}
			else
			{
				UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
				if (myGI)
				{
					FWeaponInfo myInfo;
					myGI->GetWeaponInfoByName(WeaponSlot[CorrectIndex].NameItem, myInfo);

					bool bIsFind = false;
					int8 j = 0;
					while (j < AmmoSlots.Num() && !bIsFind)
					{
						if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
						{
							bIsSuccess = true;
							bIsFind = true;
						}
						j++;
					}
				}
			}

			if (bIsSuccess)
			{
				NewCurrentIndex = CorrectIndex;
				NewIdWeapon = WeaponSlot[CorrectIndex].NameItem;
				NewAdditionalInfo = WeaponSlot[CorrectIndex].AdditionalInfo;
			}
		}
	}


	if (!bIsSuccess)
	{
		if (bIsForward)
		{
			int8 iteration = 0;
			int8 SecondIteration = 0;

			while (iteration < WeaponSlot.Num() && !bIsSuccess)
			{
				iteration++;
				int8 tmpIndex = ChangeToIndex + iteration;

				if (WeaponSlot.IsValidIndex(tmpIndex))
				{
					/*if (!WeaponSlot[tmpIndex].NameItem.IsNone())
					{
						if (WeaponSlot[tmpIndex].AdditionalInfo.Round > 0)
						{
							bIsSuccess = true;
							NewIdWeapon = WeaponSlot[tmpIndex].NameItem;
							NewAdditionalInfo = WeaponSlot[tmpIndex].AdditionalInfo;
							NewCurrentIndex = tmpIndex;
						}
						else
						{
							FWeaponInfo myInfo;
							UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
							myGI->GetWeaponInfoByName(WeaponSlot[tmpIndex].NameItem, myInfo);

							bool bIsFind = false;
							int8 j = 0;
							while (j < AmmoSlots.Num() && !bIsFind)
							{
								if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
								{
									bIsSuccess = true;
									NewIdWeapon = WeaponSlot[tmpIndex].NameItem;
									NewAdditionalInfo = WeaponSlot[tmpIndex].AdditionalInfo;
									NewCurrentIndex = tmpIndex;
									bIsFind = true;
								}
								j++;
							}
						}
					}*/
					ScrollWeapon(tmpIndex);
				}
				else
				{
					if (OldIndex != SecondIteration)
					{
						if (WeaponSlot.IsValidIndex(SecondIteration))
						{
							/*if (!WeaponSlot[SecondIteration].NameItem.IsNone())
							{
								if (WeaponSlot[SecondIteration].AdditionalInfo.Round > 0)
								{
									bIsSuccess = true;
									NewIdWeapon = WeaponSlot[SecondIteration].NameItem;
									NewAdditionalInfo = WeaponSlot[SecondIteration].AdditionalInfo;
									NewCurrentIndex = 0;
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlot[SecondIteration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
										{
											bIsSuccess = true;
											NewIdWeapon = WeaponSlot[SecondIteration].NameItem;
											NewAdditionalInfo = WeaponSlot[SecondIteration].AdditionalInfo;
											NewCurrentIndex = SecondIteration;
											bIsFind = true;
										}
										j++;
									}
								}
							}*/
							ScrollWeapon(SecondIteration);
						}
					}
					else
					{
						if (WeaponSlot.IsValidIndex(SecondIteration))
						{
							/*if (!WeaponSlot[SecondIteration].NameItem.IsNone())
							{
								if (WeaponSlot[SecondIteration].AdditionalInfo.Round > 0)
								{
									//WeaponGood, it same weapon do nothing
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlot[SecondIteration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
										{
											if (AmmoSlots[j].Cout > 0)
											{
												//WeaponGood, it same weapon do nothing
											}
											else
											{
												UE_LOG(LogTemp, Error, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
											}
										}
										j++;
									}
								}
							}*/
							ScrollWeaponLast(SecondIteration);
						}
					}
					SecondIteration++;
				}

			}
		}
		else
		{
			int8 iteration = 0;
			int8 SecondIteration = WeaponSlot.Num() - 1;

			while (iteration < WeaponSlot.Num() && !bIsSuccess)
			{
				iteration++;
				int8 tmpIndex = ChangeToIndex - iteration;

				if (WeaponSlot.IsValidIndex(tmpIndex))
				{
					/*if (!WeaponSlot[tmpIndex].NameItem.IsNone())
					{
						if (WeaponSlot[tmpIndex].AdditionalInfo.Round > 0)
						{
							bIsSuccess = true;
							NewIdWeapon = WeaponSlot[tmpIndex].NameItem;
							NewAdditionalInfo = WeaponSlot[tmpIndex].AdditionalInfo;
							NewCurrentIndex = tmpIndex;
						}
						else
						{
							FWeaponInfo myInfo;
							UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
							myGI->GetWeaponInfoByName(WeaponSlot[tmpIndex].NameItem, myInfo);

							bool bIsFind = false;
							int8 j = 0;
							while (j < AmmoSlots.Num() && !bIsFind)
							{
								if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
								{
									bIsSuccess = true;
									NewIdWeapon = WeaponSlot[tmpIndex].NameItem;
									NewAdditionalInfo = WeaponSlot[tmpIndex].AdditionalInfo;
									NewCurrentIndex = tmpIndex;
									bIsFind = true;
								}
								j++;
							}
						}
					}*/
					ScrollWeapon(tmpIndex);
				}
				else
				{
					if (OldIndex != SecondIteration)
					{
						if (WeaponSlot.IsValidIndex(SecondIteration))
						{
							/*if (!WeaponSlot[SecondIteration].NameItem.IsNone())
							{
								if (WeaponSlot[SecondIteration].AdditionalInfo.Round > 0)
								{
									bIsSuccess = true;
									NewIdWeapon = WeaponSlot[SecondIteration].NameItem;
									NewAdditionalInfo = WeaponSlot[SecondIteration].AdditionalInfo;
									NewCurrentIndex = 0;
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlot[SecondIteration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
										{
											bIsSuccess = true;
											NewIdWeapon = WeaponSlot[SecondIteration].NameItem;
											NewAdditionalInfo = WeaponSlot[SecondIteration].AdditionalInfo;
											NewCurrentIndex = SecondIteration;
											bIsFind = true;
										}
										j++;
									}
								}
							}*/
							ScrollWeapon(SecondIteration);
						}
					}
					else
					{
						if (WeaponSlot.IsValidIndex(SecondIteration))
						{
							/*if (!WeaponSlot[SecondIteration].NameItem.IsNone())
							{
								if (WeaponSlot[SecondIteration].AdditionalInfo.Round > 0)
								{
									//WeaponGood, it same weapon do nothing
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlot[SecondIteration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
										{
											if (AmmoSlots[j].Cout > 0)
											{
												//WeaponGood, it same weapon do nothing
											}
											else
											{
												UE_LOG(LogTemp, Error, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
											}
										}
										j++;
									}
								}
							}*/
							ScrollWeaponLast(SecondIteration);
						}
					}
					SecondIteration--;
				}
			}
		}
	}

	if (bIsSuccess)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, 0);
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
			if (/*WeaponSlot[i].IndexSlot*/i == IndexWeapon)
			{
				result = WeaponSlot[i].AdditionalInfo;
				bIsFind = true;
			}
			i++;
		}
		if (!bIsFind)
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon - Not Found Weapon with index"), IndexWeapon);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon - Not Correct Index Weapon"), IndexWeapon);

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


void UTDSInventoryComponent::ScrollWeapon(int32 ScrollIndex)
{
	if (!WeaponSlot[ScrollIndex].NameItem.IsNone())
	{
		if (WeaponSlot[ScrollIndex].AdditionalInfo.Round > 0)
		{
			bIsSuccess = true;
			NewIdWeapon = WeaponSlot[ScrollIndex].NameItem;
			NewAdditionalInfo = WeaponSlot[ScrollIndex].AdditionalInfo;
			NewCurrentIndex = ScrollIndex;
		}
		else
		{
			FWeaponInfo myInfo;
			UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
			myGI->GetWeaponInfoByName(WeaponSlot[ScrollIndex].NameItem, myInfo);

			bool bIsFind = false;
			int8 j = 0;
			while (j < AmmoSlots.Num() && !bIsFind)
			{
				if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
				{
					bIsSuccess = true;
					NewIdWeapon = WeaponSlot[ScrollIndex].NameItem;
					NewAdditionalInfo = WeaponSlot[ScrollIndex].AdditionalInfo;
					NewCurrentIndex = ScrollIndex;
					bIsFind = true;
				}
				j++;
			}
		}
	}
}

void UTDSInventoryComponent::ScrollWeaponLast(int32 ScrollSecondIndex)
{
	if (WeaponSlot[ScrollSecondIndex].AdditionalInfo.Round > 0)
	{
		//WeaponGood, it same weapon do nothing
	}
	else
	{
		FWeaponInfo myInfo;
		UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

		myGI->GetWeaponInfoByName(WeaponSlot[ScrollSecondIndex].NameItem, myInfo);

		bool bIsFind = false;
		int8 j = 0;
		while (j < AmmoSlots.Num() && !bIsFind)
		{
			if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
			{
				if (AmmoSlots[j].Cout > 0)
				{
					//WeaponGood, it same weapon do nothing
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
				}
			}
			j++;
		}
	}
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

void UTDSInventoryComponent::SwitchWeaponToInventory()
{

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
