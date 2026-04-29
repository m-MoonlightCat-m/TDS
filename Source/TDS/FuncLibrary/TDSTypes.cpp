#include "TDSTypes.h"
#include "TDS/TDS.h"
#include "../Interface/TDS_IntrfcGameActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include <NiagaraFunctionLibrary.h>

void UTypes::AddEffectBySurfaceType(AActor* TakeEffectAtctor, FName NameBonHit, TSubclassOf<UTDS_StateEffect> AddEffectClass, EPhysicalSurface SurfaceType)
{
	if (SurfaceType != EPhysicalSurface::SurfaceType_Default && TakeEffectAtctor && AddEffectClass)
	{
		UTDS_StateEffect* myEffect = Cast<UTDS_StateEffect>(AddEffectClass->GetDefaultObject());
		if (myEffect)
		{
			bool bIsHavePossibleSurface = false;
			int8 i = 0;
			while (i < myEffect->PossibleInteractSurface.Num() && !bIsHavePossibleSurface)
			{
				if (myEffect->PossibleInteractSurface[i] == SurfaceType)
				{
					bIsHavePossibleSurface = true;
					bool bIsCanAddEffect = false;
					if (!myEffect->bIsStakable)
					{
						int8 j = 0;
						TArray<UTDS_StateEffect*> CurrentEffects;
						ITDS_IntrfcGameActor* myInterface = Cast<ITDS_IntrfcGameActor>(TakeEffectAtctor);
						if (myInterface)
						{
							CurrentEffects = myInterface->GetAllCurrentEffects();
						}

						if (CurrentEffects.Num() >= 0)
						{
							bIsCanAddEffect = true;
							while (j < CurrentEffects.Num() && !bIsCanAddEffect)
							{
								if (CurrentEffects[j]->GetClass() == AddEffectClass)
								{
									bIsCanAddEffect = false;
								}

								j++;
							}
						}
						else
							bIsCanAddEffect = true;
					}
					else
						bIsCanAddEffect = true;

					if (bIsCanAddEffect)
					{
						
						UTDS_StateEffect* NewEffect = NewObject<UTDS_StateEffect>(TakeEffectAtctor, AddEffectClass);
						if (NewEffect)
						{
							NewEffect->InitObject(TakeEffectAtctor, NameBonHit);
						}
					}
				
				}
				i++;
			}
		}
	}
}

void UTypes::ExecuteEffectAdded(UNiagaraSystem* ExecuteFX, AActor* Target, FVector Offset, FName Socket)
{
	if (Target)
	{
		FName SocketToAttached = Socket;
		FVector Loc = Offset;
		ACharacter* myCharacter = Cast<ACharacter>(Target);
		if (myCharacter && myCharacter->GetMesh())
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(ExecuteFX, myCharacter->GetMesh(), SocketToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		}
		else
		{
			if (Target->GetRootComponent())
			{
				UNiagaraFunctionLibrary::SpawnSystemAttached(ExecuteFX, Target->GetRootComponent(), SocketToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
			}
		}
	}
}
