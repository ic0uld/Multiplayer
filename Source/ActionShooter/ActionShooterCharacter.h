// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "ActionShooterTypes.h"
#include "ActionShooterCharacter.generated.h"

class UXAbilitySystemComponent;
class UXAttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;

UCLASS(config=Game)
class AActionShooterCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* MoveForwardInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* MoveSideInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* TurnInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* LookUpInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* CrouchInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* SprintInputAction;


public:

	AActionShooterCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Input)
	float TurnRateGamepad;
	
bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	
	void GiveAbility();
	void ApplyStartupEffect();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual void PawnClientRestart() override;
	
	UPROPERTY(EditDefaultsOnly)
	UXAbilitySystemComponent* AbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly)
	UXAttributeSetBase* AttributeSetBase;
	
	void OnMoveForwardAction(const FInputActionValue& Value);
	void OnMoveSideAction(const FInputActionValue& Value);
	void OnTurnAction(const FInputActionValue& Value);
	void OnLookUpAction(const FInputActionValue& Value);
	void OnJumpActionStarted(const FInputActionValue& Value);
	void OnJumpActionEnded(const FInputActionValue& Value);
	void OnCrouchActionStarted(const FInputActionValue& Value);
	void OnCrouchActionEnded(const FInputActionValue& Value);
	void OnSprintActionStarted(const FInputActionValue& Value);
	void OnSprintActionEnded(const FInputActionValue& Value);
	
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:

	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const;

	virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

	class UXFootStepsComponent* GetFootStepComponent() const;

	void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data);
	

	virtual void Landed(const FHitResult& Hit) override;

protected:

	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
	FCharacterData CharacterData;

	UFUNCTION()
	void OnRep_CharacterData();

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplicaton = false);

	UPROPERTY(EditDefaultsOnly)
	class UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(BlueprintReadOnly)
	class UXFootStepsComponent* FootStepsComponent;

	//GAS EVENT
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	//GAS TAGS

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

	//GAS EFFECTS
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

	//DELEGATES
FDelegateHandle MaxMovementSpeedChangedDelegateHandle;
	
};

