// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActorComps/XCharacterMovementComponent.h"
#include "ActorComps/XFootStepsComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "GAS/Attributes/XAttributeSetBase.h"
#include "GAS/XAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// AActionShooterCharacter

AActionShooterCharacter::AActionShooterCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UXCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	TurnRateGamepad = 50.f;

	//GAS

	AbilitySystemComponent = CreateDefaultSubobject<UXAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxMoveSpeedAttribute()).AddUObject(this, &AActionShooterCharacter::OnMaxMovementSpeedChanged);
	
	AttributeSetBase = CreateDefaultSubobject<UXAttributeSetBase>(TEXT("AttributeSetBase"));

	FootStepsComponent = CreateDefaultSubobject<UXFootStepsComponent>(TEXT("FootStepsComponent"));
}

void AActionShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

void AActionShooterCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (!CrouchStateEffect.Get()) return;
	{
		if (AbilitySystemComponent)
		{
			FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
			
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect, 1, EffectContextHandle);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (!ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to apply crouch effect! %s"), *GetNameSafe(CrouchStateEffect));
				}
			}
		}
	}
	
}

void AActionShooterCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (AbilitySystemComponent && CrouchStateEffect.Get())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect, AbilitySystemComponent);
	}
}

FCharacterData AActionShooterCharacter::GetCharacterData() const
{
	return CharacterData;
}

void AActionShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AActionShooterCharacter, CharacterData);
}

void AActionShooterCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(CharacterData);
}

UXFootStepsComponent* AActionShooterCharacter::GetFootStepComponent() const
{
	return FootStepsComponent;
}

void AActionShooterCharacter::OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void AActionShooterCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
	}
}


void AActionShooterCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AActionShooterCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplicaton)
{


	
}

bool AActionShooterCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		return ActiveGEHandle.WasSuccessfullyApplied();
	}
	
	return false;
}

UAbilitySystemComponent* AActionShooterCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AActionShooterCharacter::GiveAbility()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto DefaultAbility : CharacterData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AActionShooterCharacter::ApplyStartupEffect()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		
		for (auto CharacterEffect : CharacterData.Effects)
		{
			ApplyGameplayEffectToSelf(CharacterEffect, EffectContext);
		}
		
	}
}

void AActionShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
	GiveAbility();
	ApplyStartupEffect();
}

void AActionShooterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
}

void AActionShooterCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AActionShooterCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AActionShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveForwardInputAction)
		{
			EnhancedInputComponent->BindAction(MoveForwardInputAction, ETriggerEvent::Triggered, this, &AActionShooterCharacter::OnMoveForwardAction);
		}
		if (MoveSideInputAction)
		{
			EnhancedInputComponent->BindAction(MoveSideInputAction, ETriggerEvent::Triggered, this, &AActionShooterCharacter::OnMoveSideAction);
		}
		if (TurnInputAction)
		{
			EnhancedInputComponent->BindAction(TurnInputAction, ETriggerEvent::Triggered, this, &AActionShooterCharacter::OnTurnAction);
		}
		if (LookUpInputAction)
		{
			EnhancedInputComponent->BindAction(LookUpInputAction, ETriggerEvent::Triggered, this, &AActionShooterCharacter::OnLookUpAction);
		}
		if (JumpInputAction)
		{
			EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &AActionShooterCharacter::OnJumpActionStarted);
			EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &AActionShooterCharacter::OnJumpActionEnded);
		}
		if (CrouchInputAction)
		{
			EnhancedInputComponent->BindAction(CrouchInputAction, ETriggerEvent::Started, this, &AActionShooterCharacter::OnCrouchActionStarted);
			EnhancedInputComponent->BindAction(CrouchInputAction, ETriggerEvent::Completed, this, &AActionShooterCharacter::OnCrouchActionEnded);
		}
		if (SprintInputAction)
		{
			EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Started, this, &AActionShooterCharacter::OnSprintActionStarted);
			EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &AActionShooterCharacter::OnSprintActionEnded);
		}
		

	}

}

void AActionShooterCharacter::OnMoveForwardAction(const FInputActionValue& Value)
{
	const float Magnitude = Value.GetMagnitude();
	if ( (Controller != nullptr) && (Magnitude != 0.0f) )
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// add movement 
		AddMovementInput(ForwardDirection, Magnitude);
	}
}

void AActionShooterCharacter::OnMoveSideAction(const FInputActionValue& Value)
{
		const float Magnitude = Value.GetMagnitude();
		if ( (Controller != nullptr) && (Magnitude != 0.0f) )
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector SideDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
			// add movement 
			AddMovementInput(SideDirection, Magnitude);
		}
}

void AActionShooterCharacter::OnTurnAction(const FInputActionValue& Value)
{
			
AddControllerYawInput(Value.GetMagnitude() * TurnRateGamepad *  GetWorld()->GetDeltaSeconds());
			
}

void AActionShooterCharacter::OnLookUpAction(const FInputActionValue& Value)
{
AddControllerPitchInput(Value.GetMagnitude() * TurnRateGamepad *  GetWorld()->GetDeltaSeconds());
}

void AActionShooterCharacter::OnJumpActionStarted(const FInputActionValue& Value)
{
    FGameplayEventData PayLoad;
	
	PayLoad.Instigator = this;
	PayLoad.EventTag = JumpEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, JumpEventTag, PayLoad);
}

void AActionShooterCharacter::OnJumpActionEnded(const FInputActionValue& Value)
{
//StopJumping();
}

void AActionShooterCharacter::OnCrouchActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags, true);
	}
	
}

void AActionShooterCharacter::OnCrouchActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer* CrouchTagsPtr = &CrouchTags; // CrouchTags'i bir işaretçiye dönüştürme
		AbilitySystemComponent->CancelAbilities(CrouchTagsPtr);
	}
}

void AActionShooterCharacter::OnSprintActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags, true);
	}
}

void AActionShooterCharacter::OnSprintActionEnded(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer* SprintTagsPtr = &SprintTags; // CrouchTags'i bir işaretçiye dönüştürme
		AbilitySystemComponent->CancelAbilities(SprintTagsPtr);
	}
}




