// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_StateEffect.h"
#include "../Character//TDSHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "../Interface/TDS_IntrfcGameActor.h"
#include "NiagaraSystem.h"
#include "../Character/TDSCharacter.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

bool UTDS_StateEffect::InitObject(AActor* Actor, FName NameBonHit)
{
	myActor = Actor;
	NameBon = NameBonHit;

	ITDS_IntrfcGameActor* myInterface = Cast<ITDS_IntrfcGameActor>(myActor);
	if (myInterface)
	{
		myInterface->Execute_AddEffect(myActor, this);
	}

	return true;
}

void UTDS_StateEffect::DestroyObject()
{

	ITDS_IntrfcGameActor* myInterface = Cast<ITDS_IntrfcGameActor>(myActor);
	if (myInterface)
	{
		myInterface->Execute_RemoveEffect(myActor, this);
	}

	myActor = nullptr;

	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
}

bool UTDS_StateEffect_ExecuteOnce::InitObject(AActor* Actor, FName NameBonHit)
{
	Super::InitObject(Actor, NameBonHit);
	ExecuteOnce();
	return true;
}

void UTDS_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UTDS_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if (myActor)
	{
		UTDSHealthComponent* myHelthComp =  Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

		if (myHelthComp)
		{
			myHelthComp->ChangeHealthValue_OnServer(Power);
		}
	}
}

bool UTDS_StateEffect_ExecuteTimer::InitObject(AActor* Actor, FName NameBonHit)
{
	Super::InitObject(Actor, NameBonHit);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UTDS_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTDS_StateEffect_ExecuteTimer::Execute, RateTime, true);
	}

	return true;
}

void UTDS_StateEffect_ExecuteTimer::DestroyObject()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Super::DestroyObject();
}

void UTDS_StateEffect_ExecuteTimer::Execute()
{
	if (myActor)
	{
		UTDSHealthComponent* myHelthComp = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

		if (myHelthComp)
		{
			myHelthComp->ChangeHealthValue_OnServer(Power);
		}
	}
}

bool UTDS_StateEffect_HealthBoost::InitObject(AActor* Actor, FName NameBonHit)
{
	Super::InitObject(Actor, NameBonHit);

	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_HealthBoostTimer, this, &UTDS_StateEffect_HealthBoost::DestroyObject, Timer, false);

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

	if (GetWorld())
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

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

bool UTDS_StateEffect_Immunity::InitObject(AActor* Actor, FName NameBonHit)
{
	Super::InitObject(Actor, NameBonHit);

	if (myActor)
	{
		UTDSHealthComponent* HealthComp = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));

		if (HealthComp)
		{
			HealthComp->bIsImmunToDamage = true;
		}
	}
	
	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ImmunityTimer, this, &UTDS_StateEffect_Immunity::EndImmunity, TimerStartImmunity, false);

	return true;
}

void UTDS_StateEffect_Immunity::DestroyObject()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 

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

	if(GetWorld())
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_EndImmunityTimer, this, &UTDS_StateEffect_Immunity::DestroyObject, TimerEndImmunity, false);

}

bool UTDS_StateEffect_Stun::InitObject(AActor* Actor, FName NameBonHit)
{
	Super::InitObject(Actor, NameBonHit);

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

	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_StunTimer, this, &UTDS_StateEffect_Stun::EndStun, TimerStartStun, false);

	return true;
}

void UTDS_StateEffect_Stun::DestroyObject()
{
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

	if (GetWorld())
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Super::DestroyObject();
}

void UTDS_StateEffect_Stun::EndStun()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectStunTimer, this, &UTDS_StateEffect_Stun::DestroyObject, TimerEffectStun, false);

}

bool UTDS_StateEffect_AuraDamage::InitObject(AActor* Actor, FName NameBonHit)
{
	Super::InitObject(Actor, NameBonHit);

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AuraDamageTimer, this, &UTDS_StateEffect_AuraDamage::DestroyObject, TimerAuraDamage, false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_DamageLightingTimer, this, &UTDS_StateEffect_AuraDamage::DealDamage, TimerDamageLighting, true);
	}

	return true;
}

void UTDS_StateEffect_AuraDamage::DestroyObject()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		
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
				HealthComp->ChangeHealthValue_OnServer(-DamagePerTick);
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

void UTDS_StateEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTDS_StateEffect, NameBon);
}