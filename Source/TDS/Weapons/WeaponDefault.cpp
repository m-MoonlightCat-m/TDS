// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDefault.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "../Character/TDSInventoryComponent.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sceletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponInit();
}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
	ClipDropTick(DeltaTime);
	ShellDropTick(DeltaTime);
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if (WeaponFiring && GetWeaponRound() > 0 && !WeaponReloading)
	{
		if (FireTimer < 0.f)
		{
			Fire();
		}
		else
			FireTimer -= DeltaTime;
	}
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			else
				CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;
		}

		if (CurrentDispersion < CurrentDispersionMin)
			CurrentDispersion = CurrentDispersionMin;
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
				CurrentDispersion = CurrentDispersionMax;
		}
	}

	if (ShowDebug)
		UE_LOG(LogTemp, Warning,TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
}

void AWeaponDefault::ClipDropTick(float DeltaTime)
{
	if (DropClipFlag)
	{
		if (DropClipTimer < 0.0f)
		{
			DropClipFlag = false;
			InitDropMesh(WeaponSetting.ClipDropMesh.DropMesh, WeaponSetting.ClipDropMesh.DropMeshOffset, WeaponSetting.ClipDropMesh.DropMeshImpulseDir, WeaponSetting.ClipDropMesh.DropMeshLifeTime, WeaponSetting.ClipDropMesh.ImpulseRandomDispersion, WeaponSetting.ClipDropMesh.PowerImpulse, WeaponSetting.ClipDropMesh.CustomMass);
		}
		else
			DropClipTimer -= DeltaTime;
	}
}

void AWeaponDefault::ShellDropTick(float DeltaTime)
{
	if (DropShellFlag)
	{
		if (DropShellTimer < 0.0f)
		{
			DropShellFlag = false;
			InitDropMesh(WeaponSetting.ShellBullets.DropMesh, WeaponSetting.ShellBullets.DropMeshOffset, WeaponSetting.ShellBullets.DropMeshImpulseDir, WeaponSetting.ShellBullets.DropMeshLifeTime, WeaponSetting.ShellBullets.ImpulseRandomDispersion, WeaponSetting.ShellBullets.PowerImpulse, WeaponSetting.ShellBullets.CustomMass);
		}
		else
			DropShellTimer -= DeltaTime;
	}
}

void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->GetSkeletalMeshAsset())
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}

	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}

	ReloadTime = WeaponSetting.ReloadTime;

	UpdateStateWeapon(EMovementState::Run_State);
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
		WeaponFiring = bIsFire;
	else
		WeaponFiring = false;
	FireTimer = 0.01f;
}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponDefault::Fire()
{
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFireAim;
	else
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFire;

	if (WeaponSetting.AnimWeaponInfo.AnimWeaponFire && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(WeaponSetting.AnimWeaponInfo.AnimWeaponFire);

	if (WeaponSetting.ShellBullets.DropMesh)
	{
		if (WeaponSetting.ShellBullets.DropMeshTime < 0.0f)
			InitDropMesh(WeaponSetting.ShellBullets.DropMesh, WeaponSetting.ShellBullets.DropMeshOffset,WeaponSetting.ShellBullets.DropMeshImpulseDir, WeaponSetting.ShellBullets.DropMeshLifeTime, WeaponSetting.ShellBullets.ImpulseRandomDispersion, WeaponSetting.ShellBullets.PowerImpulse, WeaponSetting.ShellBullets.CustomMass);
		else
		{
			DropShellFlag = true;
			DropShellTimer = WeaponSetting.ShellBullets.DropMeshTime;
		}
	}

	FireTimer = WeaponSetting.RateOfFire;
	AdditionalWeaponInfo.Round--;
	ChangeDispersionByShot();

	OnWeaponFireStart.Broadcast(AnimToPlay);


	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation->GetComponentLocation());

	int8 NumberProjectile = GetNumberProjectileByShoot();

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		FVector Endlocation;
		for (int8 i = 0; i < NumberProjectile; i++)
		{
			Endlocation = GetFireEndLocation();

			if (ProjectileInfo.Projectile)
			{
				FVector Dir = Endlocation - SpawnLocation;

				Dir.Normalize();

				FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
				SpawnRotation = myMatrix.Rotator();

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* myProjectile = Cast <AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myProjectile)
				{
					myProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
				}
			}
			else
			{
				FHitResult HitResult;
				TArray<AActor*> Actors;

				UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, Endlocation * WeaponSetting.DistanceTrace, ETraceTypeQuery::TraceTypeQuery4, false, Actors, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

				if (ShowDebug)
					DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + ShootLocation->GetForwardVector() * WeaponSetting.DistanceTrace, FColor::Black, false, 5.f, (uint8)'\000', 0.5f);

				if (HitResult.GetActor() && HitResult.PhysMaterial.IsValid())
				{
					EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(HitResult);

					if (WeaponSetting.ProjectileSetting.HitDecals.Contains(mySurfaceType))
					{
						UMaterialInterface* myMaterial = WeaponSetting.ProjectileSetting.HitDecals[mySurfaceType];

						if (myMaterial && HitResult.GetComponent())
							UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), HitResult.GetComponent(), NAME_None, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 5.0f);
					}

					if (WeaponSetting.ProjectileSetting.HitSound)
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSetting.ProjectileSetting.HitSound, HitResult.ImpactPoint);

					UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponSetting.ProjectileSetting.ProjectileDamage, GetInstigatorController(), this, NULL);
				}
			}
		}
	}

	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		if (CheckCanWeaponReload())
			InitReload();
	}
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	BlockFire = false;

	switch (NewMovementState)
	{
	case EMovementState::Aim_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::AimWalk_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Walk_StateDispersionReduction;
		break;
	case EMovementState::Run_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Run_StateDispersionReduction;
		break;
	case EMovementState::SptintRun_State:
		BlockFire = true;
		SetWeaponStateFire(false);
		break;
	default:
		break;
	}
}

