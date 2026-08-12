// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_EnvironmentStructure.h"
#include "Materials/MaterialInterface.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include <NiagaraFunctionLibrary.h>
#include "Kismet/GameplayStatics.h"

// Sets default values
ATDS_EnvironmentStructure::ATDS_EnvironmentStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATDS_EnvironmentStructure::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDS_EnvironmentStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EPhysicalSurface ATDS_EnvironmentStructure::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;

	UStaticMeshComponent* myMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (myMesh)
	{
		UMaterialInterface* myMaterial =  myMesh->GetMaterial(0);
		if (myMaterial)
		{
			Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
		}
	}

	return Result;
}

TArray<UTDS_StateEffect*> ATDS_EnvironmentStructure::GetAllCurrentEffects()
{
	return Effects;
}

void ATDS_EnvironmentStructure::AddEffect_Implementation(UTDS_StateEffect* newEffect)
{
	Effects.Add(newEffect);

	if (!newEffect->bIsAutoDestroyNiagaraEffect)
	{
		SwitchEffect(newEffect, true);
		EffectAdd = newEffect;
	}
	else
	{
		if (newEffect->bIsAutoDestroyNiagaraEffect)
			ExecuteEffectAdded_OnServer(newEffect->NiagaraEffect);
	}
}

void ATDS_EnvironmentStructure::RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);

	if (!RemoveEffect->bIsAutoDestroyNiagaraEffect)
	{
		SwitchEffect(RemoveEffect, false);
		EffectRemove = RemoveEffect;
	}
}

void ATDS_EnvironmentStructure::EffectAdd_OnRep()
{
	if (EffectAdd)
		SwitchEffect(EffectAdd, true);
}

void ATDS_EnvironmentStructure::EffectRemove_OnRep()
{
	if (EffectRemove)
	{
		SwitchEffect(EffectRemove, false);
	}
}

void ATDS_EnvironmentStructure::ExecuteEffectAdded_OnServer_Implementation(UNiagaraSystem* ExecuteFX)
{
	ExecuteEffectAdded_Multicast(ExecuteFX);
}

void ATDS_EnvironmentStructure::ExecuteEffectAdded_Multicast_Implementation(UNiagaraSystem* ExecuteFX)
{
	UTypes::ExecuteEffectAdded(ExecuteFX, this, OffsetEffect, NAME_None);
}

void ATDS_EnvironmentStructure::SwitchEffect(UTDS_StateEffect* Effect, bool bIsAdd)
{
	if (bIsAdd)
	{
		if (Effect && Effect->NiagaraEffect)
		{
			FName NameBonToAttached = NAME_None;
			FVector Loc = OffsetEffect;
			USceneComponent* mySceneComp = GetRootComponent();
			if (mySceneComp)
			{
				UNiagaraComponent* newNiagaraEmmiter = UNiagaraFunctionLibrary::SpawnSystemAttached(Effect->NiagaraEffect, mySceneComp, NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
				NiagaraSystemEffects.Add(newNiagaraEmmiter);
			}
		}
	}
	else
	{
		if (Effect && Effect->NiagaraEffect)
		{
			int32 i = 0;
			bool bIsFind = false;
			if (NiagaraSystemEffects.Num() > 0)
			{
				while (i < NiagaraSystemEffects.Num() && !bIsFind)
				{
					if (NiagaraSystemEffects[i]->GetAsset() && Effect->NiagaraEffect && Effect->NiagaraEffect == NiagaraSystemEffects[i]->GetAsset())
					{
						bIsFind = true;
						NiagaraSystemEffects[i]->Deactivate();
						NiagaraSystemEffects[i]->DestroyComponent();
						NiagaraSystemEffects.RemoveAt(i);
					}
					i++;
				}
			}

		}
	}
}

bool ATDS_EnvironmentStructure::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlag)
{
	bool Wrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlag);

	for (int32 i = 0; i < Effects.Num(); i++)
	{
		if (Effects[i])
		{
			Wrote |= Channel->ReplicateSubobject(Effects[i], *Bunch, *RepFlag);
		}
	}
	return Wrote;
}

void ATDS_EnvironmentStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDS_EnvironmentStructure, Effects);
	DOREPLIFETIME(ATDS_EnvironmentStructure, EffectAdd);
	DOREPLIFETIME(ATDS_EnvironmentStructure, EffectRemove);
}