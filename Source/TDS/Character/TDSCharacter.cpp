// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include <cmath> 
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Components/InputComponent.h"
#include "TDSInventoryComponent.h"
#include "../GameCatalog/TDSGameInstance.h"
#include "TDSCharacterHealthComponent.h"
#include "TDSStaminaComponent.h"
#include "../Weapons/Projectiles/ProjectileDefault.h"


ATDSCharacter::ATDSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InventoryComponent = CreateDefaultSubobject<UTDSInventoryComponent>(TEXT("InventoryComponent"));
	CharHealthComponent = CreateDefaultSubobject<UTDSCharacterHealthComponent>(TEXT("HealthComponent"));
	StaminaComponent = CreateDefaultSubobject<UTDSStaminaComponent>(TEXT("StaminaComponent"));

	if (CharHealthComponent)
		CharHealthComponent->OnDead.AddDynamic(this, &ATDSCharacter::CharDead);

	if (InventoryComponent)
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATDSCharacter::InitWeapon);


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentCursor)
	{
		APlayerController* myPC = Cast <APlayerController>(GetController());
		if (myPC)
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();

			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);
		}
	}

	MovementTick(DeltaSeconds);

	UpdateStamina(DeltaSeconds);
}



void ATDSCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);

	NewInputComponent->BindAxis("MoveForward", this, &ATDSCharacter::InputAxisX);
	NewInputComponent->BindAxis("MoveRight", this, &ATDSCharacter::InputAxisY);

	NewInputComponent->BindAction("MovementModeChangeSprint", IE_Pressed, this, &ATDSCharacter::StartSprint);
	NewInputComponent->BindAction("MovementModeChangeSprint", IE_Released, this, &ATDSCharacter::StopSprint);

	NewInputComponent->BindAction("FireEvent", IE_Pressed, this, &ATDSCharacter::InputAttackPressed);
	NewInputComponent->BindAction("FireEvent", IE_Released, this, &ATDSCharacter::InputAttackReleasd);
	NewInputComponent->BindAction("ReloadEvent", IE_Released, this, &ATDSCharacter::TryReloadWeapon);

	NewInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATDSCharacter::TrySwitchNextWeapon);
	NewInputComponent->BindAction(TEXT("SwitchPreviosWeapon"), EInputEvent::IE_Pressed, this, &ATDSCharacter::TrySwitchPreviosWeapon);

	NewInputComponent->BindAction(TEXT("AbilityAction"), EInputEvent::IE_Pressed, this, &ATDSCharacter::TryAbilityEnabled);
	NewInputComponent->BindAction(TEXT("AbilityAction2"), EInputEvent::IE_Pressed, this, &ATDSCharacter::TryHealthBoostEnabled);
	NewInputComponent->BindAction(TEXT("AbilityAction3"), EInputEvent::IE_Pressed, this, &ATDSCharacter::TryImmunityEnabled);
	NewInputComponent->BindAction(TEXT("AbilityAction4"), EInputEvent::IE_Pressed, this, &ATDSCharacter::TryStunEnabled);
	NewInputComponent->BindAction(TEXT("AbilityAction5"), EInputEvent::IE_Pressed, this, &ATDSCharacter::TryAuraDamageEnabled);

	NewInputComponent->BindAction(TEXT("DropCurrentWeapon"), EInputEvent::IE_Pressed, this, &ATDSCharacter::DropCurrentWeapon);


	TArray <FKey> HotKeys;
	HotKeys.Add(EKeys::One);
	HotKeys.Add(EKeys::Two);
	HotKeys.Add(EKeys::Three);
	HotKeys.Add(EKeys::Four);
	HotKeys.Add(EKeys::Five);
	HotKeys.Add(EKeys::Six);
	HotKeys.Add(EKeys::Seven);
	HotKeys.Add(EKeys::Eight);
	HotKeys.Add(EKeys::Nine);
	HotKeys.Add(EKeys::Zero);

	NewInputComponent->BindKey(HotKeys[1], IE_Pressed, this, &ATDSCharacter::TKeyPressed<1>);
	NewInputComponent->BindKey(HotKeys[2], IE_Pressed, this, &ATDSCharacter::TKeyPressed<2>);
	NewInputComponent->BindKey(HotKeys[3], IE_Pressed, this, &ATDSCharacter::TKeyPressed<3>);
	NewInputComponent->BindKey(HotKeys[4], IE_Pressed, this, &ATDSCharacter::TKeyPressed<4>);
	NewInputComponent->BindKey(HotKeys[5], IE_Pressed, this, &ATDSCharacter::TKeyPressed<5>);
	NewInputComponent->BindKey(HotKeys[6], IE_Pressed, this, &ATDSCharacter::TKeyPressed<6>);
	NewInputComponent->BindKey(HotKeys[7], IE_Pressed, this, &ATDSCharacter::TKeyPressed<7>);
	NewInputComponent->BindKey(HotKeys[8], IE_Pressed, this, &ATDSCharacter::TKeyPressed<8>);
	NewInputComponent->BindKey(HotKeys[9], IE_Pressed, this, &ATDSCharacter::TKeyPressed<9>);
	NewInputComponent->BindKey(HotKeys[0], IE_Pressed, this, &ATDSCharacter::TKeyPressed<0>);
}

