// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/XAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

void UXAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxMoveSpeedAttribute())
	{
		ACharacter* OwningCharacter = Cast<ACharacter>(GetOwningActor());
		UCharacterMovementComponent* CharacterMovementComponent = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;
		

		if (CharacterMovementComponent)
		{

			const float MaxSpeed = GetMaxMoveSpeed();
			
			CharacterMovementComponent->MaxWalkSpeed = MaxSpeed;
		}
	}
}

void UXAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UXAttributeSetBase, Health, OldHealth);
}

void UXAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UXAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UXAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{

	GAMEPLAYATTRIBUTE_REPNOTIFY(UXAttributeSetBase, Stamina, OldStamina);
	
}

void UXAttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{

	GAMEPLAYATTRIBUTE_REPNOTIFY(UXAttributeSetBase, MaxStamina, OldMaxStamina);
	
}

void UXAttributeSetBase::OnRep_MaxMoveSpeed(const FGameplayAttributeData& OldMaxMoveSpeed)
{

	GAMEPLAYATTRIBUTE_REPNOTIFY(UXAttributeSetBase, MaxMoveSpeed, OldMaxMoveSpeed);
	
}

void UXAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UXAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UXAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UXAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UXAttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UXAttributeSetBase, MaxMoveSpeed, COND_None, REPNOTIFY_Always);
}

