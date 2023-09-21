// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/InMatch/SPlayerInMatchHUD.h"
#include "Gameplay/Characters/Components/SCharacterEquipmentComponent.h"
#include "Gameplay/Characters/SPlayerCharacter.h"
#include "Gameplay/Weapons/SWeapon.h"
#include "UI/InMatch/PlayerInfo/SPlayerInfo.h"
#include "UI/InMatch/PlayerInfo/CharacterStatus/SCharacterStatus.h"
#include "UI/InMatch/PlayerInfo/Equipment/SPlayerLoadout.h"
#include "UI/InMatch/PlayerInfo/Equipment/SCurrentWeaponStatus.h"
#include "UI/InMatch/PlayerInfo/Crosshair/SCrosshair.h"

// Sets default values for this HUD's parameters
ASPlayerInMatchHUD::ASPlayerInMatchHUD() {

	DeathAnimTime = 5.0f;
	
}

// Called when the game starts or when spawned
void ASPlayerInMatchHUD::BeginPlay() {
	
	Super::BeginPlay();
	
	ASPlayerCharacter* OwningCharacter = Cast<ASPlayerCharacter>(GetOwningPawn());
	
	// check if equipment component is also valid in addition to cast being successful
	// no point proceeding with anything if the character and its equipment component aren't valid
	if (GetOwningPlayerController() && OwningCharacter) {
		
		// Bind character death delegate to own function which will be in charge of making the individual widgets react to the event (fade out anim)
		OwningCharacter->CharacterDiedDelegate.AddDynamic(this, &ASPlayerInMatchHUD::OnOwningCharacterDeath);
		
		// Bind own functions to character equipment delegates; these functions will be in charge of making the individual widgets react to the weapon change
		if (OwningCharacter->CharEquipComp) {
			
			OwningCharacter->CharEquipComp->LoadoutSpawnDelegate.AddDynamic(this, &ASPlayerInMatchHUD::OnLoadoutSpawned);
			OwningCharacter->CharEquipComp->WeaponChangeDelegate.AddDynamic(this, &ASPlayerInMatchHUD::OnWeaponChange);
			
		}

		// Create player info widget
		if (PlayerInfoWidgetClass) {

			PlayerInfoWidget = Cast<USPlayerInfo>(CreateWidget(GetOwningPlayerController(), PlayerInfoWidgetClass));
			
			if (PlayerInfoWidget) {

				// construct with delegate bindings
				PlayerInfoWidget->AddToViewport();
				PlayerInfoWidget->Construct(OwningCharacter->StatsComp);
				
			}
			
		}

		// Create weapon status widget
		if (WeaponStatusWidgetClass) {

			WeaponStatusWidget = Cast<USCurrentWeaponStatus>(CreateWidget(GetOwningPlayerController(), WeaponStatusWidgetClass));
			
			if (WeaponStatusWidget) {

				WeaponStatusWidget->AddToViewport();
			
			}
			
		}

		// Create crosshair widget
		if (CrosshairWidgetClass) {

			CrosshairWidget = Cast<USCrosshair>(CreateWidget(GetOwningPlayerController(), CrosshairWidgetClass));

			if (CrosshairWidget) {
				
				CrosshairWidget->AddToViewport();
				
			}
			
		}
				
	}
	
}


// 
void ASPlayerInMatchHUD::OnLoadoutSpawned(FLoadoutInfo LoadoutInfo, ASWeapon* CurrentWeapon, uint8 InitialActiveWeaponSlotNumber) {

	if (PlayerInfoWidget) {
		
		PlayerInfoWidget->OnLoadoutSpawned(LoadoutInfo, InitialActiveWeaponSlotNumber);
		
	}
	
	// We can send CurrentWeapon as the previous one here as trying to unbind it will yield nothing
	if (CrosshairWidget) {
		
		CrosshairWidget->OnWeaponChange(CurrentWeapon, CurrentWeapon);
		
	}

	UE_LOG(LogTemp, Log, TEXT("ASPlayerInMatchHUD::OnLoadoutSpawned called by delegate!"));
	
}


void ASPlayerInMatchHUD::OnWeaponChange(ASWeapon* oldCurrentWeapon, ASWeapon* newCurrentWeapon, uint8 newSlot) {

	if (WeaponStatusWidget) {
		
		WeaponStatusWidget->OnWeaponChange(oldCurrentWeapon, newCurrentWeapon);
		
	}
	
	if (PlayerInfoWidget) {
		
		PlayerInfoWidget->OnWeaponChange(newSlot);
		
	}
	
	if (CrosshairWidget) {
		
		CrosshairWidget->OnWeaponChange(oldCurrentWeapon, newCurrentWeapon);
		
	}

	UE_LOG(LogTemp, Log, TEXT("ASPlayerInMatchHUD::OnWeaponChange called by delegate!"));
	
}

void ASPlayerInMatchHUD::OnOwningCharacterDeath() {

	if (PlayerInfoWidget) {

		PlayerInfoWidget->OnPlayerDeath(DeathAnimTime);
		
	}

	if (WeaponStatusWidget) {
		
		WeaponStatusWidget->K2_OnPlayerDeath(DeathAnimTime);
		
	}
	
	if (CrosshairWidget) {
		
		CrosshairWidget->K2_OnPlayerDeath(DeathAnimTime);
		
	}

	UE_LOG(LogTemp, Log, TEXT("ASPlayerInMatchHUD::OnOwningCharacterDeath called by delegate!"));
	
}
