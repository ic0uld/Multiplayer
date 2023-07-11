// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/XAnimInstance.h"

#include "ActionShooterCharacter.h"
#include "ActionShooterTypes.h"
#include "Animation/AnimationAsset.h"
#include "Animation/BlendSpace.h"
#include "Animation/AnimSequence.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UXAnimInstance::GetLocomotionBlendSpace() const
{
	if (AActionShooterCharacter* ActionShooterCharacter = Cast<AActionShooterCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionShooterCharacter->GetCharacterData();

		if (Data.CharacterAnimDataAsset)
		{  
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBS;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBS : nullptr;
	
}

UAnimSequenceBase* UXAnimInstance::GetIdleAnimationAsset() const
{
	if (AActionShooterCharacter* ActionShooterCharacter = Cast<AActionShooterCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionShooterCharacter->GetCharacterData();

		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset: nullptr;
}

UBlendSpace* UXAnimInstance::GetCrouchLocomotionBlendSpace() const
{

	if (AActionShooterCharacter* ActionShooterCharacter = Cast<AActionShooterCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionShooterCharacter->GetCharacterData();

		if (Data.CharacterAnimDataAsset)
		{  
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBS;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBS : nullptr;
	
}

UAnimSequenceBase* UXAnimInstance::GetCrouchIdleAnimationAsset() const
{

	if (AActionShooterCharacter* ActionShooterCharacter = Cast<AActionShooterCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionShooterCharacter->GetCharacterData();

		if (Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset: nullptr;
	
}
