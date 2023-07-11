// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComps/XFootStepsComponent.h"

#include "ActionShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMateials/XPhysicalMaterial.h"

static TAutoConsoleVariable<int32> CVarShowFootSteps(TEXT("ShowDebugFootSteps"), 0, TEXT("Draws debug info about footsteps") TEXT(" 0: off/n") TEXT(" 1:on/n"), ECVF_Cheat);

// Sets default values for this component's properties
UXFootStepsComponent::UXFootStepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UXFootStepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UXFootStepsComponent::HandleFootStep(EFoot Foot)
{
	if (AActionShooterCharacter* Character = Cast<AActionShooterCharacter>(GetOwner()))
	{

		const int32 DebugShowFootSteps = CVarShowFootSteps.GetValueOnAnyThread();
		
		if (USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector* 20;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -50.f, ECC_WorldStatic, QueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					if (HitResult.PhysMaterial.Get())
					{
						UXPhysicalMaterial* PhysicalMaterial = Cast<UXPhysicalMaterial>(HitResult.PhysMaterial.Get());
						if (PhysicalMaterial)
						{
							UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootStepSound, Location, 1.f);
							
						}
						if (DebugShowFootSteps > 0)
						{
							DrawDebugString(GetWorld(), Location, GetNameSafe(PhysicalMaterial), nullptr, FColor::White, 4.f);
						}
					}

					if (DebugShowFootSteps > 0)
					{
						DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
					}
					
				}
				else
				{
					if (DebugShowFootSteps > 0)
					{
						DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0,1);
					}
				}
			}
			else
			{
				if (DebugShowFootSteps > 0)
				{
					DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0,1);
					DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
				}
			}
		}
	}
}



