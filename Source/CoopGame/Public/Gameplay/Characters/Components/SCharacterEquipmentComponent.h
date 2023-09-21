// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "SCharacterEquipmentComponent.generated.h"



class ASPlayerCharacter;
class ASWeapon;



// Declaration for delegate type for broadcast of initial loadout upon successful spawn
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLoadoutSpawnedSignature, FLoadoutInfo, LoadoutInfo, ASWeapon*, CurrentWeapon, uint8, InitialActiveWeaponSlotNumber);

// Declaration for delegate type for broadcast of successful weapon change
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponChangeSignature, ASWeapon*, oldCurrentWeapon, ASWeapon*, newCurrentWeapon, uint8, newSlot);



/*
 *
 * todo documentation
 *
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USCharacterEquipmentComponent : public UActorComponent {
	
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	USCharacterEquipmentComponent();

	// Spawns the configured loadout for the character and delegates its addition to the loadout
	void SpawnDefaultLoadout();
	
	// Deactivate the current weapon and change the currently active weapon to the weapon in slot 1
	bool ChangeToWeapon1(void);

	// Deactivate the current weapon and change the currently active weapon to the weapon in slot 2
	bool ChangeToWeapon2(void);

	// Deactivate the current weapon and change the currently active weapon to the weapon in slot 3
	bool ChangeToWeapon3(void);

	// Execute the pressing action of the currently active weapon's primary action
	void PerformCurrentWeaponPrimaryActionPress(void);

	// Execute the releasing action of the currently active weapon's primary action
	void PerformCurrentWeaponPrimaryActionRelease(void);

	// Execute the pressing action of the currently active weapon's secondary action
	void PerformCurrentWeaponSecondaryActionPress(void);

	// Execute the releasing action of the currently active weapon's secondary action
	void PerformCurrentWeaponSecondaryActionRelease(void);

	// Request a reload to the current weapon
	void ReloadCurrentWeapon(void);

	// Delegate to broadcast of successful loadout spawn
	FOnLoadoutSpawnedSignature LoadoutSpawnDelegate;
	
	// Delegate to broadcast successful weapon change events
	FOnWeaponChangeSignature WeaponChangeDelegate;
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Currently active weapon
	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
	ASWeapon* CurrentWeapon;
	
	// Class of weapon in Slot 1
	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
	ASWeapon* WeaponInSlot1;
	
	// Class of weapon in Slot 2
	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
	ASWeapon* WeaponInSlot2;

	// Class of weapon in Slot 3
	UPROPERTY(BlueprintReadOnly, Category = "Loadout")
	ASWeapon* WeaponInSlot3;

	
private:
	// Adds a new weapon to the loadout. Returns false if the weapon already exists in the inventory, true if it's a new weapon.
	UFUNCTION(BlueprintCallable)
	bool AddWeaponToLoadout(ASWeapon* NewWeapon, bool bActivateFirstWeapon);
	
	// Checks if a weapon of the same type as the newly created one is already present in the loadout
	bool CanNewWeaponBeAdded(ASWeapon* NewWeapon);
	
	// Finds the first open slot and adds the new weapon to said slot, returning the success of the operation
	bool FindAndAddWeaponToSlot(ASWeapon* NewWeapon);

	// Set the weapon's OwningCharacter variable and attachment to the player character
	bool SetupWeaponCharacterRelationship(ASWeapon* NewWeapon, ASPlayerCharacter* OwningCharacter);

	// Creates a temporary object that holds all information relative to the player loadout necessary for the HUD
	FLoadoutInfo CreateLoadoutInfo() const;
	
	// Array containing the weapons to be used by the character (only the first 3 members will be taken into consideration)
	UPROPERTY(EditAnywhere, Category = "Loadout")
	TArray<TSubclassOf<ASWeapon>> DefaultWeapons;
		
};
