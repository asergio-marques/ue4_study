// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/InMatch/PlayerInfo/SPlayerInfo.h"
#include "UI/InMatch/PlayerInfo/CharacterStatus/SCharacterStatus.h"
#include "UI/InMatch/PlayerInfo/Equipment/SPlayerLoadout.h"
#include "Gameplay/Characters/Components/SAttributesComponent.h"



// Information for the initial setup of the player loadout visualisation
void USPlayerInfo::Construct(USAttributesComponent* StatsComp) {
	
	if (CharStatusWidget && StatsComp) {

		// Bind character status widget functions to delegates
		StatsComp->CharacterSpawnedDelegate.AddDynamic(CharStatusWidget, &USCharacterStatus::OnCharacterSpawnComplete);
		StatsComp->HPChangedDelegate.AddDynamic(CharStatusWidget, &USCharacterStatus::OnHPChanged);
			
	}	
	
}


// 
void USPlayerInfo::OnLoadoutSpawned(FLoadoutInfo LoadoutInfo, uint8 InitialActiveWeaponSlotNumber) {

	if (LoadoutWidget) {
		
		LoadoutWidget->OnLoadoutSpawned(LoadoutInfo, InitialActiveWeaponSlotNumber);
		
	}

}


// 
void USPlayerInfo::OnWeaponChange(uint8 newSlot) {

	if (LoadoutWidget) {
		
		LoadoutWidget->OnWeaponChange(newSlot);
		
	}
	
}


// 
void USPlayerInfo::OnPlayerDeath(float DeathAnimTime) {

	if (LoadoutWidget) {
		
		LoadoutWidget->OnPlayerDeath(DeathAnimTime);
		
	}

	if (CharStatusWidget) {
		
		CharStatusWidget->K2_OnPlayerDeath(DeathAnimTime);
		
	}

}