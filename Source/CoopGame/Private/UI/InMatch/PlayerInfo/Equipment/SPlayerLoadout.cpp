// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InMatch/PlayerInfo/Equipment/SPlayerLoadout.h"
#include "UI/InMatch/PlayerInfo/Equipment/SEquipmentSlot.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"



// Information for the initial setup of the player loadout visualisation
void USPlayerLoadout::OnLoadoutSpawned(FLoadoutInfo LoadoutInfo, uint8 InitialActiveWeaponSlotNumber) {

	if (WeaponSlot1) {
		
		WeaponSlot1->Construct(LoadoutInfo.Weapon1Info);
		
	}

	if (WeaponSlot2) {
		
		WeaponSlot2->Construct(LoadoutInfo.Weapon2Info);
		
	}
	
	if (WeaponSlot3) {
		
		WeaponSlot3->Construct(LoadoutInfo.Weapon3Info);
		
	}
	
	if (SkillSlot) {
		
		SkillSlot->Construct(LoadoutInfo.SkillInfo);
		
	}
	
	UE_LOG(LogTemp, Log, TEXT("Initial active weapon is %u"), InitialActiveWeaponSlotNumber);
	switch (InitialActiveWeaponSlotNumber) {

	case 1:
		ActivateWeaponSlot1();
		break;
		
	case 2:
		ActivateWeaponSlot2();
		break;
		
	case 3:
		ActivateWeaponSlot3();
		break;
		
	default:
		// I seriously hope this doesn't happen
		UE_LOG(LogTemp, Warning, TEXT("Invalid initially active weapon number used for HUD construction"));
		break;
		
	}
	
}



void USPlayerLoadout::OnWeaponChange(uint8 newSlot) {

	UE_LOG(LogTemp, Log, TEXT("USPlayerLoadout::OnWeaponChange called by delegate!"));
	
	switch (newSlot) {

		case 1:
		ActivateWeaponSlot1();
		break;
		
	case 2:
		ActivateWeaponSlot2();
		break;
		
	case 3:
		ActivateWeaponSlot3();
		break;
		
	default:
		// do nothing, invalid value
		break;
		
	}

}


// Highlight weapon slot number 1
void USPlayerLoadout::ActivateWeaponSlot1() {

	uint8 PreviousSlotNumber = 0;
	uint8 NewSlotNumber = 0;
	
	if (ActiveWeaponSlot) {

		PreviousSlotNumber = ActiveWeaponSlot->GetSlotNumber();
		ActiveWeaponSlot->K2_DeactivateSlot();
		
	}
	if (WeaponSlot1) {

		NewSlotNumber = WeaponSlot1->GetSlotNumber();
		WeaponSlot1->K2_ActivateSlot();
		ActiveWeaponSlot = WeaponSlot1;
		
	}
	
	UE_LOG(LogTemp, Log, TEXT("Executing change from slot %u to slot %u"), PreviousSlotNumber, NewSlotNumber);
    
}


// Highlight weapon slot number 2
void USPlayerLoadout::ActivateWeaponSlot2() {

	uint8 PreviousSlotNumber = 0;
	uint8 NewSlotNumber = 0;
	
	if (ActiveWeaponSlot) {

		PreviousSlotNumber = ActiveWeaponSlot->GetSlotNumber();
		ActiveWeaponSlot->K2_DeactivateSlot();
		
	}
	if (WeaponSlot2) {

		NewSlotNumber = WeaponSlot2->GetSlotNumber();
		WeaponSlot2->K2_ActivateSlot();
		ActiveWeaponSlot = WeaponSlot2;
		
	}
	
	UE_LOG(LogTemp, Log, TEXT("Executing change from slot %u to slot %u"), PreviousSlotNumber, NewSlotNumber);
    
}


// Highlight weapon slot number 3
void USPlayerLoadout::ActivateWeaponSlot3() {

	uint8 PreviousSlotNumber = 0;
	uint8 NewSlotNumber = 0;
	
	if (ActiveWeaponSlot) {

		PreviousSlotNumber = ActiveWeaponSlot->GetSlotNumber();
		ActiveWeaponSlot->K2_DeactivateSlot();
		
	}
	if (WeaponSlot3) {

		NewSlotNumber = WeaponSlot3->GetSlotNumber();
		WeaponSlot3->K2_ActivateSlot();
		ActiveWeaponSlot = WeaponSlot3;
		
	}
	
	UE_LOG(LogTemp, Log, TEXT("Executing change from slot %u to slot %u"), PreviousSlotNumber, NewSlotNumber);
	
}


// Highlight skill slot
void USPlayerLoadout::ActivateSkillSlot() {

	if (SkillSlot) {
		
		SkillSlot->K2_ActivateSlot();
		UE_LOG(LogTemp, Log, TEXT("Activating Skill UI slot"));
		
	}

	
    
}


// Deactivate skill slot
void USPlayerLoadout::DeactivateSkillSlot() {

	if (SkillSlot) {
		
		SkillSlot->K2_DeactivateSlot();
		UE_LOG(LogTemp, Log, TEXT("Deactivating Skill UI slot"));
		
	}
    
}

void USPlayerLoadout::OnPlayerDeath(float DeathAnimTime) {

	if (WeaponSlot1) {
		
		WeaponSlot1->K2_OnPlayerDeath(DeathAnimTime);
		
	}

	if (WeaponSlot2) {
		
		WeaponSlot2->K2_OnPlayerDeath(DeathAnimTime);
		
	}
	
	if (WeaponSlot3) {
		
		WeaponSlot3->K2_OnPlayerDeath(DeathAnimTime);
		
	}
	
	if (SkillSlot) {
		
		SkillSlot->K2_OnPlayerDeath(DeathAnimTime);
		
	}
	
	
}
