// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_StateEffect.h"
#include "../Character//TDSHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "../Interface/TDS_IntrfcGameActor.h"
#include "NiagaraSystem.h"
#include "../Character/TDSCharacter.h"
#include "Engine/World.h"

bool UTDS_StateEffect::InitObject(AActor* Actor)
{
	myActor = Actor;

	ITDS_IntrfcGameActor* myInterface = Cast<ITDS_IntrfcGameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}

	return true;
}

void UTDS_StateEffect::DestroyObject()
{
	ITDS_IntrfcGameActor* myInterface = Cast<ITDS_IntrfcGameActor>(myActor);
	if (myInterface)
	{
		myInterface->RemoveEffect(this);
	}

	myActor = nullptr;

	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
}

bool UTDS_StateEffect_ExecuteOnce::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	if (NiagaraHealthEffect)
	{
		FName NameBonToAttached;
		FVector Loc = FVector(0);

		NiagaraHealthEmmiter = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraHealthEffect, myActor->GetRootComponent(), NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteOnceTimer, this, &UTDS_StateEffect_ExecuteOnce::DestroyObject, Timer, false);

	ExecuteOnce();

	return true;
}

void UTDS_StateEffect_ExecuteOnce::DestroyObject()
{

	NiagaraHealthEmmiter->DestroyComponent();
	NiagaraHealthEmmiter = nullptr;

	Super::DestroyObject();
}

void UTDS_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if (myActor)
	{
		UTDSHealthComponent* myHelthComp =  Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

		if (myHelthComp)
		{
			myHelthComp->ChangeHealthValue(Power);
		}
	}
}

bool UTDS_StateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UTDS_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTDS_StateEffect_ExecuteTimer::Execute, RateTime, true);

	if (NiagaraEffect)
	{
		FName NameBonToAttached;
		FVector Loc = FVector(0);

		NiagaraEmmiter = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffect, myActor->GetRootComponent(), NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}

	return true;
}

void UTDS_StateEffect_ExecuteTimer::DestroyObject()
{
	NiagaraEmmiter->DestroyComponent();
	NiagaraEmmiter = nullptr;

	Super::DestroyObject();
}

void UTDS_StateEffect_ExecuteTimer::Execute()
{
	if (myActor)
	{
		UTDSHealthComponent* myHelthComp = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

		if (myHelthComp)
		{
			myHelthComp->ChangeHealthValue(Power);
		}
	}
}

bool UTDS_StateEffect_HealthBoost::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_HealthBoostTimer, this, &UTDS_StateEffect_HealthBoost::DestroyObject, Timer, false);

	if (NiagaraEffectHealthBoost)
	{
		FName NameBonToAttached;
		FVector Loc = FVector(0);

		NiagaraEmmiterHealthBoost = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffectHealthBoost, myActor->GetRootComponent(), NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}

	Boosted();

	return true;
}

void UTDS_StateEffect_HealthBoost::DestroyObject()
{
	UTDSHealthComponent* HealthComp = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

	if (HealthComp)
	{
		HealthComp->SetMaxHealth(100.0f);

		if (HealthComp->GetCurrentHealth() > HealthComp->GetMaxHealth())
			HealthComp->SetCurrentHealth(100.0f);
	}

	NiagaraEmmiterHealthBoost->DestroyComponent();
	NiagaraEmmiterHealthBoost = nullptr;

	Super::DestroyObject();
}

void UTDS_StateEffect_HealthBoost::Boosted()
{
	UTDSHealthComponent* HealthComp = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

	if (HealthComp)
	{
		float OriginalHealth = HealthComp->GetMaxHealth();
		float NewMaxHealth = OriginalHealth + Power;
		HealthComp->SetMaxHealth(NewMaxHealth);

		if (HealthComp->GetCurrentHealth() == OriginalHealth)
		{
			HealthComp->SetCurrentHealth(NewMaxHealth);
		}
	}
}

bool UTDS_StateEffect_Immunity::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	if (myActor)
	{
		UTDSHealthComponent* HealthComp = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

		if (HealthComp)
		{
			HealthComp->bIsImmunToDamage = true;
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ImmunityTimer, this, &UTDS_StateEffect_Immunity::EndImmunity, TimerStartImmunity, false);
	
	if (NiagaraEffectStartImmunity)
	{
		FName NameBonToAttached;
		FVector Loc = FVector(0);

		NiagaraEmmiterImmunity = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffectStartImmunity, myActor->GetRootComponent(), NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}

	return true;
}

void UTDS_StateEffect_Immunity::DestroyObject()
{
	NiagaraEmmiterImmunity->DestroyComponent();
	NiagaraEmmiterImmunity = nullptr;

	Super::DestroyObject();
}

