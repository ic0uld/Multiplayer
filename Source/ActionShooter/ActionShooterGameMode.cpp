// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionShooterGameMode.h"
#include "ActionShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AActionShooterGameMode::AActionShooterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