void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
}

void ATDSCharacter::InputAxisY(float value)
{
	AxisY = value;
}

void ATDSCharacter::InputAxisX(float value)
{
	if (bIsSprint)
	{
		FVector ForwardVector = GetActorForwardVector();
		AddMovementInput(ForwardVector, value);
	}
	else
	{
		AddMovementInput(GetActorForwardVector(), value); 
	}
}

void ATDSCharacter::InputAttackPressed()
{
	if (bIsAlive)
		AttackCharEvent(true);
}

void ATDSCharacter::InputAttackReleasd()
{
	AttackCharEvent(false);
}

void ATDSCharacter::MovementTick(float DeltaTime)
{
	if (bIsAlive)
	{
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
		AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

		APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (myController)
		{
			FHitResult ResultHit;
			myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

			if (!bIsStuned)
			{
				float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
				SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
			}
			
			if (CurrentWeapon)
			{
				FVector Displacement = FVector(0);
				switch (MovementState)
				{
				case EMovementState::Aim_State:
					Displacement = FVector(0.0f, 0.0f, 160.0f);
					CurrentWeapon->ShouldReduceDispersion = true;
					break;
				case EMovementState::AimWalk_State:
					Displacement = FVector(0.0f, 0.0f, 160.0f);
					CurrentWeapon->ShouldReduceDispersion = true;
					break;
				case EMovementState::Walk_State:
					Displacement = FVector(0.0f, 0.0f, 120.0f);
					CurrentWeapon->ShouldReduceDispersion = false;
					break;
				case EMovementState::Run_State:
					Displacement = FVector(0.0f, 0.0f, 120.0f);
					CurrentWeapon->ShouldReduceDispersion = false;
					break;
				case EMovementState::SptintRun_State:
					break;
				default:
					break;
				}

				CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
			}
		}
	}		
}

void ATDSCharacter::AttackCharEvent(bool bIsFiring)
{
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->SetWeaponStateFire(bIsFiring);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ATDSCharacter::AttackCharEvent - CurrentWeapon"));
}

void ATDSCharacter::CharacterUpdate()
{
	float ResSpeed = 600.0f;

	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeedNormal;
		break;
	case EMovementState::AimWalk_State:
		ResSpeed = MovementInfo.AimSpeedWalk;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeedNormal;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunSpeedNormal;
		break;
	case EMovementState::SptintRun_State:
		ResSpeed = MovementInfo.SprintRunSpeed;
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATDSCharacter::ChangeMovementState()
{
	if (!WalkEnable && !SprintRunEnable && !AimEnable)
	{
		MovementState = EMovementState::Run_State;
	}
	else
	{
		if (SprintRunEnable && bIsSprint && bCanSprint)
		{
			WalkEnable = false;
			AimEnable = false;
			MovementState = EMovementState::SptintRun_State;
		}
		if (WalkEnable && !SprintRunEnable && AimEnable)
		{
			MovementState = EMovementState::AimWalk_State;
		}
		else
		{
			if (WalkEnable && !SprintRunEnable && !AimEnable)
			{
				MovementState = EMovementState::Walk_State;
			}
			else
			{
				if (!WalkEnable && !SprintRunEnable && AimEnable)
				{
					MovementState = EMovementState::Aim_State;
				}
			}
		}
	}

	CharacterUpdate();

	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon(MovementState);
	}
}

