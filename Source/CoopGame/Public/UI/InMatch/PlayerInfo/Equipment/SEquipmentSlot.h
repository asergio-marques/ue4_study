// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SEquipmentSlot.generated.h"



class UImage;
class UTextBlock;
struct FSEquipmentInfoHUD;



/**
 *
 * Class that handles all processing related to the display of a single piece of equipment's slot in the Player HUD.
 * Can be utilized for weapons and skills both.
 * 
 */
UCLASS()
class COOPGAME_API USEquipmentSlot : public UUserWidget {
	
	GENERATED_BODY()


public:
	// Allows for the weapon short hand name and slot number to be set at construct time
	void Construct(FSEquipmentInfoHUD EquipmentInfo);

	// Changes the background of the Slot from NoUse to InUse
	UFUNCTION(BlueprintImplementableEvent)
	void K2_ActivateSlot();

	// Changes the background of the Slot from InUse to NoUse
	UFUNCTION(BlueprintImplementableEvent)
	void K2_DeactivateSlot();

	// Changes the background of the Slot to Disabled (if there is no equipment item in the slot to which this icon belongs)
	UFUNCTION(BlueprintImplementableEvent)
	void K2_DisableSlot();

	// Function called once the player that owns this widget dies
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void K2_OnPlayerDeath(float DeathAnimTime);
	
	// Exposes the weapon short hand name
	uint8 GetSlotNumber() const;

	
protected:
	// Background image 
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* SlotBG;

	// Short hand name of the weapon in this weapon slot
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* WeaponShortName;

	// Text displaying the slot number that this widget represents on the HUD
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* WeaponSlotNumber;

	// Internal saved variable for slot number for ease
	uint8 SlotNumber = 0;
};