void UTDS_StateEffect_Immunity::EndImmunity()
{
	if (myActor)
	{
		UTDSHealthComponent* HealthComp = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

		if (HealthComp)
		{
			HealthComp->bIsImmunToDamage = false;
		}
	}

	NiagaraEmmiterImmunity->DestroyComponent();
	NiagaraEmmiterImmunity = nullptr;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EndImmunityTimer, this, &UTDS_StateEffect_Immunity::DestroyObject, TimerEndImmunity, false);


	if (NiagaraEffectEndImmunity)
	{
		FName NameBonToAttached;
		FVector Loc = FVector(0);

		NiagaraEmmiterImmunity = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffectEndImmunity, myActor->GetRootComponent(), NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}
}

bool UTDS_StateEffect_Stun::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	if (NiagaraEffectStun)
	{
		FName NameBonToAttached;
		FVector Loc = FVector(0);

		NiagaraEmmiterStun = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffectStun, myActor->GetRootComponent(), NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}

	APawn* myPawn = Cast<APawn>(Actor);
	if (myPawn && myPawn->GetController())
	{
		Controller = myPawn->GetController();
		myPawn->UnPossessed();
		
		if (ATDSCharacter* Character = Cast<ATDSCharacter>(myActor))
		{
			Character->bIsStuned = true;
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_StunTimer, this, &UTDS_StateEffect_Stun::EndStun, TimerStartStun, false);

	return true;
}

void UTDS_StateEffect_Stun::DestroyObject()
{
	NiagaraEmmiterStun->DestroyComponent();
	NiagaraEmmiterStun = nullptr;

	if (myActor && Controller)
	{
		APawn* myPawn = Cast<APawn>(myActor);
		if (myPawn)
			Controller->Possess(myPawn);

		if (ATDSCharacter* Character = Cast<ATDSCharacter>(myActor))
		{
			Character->bIsStuned = false;
		}
	}

	Super::DestroyObject();
}

void UTDS_StateEffect_Stun::EndStun()
{
	NiagaraEmmiterStun->DestroyComponent();
	NiagaraEmmiterStun = nullptr;

	if (NiagaraEffectStun)
	{
		FName NameBonToAttached;
		FVector Loc = FVector(0);

		NiagaraEmmiterStun = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffectEndStun, myActor->GetRootComponent(), NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectStunTimer, this, &UTDS_StateEffect_Stun::DestroyObject, TimerEffectStun, false);

}

bool UTDS_StateEffect_AuraDamage::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_AuraDamageTimer, this, &UTDS_StateEffect_AuraDamage::DestroyObject, TimerAuraDamage, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_DamageLightingTimer, this, &UTDS_StateEffect_AuraDamage::DealDamage, TimerDamageLighting, true);

	if (NiagaraSphereEffect)
	{
		FName NameBonToAttached;
		FVector Loc = FVector(0);
		NiagaraEmmiterSphere = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSphereEffect, myActor->GetRootComponent(), NameBonToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}

	if (NiagaraEmmiterSphere)
	{
		NiagaraEmmiterSphere->SetFloatParameter(FName("Radius"), AuraRadius);
	}

	return true;
}

void UTDS_StateEffect_AuraDamage::DestroyObject()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DamageLightingTimer);

	NiagaraEmmiterSphere->DestroyComponent();
	NiagaraEmmiterSphere = nullptr;

	NiagaraEmmiterLight->DestroyComponent();
	NiagaraEmmiterLight = nullptr;
		
	Super::DestroyObject();
}

void UTDS_StateEffect_AuraDamage::DealDamage()
{
	FVector Origin = myActor->GetActorLocation();

	TArray<AActor*> OverlappedActors;
	UKismetSystemLibrary::SphereOverlapActors(this, Origin, AuraRadius, ObjectTypes, nullptr, TArray<AActor*>(), OverlappedActors);
		
	for (AActor* Target : OverlappedActors)
	{
		if (!Target || Target == myActor) continue;

		bool bShouldDamage = false;

		if (myActor->Tags.Contains("Player"))
		{
			if (Target->Tags.Contains("Enemy"))
				bShouldDamage = true;
		}
		else
		{
			if (Target->Tags.Contains("Player"))
				bShouldDamage = true;
		}

		if (bShouldDamage)
		{
			UTDSHealthComponent* HealthComp = Cast<UTDSHealthComponent>(Target->GetComponentByClass(UTDSHealthComponent::StaticClass()));
			if (HealthComp)
			{
				SpawnLighting(Target);
				HealthComp->ChangeHealthValue(-DamagePerTick);
			}
		}
	}
}

void UTDS_StateEffect_AuraDamage::SpawnLighting(AActor* TargetActor)
{
	if (NiagaraLightingEffect)
	{
		FVector EndLocation = TargetActor->GetActorLocation();
		NiagaraEmmiterLight = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraLightingEffect, EndLocation);
	}
}

