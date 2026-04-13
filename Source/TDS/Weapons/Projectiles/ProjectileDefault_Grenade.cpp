// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault_Grenade.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


int32 DebugExplodeShow = 1;
FAutoConsoleVariableRef CVARExplodeShow(TEXT("TDS.DebugExplode"), DebugExplodeShow, TEXT("Draw Debug for Explode"), ECVF_Cheat);

void AProjectileDefault_Grenade::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileDefault_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimerExplose(DeltaTime);
}

void AProjectileDefault_Grenade::TimerExplose(float DeltaTime)
{
	if (TimerEnable)
	{
		if (TimerToExplose > TimeToExplose)
		{
			Explose_OnServer();
		}
		else
		{
			TimerToExplose += DeltaTime;
		}
	}
}

void AProjectileDefault_Grenade::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!TimerEnable)
	{
		Explose_OnServer();
	}
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileDefault_Grenade::ImpactProjectile()
{
	TimerEnable = true;
}


void AProjectileDefault_Grenade::GrenadeFX_Multicast_Implementation(UNiagaraSystem* FXFire, USoundBase* SoundFire)
{
	if (FXFire)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FXFire, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (SoundFire)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundFire, GetActorLocation());
	}
}

void AProjectileDefault_Grenade::Explose_OnServer_Implementation()
{
	if (DebugExplodeShow)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMinRadiusDamage, 12, FColor::Green, false, 12.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMaxRadiusDamage, 12, FColor::Red, false, 12.0f);
	}

	TimerEnable = false;

	GrenadeFX_Multicast(ProjectileSetting.ExplodeFX, ProjectileSetting.ExplodeSound);

	TArray<AActor*> IgnoredActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ProjectileSetting.ExplodeMaxDamage,
		ProjectileSetting.ExplodeMaxDamage * 0.2f,
		GetActorLocation(),
		ProjectileSetting.ProjectileMinRadiusDamage,
		ProjectileSetting.ProjectileMaxRadiusDamage,
		5,
		NULL, IgnoredActor, this, nullptr);

	this->Destroy();
}
