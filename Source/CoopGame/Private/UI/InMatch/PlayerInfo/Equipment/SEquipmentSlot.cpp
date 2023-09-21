// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/InMatch/PlayerInfo/Equipment/SEquipmentSlot.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "Components/TextBlock.h"



// Exposes the weapon short hand name
uint8 USEquipmentSlot::GetSlotNumber() const {

	return SlotNumber;

}


// Allows for the weapon short hand name and slot number to be set at construct time
void USEquipmentSlot::Construct(FSEquipmentInfoHUD EquipmentInfo) {

	if (EquipmentInfo.IsEquipped) {
		
		WeaponShortName->SetVisibility(ESlateVisibility::Visible);
		WeaponShortName->SetText(FText::FromName(EquipmentInfo.EquipmentShortName));

		// check if the slot is valid for a weapon (if weapon, show number)
		if (EquipmentInfo.EquipmentSlot > 0 && EquipmentInfo.EquipmentSlot <= 3) {
		
			WeaponSlotNumber->SetVisibility(ESlateVisibility::Visible);
			WeaponSlotNumber->SetText(FText::AsNumber(EquipmentInfo.EquipmentSlot));
			SlotNumber = EquipmentInfo.EquipmentSlot;
		
		}
		// check if slot is valid for a skill, if it's skill, hide number
		else if (EquipmentInfo.EquipmentSlot == 0) {

			WeaponSlotNumber->SetVisibility(ESlateVisibility::Hidden);
		
		}
		
	}
	// if the slot for this weapon is not equipped, then disable slot
	else {
		
		WeaponSlotNumber->SetVisibility(ESlateVisibility::Hidden);
		WeaponShortName->SetVisibility(ESlateVisibility::Hidden);
		K2_DisableSlot();
		
	}
	
}
