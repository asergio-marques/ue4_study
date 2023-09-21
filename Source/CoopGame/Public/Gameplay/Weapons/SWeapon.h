// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "SWeapon.generated.h"



class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShakeBase;
class ASPlayerCharacter;



/*
 *
 * ASWeapon is the base class from which all player weapons are derived. It declares functions which the weapon archetype classes (e.g.: ASShootingWeapon) will define, which correspond to the following actions that can be taken by the player:
 *		Use Primary Action Press (Left-Mouse Button)
 *		Use Secondary Action Release (Left-Mouse Button)
 *		Use Primary Action Press (Right-Mouse Button)
 *		Use Secondary Action Release (Right-Mouse Button)
 *		Trigger Reload Functionality (Reload key)
 *
 *		It also stores information common to all weapons, such as weapon name, information for the player display, socket information, damage type and quantity, and exposes them when necessary
 * 
 */
UCLASS()
class COOPGAME_API ASWeapon : public AActor {
	
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ASWeapon();
	
	// Execute the pressing action of this weapon's primary action (to be fully overriden by child classes, no Super!)
	virtual void OnPrimaryWeaponActionPressed();

	// Execute the releasing action of this weapon's primary action (to be fully overriden by child classes, no Super!)
	virtual void OnPrimaryWeaponActionReleased();

	// Execute the pressing action of this weapon's secondary action (to be fully overriden by child classes, no Super!)
	virtual void OnSecondaryWeaponActionPressed();

	// Execute the releasing action of this weapon's secondary action (to be fully overriden by child classes, no Super!)
	virtual void OnSecondaryWeaponActionReleased();

	// Call the weapon reload function
	virtual void OnReloadPressed();
	
	// Make the player camera's shake according to the weapon's parameters
	virtual void ShakePlayerCamera();
	
	// Perform necessary actions to make a weapon ready for use in gameplay
	bool ActivateWeapon();
	
	// Perform necessary actions to make a weapon unable to be used in gameplay
	bool DeactivateWeapon();

	// Function to be defined by child classes for automatic activation of weapon stereotype/specific actions (Super should always be called on child classes!)
	virtual void TriggerAutomaticActions(void);

	// Requests broadcast of information from the subcomponents after this weapon's activation has been confirmed to be successful and broadcast
	virtual void RequestSubcomponentBroadcast(void);

	// Returns the reload type of this weapon (NoReload by default, should be overwritten by child classes if they have a reload system)
	virtual UReloadType GetReloadType() const;
	
	// Get the name of the socket to which the weapon shall attach to
	FORCEINLINE const FName& GetCharacterSocketName() const;

	// Get the weapon's short hand name
	FORCEINLINE const FName& GetWeaponShortName() const;

	// Get the weapon's activity status
	FORCEINLINE bool IsWeaponActive() const;

	// Mesh of the weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	// Pointer to the character that owns this weapon (used for attachment), must be assigned immediately after this actor is spawned
	ASPlayerCharacter* WeaponOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* CrosshairTexture;
		
	
protected:
	// Function to be defined by child classes for cancelling of weapon stereotype/specific actions (Super should always be called on child classes!)
	virtual void CancelOngoingActions(void);
	
	// Name of the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment")
	FName FullWeaponName;

	// Shorthand of the weapon for display on the UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment")
	FName ShortWeaponName;
	
	// Damage type dealt by the weapon
	UPROPERTY(EditDefaultsOnly, Category = "Base Parameters")
	TSubclassOf<UDamageType> DamageType;

	// Parameters for the camera shake executed upon weapon action
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TSubclassOf<UCameraShakeBase> UseCameraShake;
	
	// Name of socket to which the weapon shall attach to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment")
	FName CharacterSocketName;

	// Base damage of this weapon
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (ClampMin = 1.0, ClampMax = 100.0))
	float ShotDamage;

	// Represents the current active status of the weapon (if it's been selected for use by the player)
	bool IsActive;
	
};
