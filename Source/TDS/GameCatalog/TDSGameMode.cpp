// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSGameMode.h"
#include "TDSPlayerController.h"
#include "TDS/Character/TDSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATDSGameMode::ATDSGameMode()
{
	PlayerControllerClass = ATDSPlayerController::StaticClass();

/*	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Character/BP_TopDownCharacter"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TDS/Blueprint/Character/BP_TopDownCharacter.BP_TopDownCharacter_C"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TDS/Blueprint/Core/Game/BP_PlayerController.BP_PlayerController_C"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}*/
}

void ATDSGameMode::PlayerCharacterDead()
{
}
