// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_StateEffect.h"
#include "../Character//TDSHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "../Interface/TDS_IntrfcGameActor.h"
#include "NiagaraSystem.h"

bool UTDS_StateEffect::InitObject(AActor* Actor)
{
	myActor = Actor;

	ITDS_IntrfcGameActor* myInterface = Cast<ITDS_IntrfcGameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}

	UE_LOG(LogTemp, Warning, TEXT("UTDS_StateEffect::InitObject"));

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
			myHelthComp->ChangeHealthValue(Power);
		}
	}

	DestroyObject();
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