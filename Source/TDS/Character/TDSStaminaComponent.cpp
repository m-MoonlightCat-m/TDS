// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSStaminaComponent.h"
#include "TDSCharacter.h"

// Sets default values for this component's properties
UTDSStaminaComponent::UTDSStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTDSStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTDSStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UTDSStaminaComponent::GetCurrentStamina()
{
	return Stamina;
}

void UTDSStaminaComponent::SetCurrentStamina(float NewStamina)
{
	Stamina = NewStamina;
}

void UTDSStaminaComponent::UpdateStaminaValue(float StaminaValue)
{
	Stamina = StaminaValue;

	ATDSCharacter* Charecter = Cast<ATDSCharacter>(GetOwner());

	if (Stamina > 100.0f)
	{
		Stamina = 100.0f;
	}
	else
	{
		if (Stamina <= 0.0f)
		{
			Stamina = 0.0f;

			if (Charecter)
			{
				Charecter->SetCanSprint(false);
			}
		}
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDawnStaminaTimer, this, &UTDSStaminaComponent::CoolDawnStaminaEnd, CoolDawnStaminaRecoverTime, false);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_StaminaRecoveryRateTimer);
	}

	OnStaminaChange.Broadcast(Stamina);
}

void UTDSStaminaComponent::RecoveryStamina()
{
	ATDSCharacter* Charecter = Cast<ATDSCharacter>(GetOwner());
	float tmp = Stamina;
	tmp += StaminaRecoveryValue;

	if (tmp > 100.0f)
		Stamina = 100.0f;
	else
		Stamina = tmp;

	Charecter->SetCanSprint(true);

	OnStaminaChange.Broadcast(Stamina);
}

void UTDSStaminaComponent::CoolDawnStaminaEnd()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_StaminaRecoveryRateTimer, this, &UTDSStaminaComponent::RecoveryStamina, StaminaRecoveryRate, true);
}

