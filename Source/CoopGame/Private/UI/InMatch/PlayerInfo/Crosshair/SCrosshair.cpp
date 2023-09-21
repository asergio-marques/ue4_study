// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/InMatch/PlayerInfo/Crosshair/SCrosshair.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Gameplay/Weapons/Components/SAmmoSystemComponent.h"
#include "Gameplay/Weapons/Types/Shooting/SShootingWeapon.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"



// First operations upon construction
void USCrosshair::NativeConstruct() {
	
	Super::NativeConstruct();

	// hide reload widgets at start and set progress to 0
	HideAllReloadWidgets();
	
}


// Replaces the reticule being displayed and stores the reload type of the new active weapon
void USCrosshair::OnWeaponChange(ASWeapon* oldCurrentWeapon, ASWeapon* newCurrentWeapon) {

	// set crosshair texture and reload type
	ReticuleWidget->SetBrushFromTexture(newCurrentWeapon->CrosshairTexture);
	CurrentWeaponReloadType = newCurrentWeapon->GetReloadType();

	// todo check if necessary
	// Hide all reload progress widgets; they will be unhidden upon reload trigger
	// HideAllReloadWidgets();
	
	if (oldCurrentWeapon) {
		
		ASShootingWeapon* PreviousWeapon = Cast<ASShootingWeapon>(oldCurrentWeapon);

		// Unbind ammo system delegates if weapon is a shooting weapon (has an ammo system component)
		if (PreviousWeapon && PreviousWeapon->AmmoSysComp) {
			
			UnbindReloadDelegates(PreviousWeapon->AmmoSysComp);

		}
		
	}
	
	if (newCurrentWeapon) {
		
		ASShootingWeapon* CurrentWeapon = Cast<ASShootingWeapon>(newCurrentWeapon);

		// if weapon is a shooting weapon, it has an ammo system component, so bind the functions there and set all parameters
		if (CurrentWeapon && CurrentWeapon->AmmoSysComp) {

			BindReloadDelegates(CurrentWeapon->AmmoSysComp);
			
		}
		
	}
	
}


// Auxiliary function for hiding all reload-indicating widgets
void USCrosshair::HideAllReloadWidgets() {

	// hide reload widgets at start and set progress to 0
	// general widgets
	ReloadCancelledIcon->SetVisibility(ESlateVisibility::Hidden);
	ReloadCompleteIcon->SetVisibility(ESlateVisibility::Hidden);
	
	// passive widgets
	PassiveReloadIndicator->SetVisibility(ESlateVisibility::Hidden);
	PassiveBulletsInBar->SetVisibility(ESlateVisibility::Hidden);
	PassiveBulletsInBar->SetPercent(0.0f);
	
	// magazine widgets
	MagazineReloadIcon->SetVisibility(ESlateVisibility::Hidden);
	MagazineBulletsInBar->SetVisibility(ESlateVisibility::Hidden);
	MagazineBulletsInBar->SetPercent(0.0f);
	MagazineCompleteBar->SetVisibility(ESlateVisibility::Hidden);
	MagazineCompleteBar->SetPercent(0.0f);
	
	// per-bullet widgets
	PerBulletIcon->SetVisibility(ESlateVisibility::Hidden);
	PerBulletBulletsInBar->SetVisibility(ESlateVisibility::Hidden);
	PerBulletBulletsInBar->SetPercent(0.0f);
	PerBulletCompleteBar->SetVisibility(ESlateVisibility::Hidden);
	PerBulletCompleteBar->SetPercent(0.0f);
	
}


// Auxiliary function for processing the unbinding of the previous weapon's reload system delegates
void USCrosshair::UnbindReloadDelegates(USAmmoSystemComponent* AmmoSysComp) {

	if (AmmoSysComp) {
		
		// which delegates to bind/unbind depends on the reloadtype!
		switch (AmmoSysComp->GetReloadType()) {

		case UReloadType::PassiveReload:
			UnbindPassiveReloadDelegates(AmmoSysComp);
			break;

		case UReloadType::ActiveMagazineReload:
			UnbindMagazineReloadDelegates(AmmoSysComp);
			break;

		case UReloadType::ActivePerBulletReload:
			UnbindPerBulletReloadDelegates(AmmoSysComp);
			break;

		default:
			// No reload configured, no functions to unbind
			break;
				
		}
		
	}
	
}


