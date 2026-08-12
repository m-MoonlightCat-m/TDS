// Fill out your copyright notice in the Description page of Project Settings.


#include "../Character/TDS_EnemyCharacter.h"
#include "Net/UnrealNetwork.h"
#include <NiagaraFunctionLibrary.h>
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATDS_EnemyCharacter::ATDS_EnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATDS_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDS_EnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATDS_EnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATDS_EnemyCharacter::AddEffect_Implementation(UTDS_StateEffect* newEffect)
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

void ATDS_EnemyCharacter::RemoveEffect_Implementation(UTDS_StateEffect * RemoveEffect)
{
	Effects.Remove(RemoveEffect);

	if (!RemoveEffect->bIsAutoDestroyNiagaraEffect)
	{
		SwitchEffect(RemoveEffect, false);
		EffectRemove = RemoveEffect;
	}
}

void ATDS_EnemyCharacter::EffectAdd_OnRep()
{
	if (EffectAdd)
		SwitchEffect(EffectAdd, true);
}

void ATDS_EnemyCharacter::EffectRemove_OnRep()
{
	if (EffectRemove)
	{
		SwitchEffect(EffectRemove, false);
	}
}

void ATDS_EnemyCharacter::ExecuteEffectAdded_OnServer_Implementation(UNiagaraSystem* ExecuteFX)
{
	ExecuteEffectAdded_Multicast(ExecuteFX);
}

void ATDS_EnemyCharacter::ExecuteEffectAdded_Multicast_Implementation(UNiagaraSystem* ExecuteFX)
{
	UTypes::ExecuteEffectAdded(ExecuteFX, this, FVector(0), FName("Spine_01"));
}

void ATDS_EnemyCharacter::SwitchEffect(UTDS_StateEffect * Effect, bool bIsAdd)
{
	if (bIsAdd)
	{
		if (Effect && Effect->NiagaraEffect)
		{
			FName NameBonToAttached = Effect->NameBon;
			FVector Loc = FVector(0);
			USkeletalMeshComponent* myMesh = GetMesh();
			if (myMesh)
			{
				UNiagaraComponent* newNiagaraEmmiter = UNiagaraFunctionLibrary::SpawnSystemAttached(Effect->NiagaraEffect, myMesh, NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
				NiagaraSystemEffects.Add(newNiagaraEmmiter);
			}
		}
	}
	else
	{
		if (Effect && Effect->NiagaraEffect)
		{
			FString TargetEffectName = Effect->NiagaraEffect->GetName();

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

bool ATDS_EnemyCharacter::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlag)
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

void ATDS_EnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDS_EnemyCharacter, Effects);
	DOREPLIFETIME(ATDS_EnemyCharacter, EffectAdd);
	DOREPLIFETIME(ATDS_EnemyCharacter, EffectRemove);
}

