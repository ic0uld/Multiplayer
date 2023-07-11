// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionShooterCharacter.h"
#include "Abilities/GameplayAbility.h"
#include "XGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONSHOOTER_API UXGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	
protected:

	UPROPERTY(EditDefaultsOnly, Category= "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectToRemoveOnEnd;

	UPROPERTY(EditDefaultsOnly, Category= "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectToJustApplyOnStart;
	
	UPROPERTY(EditDefaultsOnly, Category= "Effects")
	TArray<FActiveGameplayEffectHandle> RemoveOnEndEffectHandle;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActionShooterCharacter* GetActionShooterCharacterFromActorInfo() const;
};
