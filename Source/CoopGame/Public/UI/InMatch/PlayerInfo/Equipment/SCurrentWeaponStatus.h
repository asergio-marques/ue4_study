// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "SCurrentWeaponStatus.generated.h"



class UTextBlock;
class ASWeapon;



/**
 *
 * Class for the HUD element that displays the current active weapon
 * 
 */
UCLASS()
class COOPGAME_API USCurrentWeaponStatus : public UUserWidget {
	
	GENERATED_BODY()

	
public:
	// Function called by the owning HUD once a weapon switch is commanded and a weapon is to be activated, binding functions to the new weapon objects' delegates and unbinding functions from the previous one
	// Note: the initial set up of the ammo values is done at activation of automatic abilities (broadcast of initial values after activation of weapon) 
	UFUNCTION()
	void OnWeaponChange(ASWeapon* oldCurrentWeapon, ASWeapon* newCurrentWeapon);

	// Function that only updates the background of the CurrentWeaponStatus HUD element, done upon weapon switch
	UFUNCTION(BlueprintImplementableEvent)
	void K2_UpdateBackground(UReloadType newCurrentWeaponType);

	// Function that updates the values for the bullets available and the reloads available after changes are performed to the ammo quantity, be it removals or additions aka reloading (to be rebound at weapon switch)
	UFUNCTION()
	void OnAmmoChanged(int32 newBulletsCurrentlyActive, int32 newAvailableReloads);

	// Function called once the player that owns this widget dies
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void K2_OnPlayerDeath(float DeathAnimTime);


protected:
	// Pointer to text object that displays how many bullets are available to the player
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AvailableBullets;

	// Pointer to text object that displays the maximum bullets the weapon currently in use may have (should be changed only at weapon switch)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MaximumBullets;
	
	// Pointer to text object that displays how many more reloading actions are available to the player (in bullets, in case the current weapon is of a per-bullet type, or in magazines, in case the current weapon is of a magazine type)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AvailableReloads;

	// Pointer to text object for the text "bullets left" (should not be changed, ever)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MaximumBulletsText;

	// Pointer to text object that displays the types of reload (should be changed only at weapon switch)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AvailableReloadsText;

	// Function that only updates the text labels of the CurrentWeaponStatus HUD element, done upon weapon switch
	void UpdateLabels(UReloadType newCurrentWeaponType);
	
};