bool ATDSCharacter::GetIsAlive()
{
	return bIsAlive;
}

AWeaponDefault* ATDSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATDSCharacter::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;

	if (myGI)
	{
		if (myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		{
			if (myWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* myWeapon = Cast <AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					CurrentWeapon = myWeapon;

					myWeapon->IdWeaponName = IdWeaponName;
					myWeapon->WeaponSetting = myWeaponInfo;

					//!!!DEBUG!!!!
					myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
					myWeapon->UpdateStateWeapon(MovementState);

					myWeapon->AdditionalWeaponInfo = WeaponAdditionalInfo;
					CurrentIndexWeapon = NewCurrentIndexWeapon;

					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATDSCharacter::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATDSCharacter::WeaponReloadEnd);

					myWeapon->OnWeaponFireStart.AddDynamic(this, &ATDSCharacter::WeaponFireStart);

					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
						CurrentWeapon->InitReload();

					if(InventoryComponent)
						InventoryComponent->OnWeaponAmmoAviable.Broadcast(myWeapon->WeaponSetting.WeaponType);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATDSCharacter::InitWeapon - Weapon not found in table"));
		}
	}

	
}

void ATDSCharacter::TryReloadWeapon()
{
	if (bIsAlive && CurrentWeapon && !CurrentWeapon->WeaponReloading)
	{
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload())
		{
			CurrentWeapon->InitReload();
		}
	}
}

void ATDSCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void ATDSCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}

	WeaponReloadEnd_BP(bIsSuccess, AmmoTake);
}

void ATDSCharacter::WeaponFireStart(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);

	WeaponFireStart_BP(Anim);
}

bool ATDSCharacter::TrySwitchWeaponToIndexByKeyInput(int32 ToIndex)
{
	bool bIsSuccess = false;
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlot.IsValidIndex(ToIndex))
	{
		if (CurrentIndexWeapon != ToIndex && InventoryComponent)
		{
			int32 OldIndex = CurrentIndexWeapon;
			FAdditionalWeaponInfo OldInfo;

			if (CurrentWeapon)
			{
				OldInfo = CurrentWeapon->AdditionalWeaponInfo;
				if (CurrentWeapon->WeaponReloading)
					CurrentWeapon->CancelReload();
			}

			bIsSuccess = InventoryComponent->SwitchWeaponByIndex(ToIndex, OldIndex, OldInfo);
		}
	}

	return bIsSuccess;
}

void ATDSCharacter::DropCurrentWeapon()
{
	if (InventoryComponent)
	{
		FDropItem ItemInfo;
		InventoryComponent->DropWeaponByIndex(CurrentIndexWeapon, ItemInfo);
	}
}

void ATDSCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	//in BP
}

void ATDSCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess, int32 AmmoTake)
{
	//in BP
}

void ATDSCharacter::WeaponFireStart_BP_Implementation(UAnimMontage* Anim)
{
	//in BP
}

UDecalComponent* ATDSCharacter::GetCursorToWorld()
{
	return CurrentCursor;
}


void ATDSCharacter::StartSprint()
{
	if (StaminaComponent && StaminaComponent->GetCurrentStamina() > 0.0f)
	{
		bIsSprint = true;
		SprintRunEnable = true;
		ChangeMovementState();
	}
	else
	{
		bIsSprint = false;
		SprintRunEnable = false;
		ChangeMovementState();
	}
}

void ATDSCharacter::StopSprint()
{
	bIsSprint = false;
	SprintRunEnable = false;
	ChangeMovementState();
}

void ATDSCharacter::UpdateStamina(float DeltaTime)
{
	if (bIsSprint)
	{
		float NewStamina = StaminaComponent->GetCurrentStamina() - 1.0f;
		StaminaComponent->UpdateStaminaValue(NewStamina);

		if (NewStamina <= 0.0f)
		{
			bIsSprint = false;
			SprintRunEnable = false;
			WalkEnable = true;
			ChangeMovementState();
		}
	}
}

