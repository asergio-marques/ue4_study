// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/InMatch/PlayerInfo/Equipment/SCurrentWeaponStatus.h"
#include "Gameplay/Weapons/Types/Shooting/SShootingWeapon.h"
#include "Gameplay/Weapons/Components/SAmmoSystemComponent.h"
#include "Components/TextBlock.h"
#include "Components/TextWidgetTypes.h"




// 
void USCurrentWeaponStatus::OnWeaponChange(ASWeapon* oldCurrentWeapon, ASWeapon* newCurrentWeapon) {

	if (oldCurrentWeapon) {
		
		ASShootingWeapon* PreviousWeapon = Cast<ASShootingWeapon>(oldCurrentWeapon);

		// Unbind ammo system delegates if weapon is a shooting weapon (has an ammo system component)
		if (PreviousWeapon && PreviousWeapon->AmmoSysComp) {

			if (PreviousWeapon->AmmoSysComp->AmmoChangedDelegate.IsBound()) {
				
				PreviousWeapon->AmmoSysComp->AmmoChangedDelegate.RemoveDynamic(this, &USCurrentWeaponStatus::OnAmmoChanged);
				
			}

			UE_LOG(LogTemp, Log, TEXT("previous weapon ammo change delegate bound? %u"), PreviousWeapon->AmmoSysComp->AmmoChangedDelegate.IsBound());
			
		}
		
	}
	
	if (newCurrentWeapon) {
		
		ASShootingWeapon* CurrentWeapon = Cast<ASShootingWeapon>(newCurrentWeapon);

		// if weapon is a shooting weapon, it has an ammo system component, so bind the functions there and set all parameters
		if (CurrentWeapon && CurrentWeapon->AmmoSysComp) {

			// Make widget visible and give it the correct color and text labels
			SetVisibility(ESlateVisibility::Visible);
			K2_UpdateBackground(CurrentWeapon->AmmoSysComp->GetReloadType());
			UpdateLabels(CurrentWeapon->AmmoSysComp->GetReloadType());

			// Set fixed values
			if (MaximumBullets) {
				
				MaximumBullets->SetText(FText::AsNumber(CurrentWeapon->AmmoSysComp->GetMaximumBullets()));
				
			}

			// Bind ammo system delegates
			CurrentWeapon->AmmoSysComp->AmmoChangedDelegate.AddDynamic(this, &USCurrentWeaponStatus::OnAmmoChanged);
			UE_LOG(LogTemp, Log, TEXT("current weapon ammo change delegate bound? %u"), CurrentWeapon->AmmoSysComp->AmmoChangedDelegate.IsBound());
			
		}
		// else, use N/A values for everything
		else {

			// default grey
			K2_UpdateBackground(UReloadType::NoReload);
			SetVisibility(ESlateVisibility::Hidden);
		
		}
		
	}
		
}


// Function that updates the values for the bullets available and the reloads available after changes are performed to the ammo quantity, be it removals or additions aka reloading (to be rebound at weapon switch) 
void USCurrentWeaponStatus::OnAmmoChanged(int32 newBulletsCurrentlyActive, int32 newAvailableReloads) {

	if (AvailableBullets) {
		
		AvailableBullets->SetText(FText::AsNumber(newBulletsCurrentlyActive));
		
	}

	if (AvailableReloads) {
		
		if (newAvailableReloads >= 0) {
			
			AvailableReloads->SetText(FText::AsNumber(newAvailableReloads));
			
		}
		// newAvailableReloads under 0 is code meaning that there is no limits on reloads
		else {
			
			AvailableReloads->SetText(FText::FromString("N/A"));
			
		}
		
	}

}


// Function that only updates the text labels of the CurrentWeaponStatus HUD element, done upon weapon switch
void USCurrentWeaponStatus::UpdateLabels(UReloadType newCurrentWeaponType) {

	if (AvailableReloadsText) {
		
		switch (newCurrentWeaponType) {

		case UReloadType::PassiveReload:
			AvailableReloadsText->SetText(FText::FromString("Bullets left"));
			break;
		
		case UReloadType::ActiveMagazineReload:
			AvailableReloadsText->SetText(FText::FromString("Magazines left"));
			break;
				
		case UReloadType::ActivePerBulletReload:
			AvailableReloadsText->SetText(FText::FromString("Bullets left"));
			break;
		
		default:
			AvailableReloadsText->SetText(FText::FromString("Reloads left"));
			// as good as nothing since it's gonna be hidden anyway but shrug
			break;
		
		}
		
	}
	
}
