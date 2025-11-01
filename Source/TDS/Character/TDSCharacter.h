// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../GameCatalog/TDSPlayerController.h"
#include "Components/InputComponent.h"
#include "../FuncLibrary/TDSTypes.h"
#include "../GameCatalog/TDSGameInstance.h"
#include "../Weapons/WeaponDefault.h"
#include "../Character/TDSInventoryComponent.h"
#include "../Character/TDSCharacterHealthComponent.h"
#include "../Interface/TDS_IntrfcGameActor.h"
#include "../StateEffects/TDS_StateEffect.h"
#include "Engine/DamageEvents.h"
#include "TDSCharacter.generated.h"


UCLASS(Blueprintable)
class ATDSCharacter : public ACharacter, public ITDS_IntrfcGameActor
{
	GENERATED_BODY()

public:
	ATDSCharacter();

	FTimerHandle TimerHandle_RagDollTimer;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* NewInputComponent) override;
	virtual void BeginPlay() override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UTDSInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UTDSCharacterHealthComponent* CharHealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UTDSStaminaComponent* StaminaComponent;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Walk_State;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool SprintRunEnable = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool WalkEnable = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool AimEnable = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<UAnimMontage*> DeadsAnim;


	//Effect
	TArray<UTDS_StateEffect*> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UTDS_StateEffect> AbilityEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UTDS_StateEffect> HealthBoostEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UTDS_StateEffect> ImmunityEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UTDS_StateEffect> StunEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UTDS_StateEffect> AuraDamageEffect;

	//Inputs
	UFUNCTION()
	void InputAxisY(float value);
	UFUNCTION()
	void InputAxisX(float value);
	UFUNCTION()
	void InputAttackPressed();
	UFUNCTION()
	void InputAttackReleasd();

	float AxisX = 0.0f;
	float AxisY = 0.0f;

	bool bIsStuned = false;

	//Tick Func
	UFUNCTION()
	void MovementTick(float DeltaTime);

	//Func
	UFUNCTION()
	void AttackCharEvent(bool bIsFiring);

	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState();


	//Weapon
	AWeaponDefault* CurrentWeapon = nullptr;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeaponDefault* GetCurrentWeapon();
	UFUNCTION()
	void InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon);
	UFUNCTION(BlueprintCallable)
	void TryReloadWeapon();
	UFUNCTION()
	void WeaponReloadStart(UAnimMontage* Anim);
	UFUNCTION()
	void WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake);
	UFUNCTION()
	void WeaponFireStart(UAnimMontage* Anim);
	UFUNCTION()
	bool TrySwitchWeaponToIndexByKeyInput(int32 ToIndex);
	UFUNCTION()
	void DropCurrentWeapon();
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess, int32 AmmoTake);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponFireStart_BP(UAnimMontage* Anim);

	//Cursor
	UDecalComponent* CurrentCursor = nullptr;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UDecalComponent* GetCursorToWorld();

	//Sprint
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopSprint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	bool bIsSprint = false;


	void UpdateStamina(float DeltaTime);
	void SetCanSprint(bool bNewCanSprint);

	FVector LastDirection;
	bool bCanSprint = true;
	//EndSprint

	//Inventory Inputs
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CurrentIndexWeapon = 0;

	void TrySwitchNextWeapon();
	void TrySwitchPreviosWeapon();

	//Ability Inputs
	void TryAbilityEnabled();
	void TryHealthBoostEnabled();
	void TryImmunityEnabled();
	void TryStunEnabled();
	void TryAuraDamageEnabled();

	template <int32 Id>
	void TKeyPressed()
	{
		TrySwitchWeaponToIndexByKeyInput(Id);
	}

	//Interface
	EPhysicalSurface GetSurfaceType() override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UTDS_StateEffect*> GetAllCurrentEffects() override;
	void AddEffect(UTDS_StateEffect* newEffect) override;
	void RemoveEffect(UTDS_StateEffect* RemoveEffect) override;
	//EndInterface

	UFUNCTION(BlueprintCallable)
	void CharDead();
	void EnableRagdoll();
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void CharDead_BP();

};