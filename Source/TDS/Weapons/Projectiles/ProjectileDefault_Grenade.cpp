// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault_Grenade.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


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
			Explose();
		}
		else
		{
			TimerToExplose += DeltaTime;
		}
	}
}

void AProjectileDefault_Grenade::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileDefault_Grenade::ImpactProjectile()
{
	TimerEnable = true;
}

void AProjectileDefault_Grenade::Explose()
{
	if (DebugExplodeShow)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMinRadiusDamage, 12, FColor::Green, false, 12.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMaxRadiusDamage, 12, FColor::Red, false, 12.0f);
	}

	TimerEnable = false;

	if (ProjectileSetting.ExplodeFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ProjectileSetting.ExplodeFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (ProjectileSetting.ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.ExplodeSound, GetActorLocation());
	}

	TArray<AActor*> IgnoredActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ProjectileSetting.ExplodeMaxDamage,
		ProjectileSetting.ExplodeMaxDamage*0.2f,
		GetActorLocation(),
		1000.0f,
		2000.0f,
		5,
		NULL, IgnoredActor, this, nullptr);

	this->Destroy();
}
