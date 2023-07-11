// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotfiy/AnimNotify_Step.h"

#include "ActionShooterCharacter.h"
#include "ActorComps/XFootStepsComponent.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	check(MeshComp);

	AActionShooterCharacter* Character = MeshComp ? Cast<AActionShooterCharacter>(MeshComp->GetOwner()) : nullptr;
	if (Character)
	{
		if (UXFootStepsComponent* FootStepsComponent = Character->GetFootStepComponent())
		{
			FootStepsComponent->HandleFootStep(Foot);
		}
		
	}
}
