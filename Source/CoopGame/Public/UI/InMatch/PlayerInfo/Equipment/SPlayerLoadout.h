// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SPlayerLoadout.generated.h"



class USEquipmentSlot;
class ASWeapon;
struct FLoadoutInfo;



/**
 *
 * Class that displays all information regarding the player character's loadout in the Player HUD, handling calls to individual slots.
 * 
 */
UCLASS()
class COOPGAME_API USPlayerLoadout : public UUserWidget {
	
	GENERATED_BODY()
	
	
public:
	// Information for the initial setup of the player loadout visualisation
	void OnLoadoutSpawned(FLoadoutInfo LoadoutInfo, uint8 InitialActiveWeaponSlotNumber);

	// Function called by the owning HUD once a weapon switch is commanded and a weapon is to be activated, triggering the HUD changes to signal a new weapon is now being utilized
	UFUNCTION()
	void OnWeaponChange(uint8 newSlot);
	
	// Highlight weapon slot number 1
	virtual void ActivateWeaponSlot1();

	// Highlight weapon slot number 2
	virtual void ActivateWeaponSlot2();

	// Highlight weapon slot number 3
	virtual void ActivateWeaponSlot3();

	// Highlight skill slot
	virtual void ActivateSkillSlot();

	// Deactivate skill slot
	virtual void DeactivateSkillSlot();

	// Function called once the player that owns this widget dies
	void OnPlayerDeath(float DeathAnimTime);
	
	
protected:	
	// Pointer to WeaponSlot1 object to be instanced in the child BP class
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USEquipmentSlot* WeaponSlot1;

	// Pointer to WeaponSlot2 object to be instanced in the child BP class 
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USEquipmentSlot* WeaponSlot2;

	// Pointer to WeaponSlot3 object to be instanced in the child BP class
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USEquipmentSlot* WeaponSlot3;

	// Pointer to SkillSlot object to be instanced in the child BP class
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USEquipmentSlot* SkillSlot;

	// Internal Status that symbolizes whether the skill slot is currently active.
	bool ActiveSkill = false;

	
private:
	// Pointer to slot of the currently active weapon
	UPROPERTY()
	USEquipmentSlot* ActiveWeaponSlot;
	
};
