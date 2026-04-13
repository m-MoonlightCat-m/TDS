// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AProjectileDefault::AProjectileDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));

	BulletCollisionSphere->SetSphereRadius(16.f);

	BulletCollisionSphere->bReturnMaterialOnMove = true;

	BulletCollisionSphere->SetCanEverAffectNavigation(false);

	RootComponent = BulletCollisionSphere;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projectile Mesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);

	BulletFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Bullet FX"));
	BulletFX->SetupAttachment(RootComponent);

	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Bullet ProjectileMovemet"));
	BulletProjectileMovement->UpdatedComponent = RootComponent;
	BulletProjectileMovement->InitialSpeed = 1.f;
	BulletProjectileMovement->MaxSpeed = 0.f;

	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AProjectileDefault::BeginPlay()
{
	Super::BeginPlay();

	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereEndOverlap);
}

// Called every frame
void AProjectileDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileDefault::InitProjectile(FProjectileInfo InitParam)
{
	BulletProjectileMovement->InitialSpeed = InitParam.ProjectileInitSpeed;
	BulletProjectileMovement->MaxSpeed = InitParam.ProjectileMaxSpeed;
	this->SetLifeSpan(InitParam.ProjectileLifeTime);

	if (InitParam.ProjectileStaticMesh)
	{
		InitVisualMeshProjectile_Multicast(InitParam.ProjectileStaticMesh, InitParam.ProjectileStaticMeshOffset);
	}
	else
		BulletMesh->DestroyComponent();

	if (BulletFX)
	{
		if (InitParam.ProjectileTrailFX)
		{
			InitVisualTrailProjectile_Multicast(InitParam.ProjectileTrailFX, InitParam.ProjectileTrailFXOffset);
		}
		else
		{
			BulletFX->DestroyComponent();
			UE_LOG(LogTemp, Warning, TEXT("ProjectileTrailFX is not set."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BulletFX is nullptr! Cannot set transform."));
	}

	ProjectileSetting = InitParam;
}

void AProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && Hit.PhysMaterial.IsValid())
	{
		EPhysicalSurface mySurfacetype = UGameplayStatics::GetSurfaceType(Hit);

		if (ProjectileSetting.HitDecals.Contains(mySurfacetype))
		{
			UMaterialInterface* myMaterial = ProjectileSetting.HitDecals[mySurfacetype];

			if (myMaterial && OtherComp)
			{
				SpawnHitDecal_Multicast(myMaterial, OtherComp, Hit);
			}
		}
		if (ProjectileSetting.HitFXs.Contains(mySurfacetype))
		{
			UNiagaraSystem* myNiagara = ProjectileSetting.HitFXs[mySurfacetype];
			if (myNiagara)
			{
				SpawnHitFX_Multicast(myNiagara, Hit);
			}
		}

		if (ProjectileSetting.HitSound)
		{
			SpawnHitSound_Multicast(ProjectileSetting.HitSound, Hit);
		}

		
		UTypes::AddEffectBySurfaceType(Hit.GetActor(), Hit.BoneName, ProjectileSetting.Effect, mySurfacetype);
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileSetting.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);
	UAISense_Damage::ReportDamageEvent(GetWorld(), Hit.GetActor(), GetInstigator(), ProjectileSetting.ProjectileDamage, Hit.Location, Hit.Location);
	
	ImpactProjectile();
}

void AProjectileDefault::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AProjectileDefault::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AProjectileDefault::ImpactProjectile()
{
	this->Destroy();
}

void AProjectileDefault::InitVisualMeshProjectile_Multicast_Implementation(UStaticMesh* NewMesh, FTransform Meshrelative)
{
	BulletMesh->SetStaticMesh(NewMesh);
	BulletMesh->SetRelativeTransform(Meshrelative);
}

void AProjectileDefault::InitVisualTrailProjectile_Multicast_Implementation(UNiagaraSystem* NewFX, FTransform TemplateRelative)
{
	BulletFX->SetAsset(NewFX);
	BulletFX->SetRelativeTransform(TemplateRelative);
}


void AProjectileDefault::SpawnHitDecal_Multicast_Implementation(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComp, FHitResult HitResult)
{
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), OtherComp, NAME_None, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
}

void AProjectileDefault::SpawnHitFX_Multicast_Implementation(UNiagaraSystem* FXTemplate, FHitResult HitResult)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FXTemplate, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), FVector(1.0f), true);
}

void AProjectileDefault::SpawnHitSound_Multicast_Implementation(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
}