void ATDSCharacter::SetCanSprint(bool bNewCanSprint)
{
	bCanSprint = bNewCanSprint;

	if (!bCanSprint && bIsSprint)
	{
		bIsSprint = false;
		ChangeMovementState();
	}
}


void ATDSCharacter::TrySwitchNextWeapon()
{
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlot.Num() > 1)
	{
		//We have more then one weapon go switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				CurrentWeapon->CancelReload();
		}

		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndexByNextPreviosIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
			{
			}
		}
	}
}

void ATDSCharacter::TrySwitchPreviosWeapon()
{

	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlot.Num() > 1)
	{
		//We have more then one weapon go switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				CurrentWeapon->CancelReload();
		}

		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndexByNextPreviosIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
			{
			}
		}
	}
}

void ATDSCharacter::TryAbilityEnabled()
{
	if (AbilityEffect)
	{
		UTDS_StateEffect* NewEffect = NewObject<UTDS_StateEffect>(this, AbilityEffect);
		if (NewEffect)
			NewEffect->InitObject(this, NAME_None);
	}
}

void ATDSCharacter::TryHealthBoostEnabled()
{
	if (HealthBoostEffect)
	{
		UTDS_StateEffect* NewEffect = NewObject<UTDS_StateEffect>(this, HealthBoostEffect);
		if (NewEffect)
			NewEffect->InitObject(this, NAME_None);
	}
}

void ATDSCharacter::TryImmunityEnabled()
{
	if (ImmunityEffect)
	{
		UTDS_StateEffect* NewEffect = NewObject<UTDS_StateEffect>(this, ImmunityEffect);
		if (NewEffect)
			NewEffect->InitObject(this, NAME_None);
	}
}

void ATDSCharacter::TryStunEnabled()
{
	if (StunEffect)
	{
		UTDS_StateEffect* NewEffect = NewObject<UTDS_StateEffect>(this, StunEffect);
		if (NewEffect)
			NewEffect->InitObject(this, NAME_None);
	}
}

void ATDSCharacter::TryAuraDamageEnabled()
{
	if (AuraDamageEffect)
	{
		UTDS_StateEffect* NewEffect = NewObject<UTDS_StateEffect>(this, AuraDamageEffect);
		if (NewEffect)
			NewEffect->InitObject(this, NAME_None);
	}
}

EPhysicalSurface ATDSCharacter::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;

	if (CharHealthComponent)
	{
		if (CharHealthComponent->GetCurrentShield() <= 0)
		{
			if (GetMesh())
			{
				UMaterialInterface* myMaterial = GetMesh()->GetMaterial(0);
				if (myMaterial)
				{
					Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
				}
			}
		}
	}


	return Result;
}

TArray<UTDS_StateEffect*> ATDSCharacter::GetAllCurrentEffects()
{
	return Effects;
}

void ATDSCharacter::AddEffect(UTDS_StateEffect* newEffect)
{
	Effects.Add(newEffect);
}

void ATDSCharacter::RemoveEffect(UTDS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);
}

void ATDSCharacter::CharDead_BP_Implementation()
{
	//in BP
}

void ATDSCharacter::CharDead()
{
	float TimeAnim = 0.0f;
	int32 rnd = FMath::RandHelper(DeadsAnim.Num());

	if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh()->GetAnimInstance())
	{
		TimeAnim = DeadsAnim[rnd]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
	}

	bIsAlive = false;

	if (GetController())
	{
		GetController()->UnPossess();
	}

	//Timer Ragdoll
	GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &ATDSCharacter::EnableRagdoll, TimeAnim, false);
	
	GetCursorToWorld()->SetVisibility(false);

	AttackCharEvent(false);

	CharDead_BP();
}

void ATDSCharacter::EnableRagdoll()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

float ATDSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsAlive)
		CharHealthComponent->ChangeHealthValue(-DamageAmount);

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		AProjectileDefault* myProjectile = Cast<AProjectileDefault>(DamageCauser);
		if (myProjectile)
		{
			UTypes::AddEffectBySurfaceType(this, NAME_None, myProjectile->ProjectileSetting.Effect, GetSurfaceType());
		}
	}

	return ActualDamage;
}

