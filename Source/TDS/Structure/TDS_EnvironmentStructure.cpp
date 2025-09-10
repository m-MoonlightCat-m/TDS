// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_EnvironmentStructure.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ATDS_EnvironmentStructure::ATDS_EnvironmentStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void ATDS_EnvironmentStructure::AddEffect(UTDS_StateEffect* newEffect)
{
	Effects.Add(newEffect);
}

void ATDS_EnvironmentStructure::RemoveEffect(UTDS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);
}