void AWeaponDefault::ChangeDispersionByShot()
{
	CurrentDispersion = CurrentDispersion + CurrentDispersionRecoil;
}

float AWeaponDefault::GetCurrentDispersion() const
{
	return 0.0f;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);

	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal() * -20000.0f;
		if (ShowDebug)
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSetting.DistanceTrace, GetCurrentDispersion()* PI / 180.0f, GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
		if (ShowDebug)
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSetting.DistanceTrace, GetCurrentDispersion() * PI / 180.0f, GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}

	if (ShowDebug)
	{
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 5.f, (uint8)'\000', 0.5f);

		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 5.f, (uint8)'\000', 0.5f);

		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Black, false, 5.f, (uint8)'\000', 0.5f);

		DrawDebugSphere(GetWorld(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * SizeVectorToChangeShootDirectionLogic, 10.f, 8, FColor::Red, false, 4.0f);
	}

	return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShoot() const
{
	return WeaponSetting.NumberProjectileByShot;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return AdditionalWeaponInfo.Round;
}

void AWeaponDefault::InitReload()
{
	WeaponReloading = true;

	ReloadTimer = WeaponSetting.ReloadTime;

	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharReloadAim;
	else
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharReload;

	OnWeaponReloadStart.Broadcast(AnimToPlay);

	UAnimMontage* AnimWeaponToPlay = nullptr;
	if (WeaponAiming)
		AnimWeaponToPlay = WeaponSetting.AnimWeaponInfo.AnimWeaponReloadAim;
	else
		AnimWeaponToPlay = WeaponSetting.AnimWeaponInfo.AnimWeaponReload;

	if ((WeaponSetting.AnimWeaponInfo.AnimWeaponReload || WeaponSetting.AnimWeaponInfo.AnimWeaponReloadAim) && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(AnimWeaponToPlay);

	if (WeaponSetting.ClipDropMesh.DropMesh)
	{
		DropClipFlag = true;
		DropClipTimer = WeaponSetting.ClipDropMesh.DropMeshTime;
	}
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;

	int8 AviableAmmoFromInventory = GetAviableAmmoForReload();
	int8 AmmoNeedTakeFromInv;
	int8 NeedToReload = WeaponSetting.MaxRound - AdditionalWeaponInfo.Round;

	if (NeedToReload > AviableAmmoFromInventory)
	{
		AdditionalWeaponInfo.Round = AviableAmmoFromInventory;
		AmmoNeedTakeFromInv = AviableAmmoFromInventory;
	}
	else
	{
		AdditionalWeaponInfo.Round += NeedToReload;
		AmmoNeedTakeFromInv = NeedToReload;
	}

	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedTakeFromInv);
}

void AWeaponDefault::CancelReload()
{
	WeaponReloading = false;
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);

	OnWeaponReloadEnd.Broadcast(false, 0);
	DropClipFlag = false;
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool result = true;
	if (GetOwner())
	{
		UTDSInventoryComponent* MyInv = Cast<UTDSInventoryComponent>(GetOwner()->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		if (MyInv)
		{
			int8 AviableAmmoForWeapon;
			if (!MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				result = false;
			}
		}
	}

	return result;
}

int8 AWeaponDefault::GetAviableAmmoForReload()
{
	int8 AviableAmmoForWeapon = WeaponSetting.MaxRound;
	if (GetOwner())
	{
		UTDSInventoryComponent* MyInv = Cast<UTDSInventoryComponent>(GetOwner()->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		if (MyInv)
		{
			if (MyInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				AviableAmmoForWeapon = AviableAmmoForWeapon;
			}
		}
	}

	return AviableAmmoForWeapon;
}

void AWeaponDefault::InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float LifeTimeMesh, float ImpulseRandomDispertion, float PowerImpulse, float CustomMass)
{
	if (DropMesh)
	{
		FTransform Transform;

		FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() * Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;

		Transform.SetLocation(GetActorLocation() + LocalDir);
		Transform.SetScale3D(Offset.GetScale3D());

		Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());
		AStaticMeshActor* NewActor = nullptr;

		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Param.Owner = this;
		NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Param);

		if (NewActor && NewActor->GetStaticMeshComponent())
		{
			NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
			NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

			NewActor->SetActorTickEnabled(false);
			NewActor->InitialLifeSpan = LifeTimeMesh;

			NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
			NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
			NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);



			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);



			if (CustomMass > 0.0f)
				NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, CustomMass, true);

			if (!DropImpulseDirection.IsNearlyZero())
			{
				FVector FinalDir;
				LocalDir = LocalDir + (DropImpulseDirection * 1000.0f);

				if (!FMath::IsNearlyZero(ImpulseRandomDispertion))
					FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandomDispertion);
				FinalDir.GetSafeNormal(0.0001f);

				NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir * PowerImpulse);
			}
		}
	}
}

