// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "SPlayerCharacter.generated.h"



class UCameraComponent;
class USpringArmComponent;
class USCharacterEquipmentComponent;
class USAttributesComponent;
class ASWeapon;



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDiedSignature);



/*
 *
 * Class that implements the player character, the main vehicle through which players can play (wow what a load of nothing).
 * Implements the basic movement functions (which derive from the ACharacter base class) and weapon action calls, plus utilities necessary for any child component.
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerCharacter : public ACharacter {

	GENERATED_BODY()

	
public:
	// Sets default values for this character's properties
	ASPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Returns player character's eye location
	virtual FVector GetPawnViewLocation() const override;
	
	// Spring arm component to which the player camera is attached
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;
	
	// Camera component through which the player can visualize the game
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	// Component that handles everything related to inventory management
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USCharacterEquipmentComponent* CharEquipComp;

	// Component that handles everything related to attributes (calculation of damage, health management...)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributesComponent* StatsComp;

	// Delegate to broadcast the character's death
	FOnCharacterDiedSignature CharacterDiedDelegate;
	
	// variable that dictates if the camera should be zoomed in
	bool bWantsToZoomIn;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when "Move Forward" or "Move Backward" keys are pressed; "Move Forward" -> Value = 1.0f, "Move Backward" -> Value = -1.0f
	void MoveForward(float Value);

	// Called when "Move Right" or "Move Left" keys are pressed; "Move Right" -> Value = 1.0f, "Move Left" -> Value = -1.0f
	void MoveRight(float Value);

	// Called when "Crouch" key is pressed
	void CrouchStart(void);
	
	// Called when "Crouch" key is released
	void CrouchEnd(void);

	// Called when "Jump" key is pressed
	virtual void Jump(void) override;

	// Called when "WeaponSlot1" key is pressed
	virtual void ChangetoWeaponSlot1(void);

	// Called when "WeaponSlot2" key is pressed
	virtual void ChangetoWeaponSlot2(void);

	// Called when "WeaponSlot3" key is pressed
	virtual void ChangetoWeaponSlot3(void);
	
	// Called when "MousePrimaryAction" key is pressed
	virtual void WeaponPrimaryActionPress(void);

	// Called when "MousePrimaryAction" key is released
	virtual void WeaponPrimaryActionRelease(void);

	// Called when "MouseSecondaryAction" key is pressed
	virtual void WeaponSecondaryActionPress(void);
	
	// Called when "MouseSecondaryAction" key is released
	virtual void WeaponSecondaryActionRelease(void);

	// Called when "ReloadWeapon" is pressed
	virtual void ReloadPressed(void);

	// Called when "WeaponSlot1" is pressed
	virtual void ChangeToWeaponSlot1(void);
	
	// Called when "WeaponSlot2" is pressed
	virtual void ChangeToWeaponSlot2(void);
	
	// Called when "WeaponSlot3" is pressed
	virtual void ChangeToWeaponSlot3(void);

	// Called when a health change has been detected by the attribute component
	UFUNCTION()
	virtual void OnHPChanged(USAttributesComponent* AttrComp, float CurrentHP, float DeltaHP, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
	// Configurable value for zoom-out capabilities, also the default FOV for a character
	UPROPERTY(EditDefaultsOnly, Category = "Camera Settings")
	float DefaultFOV;

	// Configurable value for zoom-in capabilities
	UPROPERTY(EditDefaultsOnly, Category = "Camera Settings")
	float ZoomedInFOV;

	// Speed at which the POV should be adjusted after zoom-in/zoom-out is triggered (in degrees/second)
	UPROPERTY(EditDefaultsOnly, Category = "Camera Settings", meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float POVInterpSpeed;

	// Variable that signalizes if the player character is alive
	UPROPERTY(BlueprintReadOnly, Category = "Player Status")
	bool bIsAlive;

	
private:
	// Smooths the adjustment of the Camera FOV, called in Tick()
	virtual void ProcessPOV(float DeltaTime);
	
};
