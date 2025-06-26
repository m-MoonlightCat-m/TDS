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
#include "TDSCharacter.generated.h"


UCLASS(Blueprintable)
class ATDSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATDSCharacter();

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
	class UTDSCharacterHealthComponent* HealthComponent;

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

	AWeaponDefault* CurrentWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	FName InitWeaponName;

	UDecalComponent* CurrentCursor = nullptr;

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

	UFUNCTION()
	void MovementTick(float DeltaTime);
	UFUNCTION()
	void AttackCharEvent(bool bIsFiring);

	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState();

	UFUNCTION(BlueprintCallable)
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
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess, int32 AmmoTake);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponFireStart_BP(UAnimMontage* Anim);


	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopSprint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	bool bIsSprint = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaCoast = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float RegenerationStaminaRate = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina;

	void UpdateStamina(float DeltaTime);

	FVector LastDirection;
	bool bCanSprint;

	FTimerHandle StaminaRecoveryTimer;  
	bool bIsRecoveringStamina;

	void StartStaminaRecovery();
	void EndStaminaRecovery();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CurrentIndexWeapon = 0;

	void TrySwitchNextWeapon();
	void TrySwitchPreviosWeapon();
};
