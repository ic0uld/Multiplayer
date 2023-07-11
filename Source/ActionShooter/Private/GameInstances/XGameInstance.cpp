// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstances/XGameInstance.h"
#include "AbilitySystemGlobals.h"

void UXGameInstance::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
