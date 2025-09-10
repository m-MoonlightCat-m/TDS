// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_IntrfcGameActor.h"

// Add default functionality here for any ITDS_IntrfcGameActor functions that are not pure virtual.

EPhysicalSurface ITDS_IntrfcGameActor::GetSurfaceType()
{
	return EPhysicalSurface::SurfaceType_Default;
}

TArray<UTDS_StateEffect*> ITDS_IntrfcGameActor::GetAllCurrentEffects()
{
	return TArray<UTDS_StateEffect*>();
}

void ITDS_IntrfcGameActor::AddEffect(UTDS_StateEffect* newEffect)
{

}

void ITDS_IntrfcGameActor::RemoveEffect(UTDS_StateEffect* RemoveEffect)
{

}
