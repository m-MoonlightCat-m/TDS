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
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"


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

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MovementTick(DeltaSeconds);

	if (bIsSprint == true && Stamina > 0.0f)
	{
		DecreasStamina();
	}
	else if (bIsSprint == false && Stamina <= 100.0f)
	{
		IncreasStamina();
	}
}



void ATDSCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);

	InputComponent->BindAxis("MoveForward", this, &ATDSCharacter::InputAxisX);
	InputComponent->BindAxis("MoveRight", this, &ATDSCharacter::InputAxisY);

	InputComponent->BindAction("MovementModeChangeSprint", IE_Pressed, this, &ATDSCharacter::Sprint);
	InputComponent->BindAction("MovementModeChangeSprint", IE_Pressed, this, &ATDSCharacter::StopSprint);
}

void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATDSCharacter::InputAxisY(float value)
{
	AxisY = value;
}

void ATDSCharacter::InputAxisX(float value)
{
	AxisX = value;
}

void ATDSCharacter::MovementTick(float DeltaTime)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

	APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (myController)
	{
		FHitResult ResultHit;
		myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);

		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
	}
}

void ATDSCharacter::CharacterUpdate()
{
	float ResSpeed = 600.0f;

	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeedNormal;
		StopSprint();
		break;
	case EMovementState::AimWalk_State:
		ResSpeed = MovementInfo.AimSpeedWalk;
		StopSprint();
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeedNormal;
		StopSprint();
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunSpeedNormal;
		StopSprint();
		break;
	case EMovementState::SptintRun_State:
		ResSpeed = MovementInfo.SprintRunSpeed;
		Sprint();
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
		if (SprintRunEnable)
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
}

void ATDSCharacter::Sprint()
{
	bIsSprint = true;
	DecreasStamina();
}

void ATDSCharacter::StopSprint()
{
	bIsSprint = false;
	IncreasStamina();
}

void ATDSCharacter::DecreasStamina()
{
	if (bIsSprint == true && Stamina > 0.0f)
	{
		Stamina = Stamina - MinusStamina;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Stamina Decreas %f"), Stamina));
	}
	else if (Stamina == 0.0f)
	{
		StopSprint();
	}
}

void ATDSCharacter::IncreasStamina()
{
	if (bIsSprint == false && Stamina < 100.0f)
	{
		Stamina = Stamina + PlusStamina;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Stamina Increas %f"), Stamina));
	}
}