// Auxiliary function for unbinding all passive reload system delegates
void USCrosshair::UnbindPassiveReloadDelegates(USAmmoSystemComponent* AmmoSysComp) {

	if (AmmoSysComp) {
		
		// Bullets added delegate (for +1 pop-up once bullets-in)
		if (AmmoSysComp->BulletsAddedDelegate.IsBound()) {

			AmmoSysComp->BulletsAddedDelegate.RemoveDynamic(this, &USCrosshair::K2_OnBulletsAdded);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous bullets added delegate bound? %u"), AmmoSysComp->BulletsAddedDelegate.IsBound());

	
		// Passive start delegate (for initial delay animation and successive bullets-in)
		if (AmmoSysComp->PassiveStartDelegate.IsBound()) {

			AmmoSysComp->PassiveStartDelegate.RemoveDynamic(this, &USCrosshair::USCrosshair::K2_OnPassiveReloadStarted);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous passive start delegate bound? %u"), AmmoSysComp->PassiveStartDelegate.IsBound());


		// Passive reload cycle start delegate (looping section)
		if (AmmoSysComp->PassiveReloadCycleStartDelegate.IsBound()) {

			AmmoSysComp->PassiveReloadCycleStartDelegate.RemoveDynamic(this, &USCrosshair::K2_OnPassiveReloadCycleStarted);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous passive reload cycle start delegate bound? %u"), AmmoSysComp->PassiveReloadCycleStartDelegate.IsBound());
		
		
		// Passive cancel delegate (for reload cancel animation to be triggered)
		if (AmmoSysComp->PassiveCancelDelegate.IsBound()) {

			AmmoSysComp->PassiveCancelDelegate.RemoveDynamic(this, &USCrosshair::K2_OnPassiveReloadCancelled);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous passive cancel delegate bound? %u"), AmmoSysComp->PassiveCancelDelegate.IsBound());
		
	}
	
}


// Auxiliary function for unbinding all magazine reload system delegates
void USCrosshair::UnbindMagazineReloadDelegates(USAmmoSystemComponent* AmmoSysComp) {

	if (AmmoSysComp) {
		
		// Magazine start delegate (start of bullets-in animation)
		if (AmmoSysComp->MagazineStartDelegate.IsBound()) {

			AmmoSysComp->MagazineStartDelegate.RemoveDynamic(this, &USCrosshair::K2_OnMagazineReloadStarted);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous magazine start delegate bound? %u"), AmmoSysComp->MagazineStartDelegate.IsBound());

	
		// Magazine bullets-in delegate (start of wind-down animation)
		if (AmmoSysComp->MagazineBulletsInDelegate.IsBound()) {

			AmmoSysComp->MagazineBulletsInDelegate.RemoveDynamic(this, &USCrosshair::K2_OnMagazineReloadBulletsIn);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous magazine bullets-in delegate bound? %u"), AmmoSysComp->MagazineBulletsInDelegate.IsBound());

	
		// Magazine complete delegate (start of fade-out animation)
		if (AmmoSysComp->MagazineCompleteDelegate.IsBound()) {

			AmmoSysComp->MagazineCompleteDelegate.RemoveDynamic(this, &USCrosshair::K2_OnMagazineReloadComplete);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous magazine complete delegate bound? %u"), AmmoSysComp->MagazineCompleteDelegate.IsBound());

	
		// Magazine cancel delegate (for reload cancel animation to be triggered)
		if (AmmoSysComp->MagazineCancelDelegate.IsBound()) {

			AmmoSysComp->MagazineCancelDelegate.RemoveDynamic(this, &USCrosshair::K2_OnMagazineReloadCancelled);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous magazine cancel delegate bound? %u"), AmmoSysComp->MagazineCancelDelegate.IsBound());
		
	}
	
}


// Auxiliary function for unbinding all per-bullet reload system delegates
void USCrosshair::UnbindPerBulletReloadDelegates(USAmmoSystemComponent* AmmoSysComp) {

	if (AmmoSysComp) {
		
		// Bullets added delegate (for +x pop-up once bullets-in)
		if (AmmoSysComp->BulletsAddedDelegate.IsBound()) {

			AmmoSysComp->BulletsAddedDelegate.RemoveDynamic(this, &USCrosshair::K2_OnBulletsAdded);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous bullets added delegate bound? %u"), AmmoSysComp->BulletsAddedDelegate.IsBound());
	

		// Per-bullet start delegate (start of readying-up animation)
		if (AmmoSysComp->PerBulletStartDelegate.IsBound()) {

			AmmoSysComp->PerBulletStartDelegate.RemoveDynamic(this, &USCrosshair::K2_OnPerBulletReloadStarted);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous per-bullet start delegate bound? %u"), AmmoSysComp->PerBulletStartDelegate.IsBound());

		
		// Per-bullet reload cycle start delegate (looping section)
		if (AmmoSysComp->PerBulletReloadCycleStartDelegate.IsBound()) {

			AmmoSysComp->PerBulletReloadCycleStartDelegate.RemoveDynamic(this, &USCrosshair::K2_OnPerBulletReloadCycleStarted);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous per-bullet reload cycle start delegate bound? %u"), AmmoSysComp->PerBulletReloadCycleStartDelegate.IsBound());
		
	
		// Per-bullet timed cancel delegate (start of wind-down animation)
		if (AmmoSysComp->PerBulletTimedCancelDelegate.IsBound()) {

			AmmoSysComp->PerBulletTimedCancelDelegate.RemoveDynamic(this, &USCrosshair::K2_OnPerBulletCancelledWithTime);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous per-bullet timed cancel delegate bound? %u"), AmmoSysComp->PerBulletTimedCancelDelegate.IsBound());


		// Per-bullet complete delegate (successful timed cancel)
		if (AmmoSysComp->PerBulletCompleteDelegate.IsBound()) {

			AmmoSysComp->PerBulletCompleteDelegate.RemoveDynamic(this, &USCrosshair::K2_OnPerBulletComplete);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous per-bullet complete delegate bound? %u"), AmmoSysComp->PerBulletCompleteDelegate.IsBound());

		
		// Per-bullet instant cancel delegate (for reload cancel animation to be triggered)
		if (AmmoSysComp->PerBulletInstantCancelDelegate.IsBound()) {

			AmmoSysComp->PerBulletInstantCancelDelegate.RemoveDynamic(this, &USCrosshair::K2_OnPerBulletCancelledInstant);
				
		}
		UE_LOG(LogTemp, Log, TEXT("previous per-bullet instant cancel delegate bound? %u"), AmmoSysComp->PerBulletInstantCancelDelegate.IsBound());
		
	}
	
}


// Auxiliary function for processing of new weapon reload type and binding of appropriate delegates
void USCrosshair::BindReloadDelegates(USAmmoSystemComponent* AmmoSysComp) {

	if (AmmoSysComp)
	{
		// which delegates to bind/unbind depends on the reloadtype!
		switch (AmmoSysComp->GetReloadType()) {

		case UReloadType::PassiveReload:
			BindPassiveReloadDelegates(AmmoSysComp);
			break;

		case UReloadType::ActiveMagazineReload:
			BindMagazineReloadDelegates(AmmoSysComp);
			break;

		case UReloadType::ActivePerBulletReload:
			BindPerBulletReloadDelegates(AmmoSysComp);
			break;

		default:
			// No reload configured, no functions to unbind
			break;
				
		}

	}		
	
}


// Auxiliary function for binding all passive reload system delegates
void USCrosshair::BindPassiveReloadDelegates(USAmmoSystemComponent* AmmoSysComp) {

	if (AmmoSysComp) {
		
		// Bullets added delegate (for +1 pop-up once bullets-in)
		AmmoSysComp->BulletsAddedDelegate.AddDynamic(this, &USCrosshair::K2_OnBulletsAdded);
		UE_LOG(LogTemp, Log, TEXT("current bullets added delegate bound? %u"), AmmoSysComp->BulletsAddedDelegate.IsBound());
	
		// Passive start delegate (for initial delay animation and successive bullets-in)
		AmmoSysComp->PassiveStartDelegate.AddDynamic(this, &USCrosshair::USCrosshair::K2_OnPassiveReloadStarted);
		UE_LOG(LogTemp, Log, TEXT("current passive start delegate bound? %u"), AmmoSysComp->PassiveStartDelegate.IsBound());

		// Passive reload cycle start delegate (looping section)
		AmmoSysComp->PassiveReloadCycleStartDelegate.AddDynamic(this, &USCrosshair::K2_OnPassiveReloadCycleStarted);
		UE_LOG(LogTemp, Log, TEXT("current passive reload cycle start delegate bound? %u"), AmmoSysComp->PassiveReloadCycleStartDelegate.IsBound());
		
		// Passive cancel delegate (for reload cancel animation to be triggered)
		AmmoSysComp->PassiveCancelDelegate.AddDynamic(this, &USCrosshair::K2_OnPassiveReloadCancelled);
		UE_LOG(LogTemp, Log, TEXT("current passive cancel delegate bound? %u"), AmmoSysComp->PassiveCancelDelegate.IsBound());
		
	}
	
}


// Auxiliary function for binding all magazine reload system delegates
void USCrosshair::BindMagazineReloadDelegates(USAmmoSystemComponent* AmmoSysComp) {

	if (AmmoSysComp) {
		
		// Magazine start delegate (start of bullets-in animation)
		AmmoSysComp->MagazineStartDelegate.AddDynamic(this, &USCrosshair::K2_OnMagazineReloadStarted);	
		UE_LOG(LogTemp, Log, TEXT("current magazine start delegate bound? %u"), AmmoSysComp->MagazineStartDelegate.IsBound());
	
		// Magazine bullets-in delegate (start of wind-down animation)
		AmmoSysComp->MagazineBulletsInDelegate.AddDynamic(this, &USCrosshair::K2_OnMagazineReloadBulletsIn);
		UE_LOG(LogTemp, Log, TEXT("current magazine bullets-in delegate bound? %u"), AmmoSysComp->MagazineBulletsInDelegate.IsBound());
	
		// Magazine complete delegate (start of fade-out animation)
		AmmoSysComp->MagazineCompleteDelegate.AddDynamic(this, &USCrosshair::K2_OnMagazineReloadComplete);
		UE_LOG(LogTemp, Log, TEXT("current magazine complete delegate bound? %u"), AmmoSysComp->MagazineCompleteDelegate.IsBound());
	
		// Magazine cancel delegate (for reload cancel animation to be triggered)
		AmmoSysComp->MagazineCancelDelegate.AddDynamic(this, &USCrosshair::K2_OnMagazineReloadCancelled);
		UE_LOG(LogTemp, Log, TEXT("current magazine cancel delegate bound? %u"), AmmoSysComp->MagazineCancelDelegate.IsBound());
		
	}
	
}


// Auxiliary function for binding all per-bullet reload system delegates
void USCrosshair::BindPerBulletReloadDelegates(USAmmoSystemComponent* AmmoSysComp) {

	if (AmmoSysComp) {
		
		// Bullets added delegate (for +x pop-up once bullets-in)
		AmmoSysComp->BulletsAddedDelegate.AddDynamic(this, &USCrosshair::K2_OnBulletsAdded);
		UE_LOG(LogTemp, Log, TEXT("current bullets added delegate bound? %u"), AmmoSysComp->BulletsAddedDelegate.IsBound());

		// Per-bullet start delegate (start of readying-up animation)
		AmmoSysComp->PerBulletStartDelegate.AddDynamic(this, &USCrosshair::K2_OnPerBulletReloadStarted);
		UE_LOG(LogTemp, Log, TEXT("current per-bullet start delegate bound? %u"), AmmoSysComp->PerBulletStartDelegate.IsBound());

		// Per-bullet reload cycle start delegate (looping section)
		AmmoSysComp->PerBulletReloadCycleStartDelegate.AddDynamic(this, &USCrosshair::K2_OnPerBulletReloadCycleStarted);
		UE_LOG(LogTemp, Log, TEXT("current per-bullet reload cycle start delegate bound? %u"), AmmoSysComp->PerBulletReloadCycleStartDelegate.IsBound());
		
		// Per-bullet timed cancel delegate (start of wind-down animation)
		AmmoSysComp->PerBulletTimedCancelDelegate.AddDynamic(this, &USCrosshair::K2_OnPerBulletCancelledWithTime);
		UE_LOG(LogTemp, Log, TEXT("current per-bullet timed cancel delegate bound? %u"), AmmoSysComp->PerBulletTimedCancelDelegate.IsBound());

		// Per-bullet complete delegate (successful timed cancel)
		AmmoSysComp->PerBulletCompleteDelegate.AddDynamic(this, &USCrosshair::K2_OnPerBulletComplete);
		UE_LOG(LogTemp, Log, TEXT("current per-bullet complete delegate bound? %u"), AmmoSysComp->PerBulletCompleteDelegate.IsBound());
		
		// Per-bullet instant cancel delegate (for reload cancel animation to be triggered)
		AmmoSysComp->PerBulletInstantCancelDelegate.AddDynamic(this, &USCrosshair::K2_OnPerBulletCancelledInstant);
		UE_LOG(LogTemp, Log, TEXT("current per-bullet instant cancel delegate bound? %u"), AmmoSysComp->PerBulletInstantCancelDelegate.IsBound());
		
	}
	
}
