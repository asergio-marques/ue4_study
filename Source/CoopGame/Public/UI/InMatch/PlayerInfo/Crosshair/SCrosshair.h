// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Weapons/Types/Shooting/SShootingWeapon.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "SCrosshair.generated.h"



class UImage;
class UProgressBar;
class UTextBlock;
class ASWeapon;



/**
 *
 *
 * 
 */
UCLASS()
class COOPGAME_API USCrosshair : public UUserWidget {
	
	GENERATED_BODY()


public:
	// First operations upon construction
	virtual void NativeConstruct() override;

	
	// General
	
	// Replaces the reticule being displayed and stores the reload type of the new active weapon
	void OnWeaponChange(ASWeapon* oldCurrentWeapon, ASWeapon* newCurrentWeapon);

	// Function called once the player that owns this widget dies
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void K2_OnPlayerDeath(float DeathAnimTime);

	// Blueprint function called every time passive or per-bullet reload adds bullets to the currently available ones, activates the BulletIncreaseText widget
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnBulletsAdded(uint8 NumBulletsAdded);

	// Passive reload
	
	// Blueprint function called every time passive reload is triggered, activates the Delay animation and then the BulletsIn animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPassiveReloadStarted(float FirstDelay);

	// Blueprint function called every time a new assive reload cycle is triggered, activation the BulletsIn animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPassiveReloadCycleStarted(float BulletsInDelay);
	
	// Blueprint function called when passive reload is cancelled, playing the CancelIndication and then the FadeOut animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPassiveReloadCancelled(UReloadCancelTrigger CancelType);

	
	// Magazine reload
	
	// Blueprint function called when magazine reload is triggered, playing the BulletsIn animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnMagazineReloadStarted(float TimeToBulletsIn);
	
	// Blueprint function called when magazine reload has inserted its bullets, playing the Complete animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnMagazineReloadBulletsIn(float TimeToCompletion);
	
	// Blueprint function called when magazine reload is complete, playing the FadeOut animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnMagazineReloadComplete();
	
	// Blueprint function called when magazine reload is cancelled with wind-down, playing the CancelIndication and then the FadeOut animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnMagazineReloadCancelled();

	
	// Per-bullet reload
	
	// Blueprint function called every time per-bullet reload is triggered, activates the Delay animation and then the BulletsIn animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPerBulletReloadStarted(float FirstDelay);

	// Blueprint function called every time a new per-bullet reload cycle is triggered, activation the BulletsIn animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPerBulletReloadCycleStarted(float BulletsInDelay);
	
	// Blueprint function called when per-bullet reload is cancelled with wind-down, playing the Complete animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPerBulletCancelledWithTime(float TimeToCompletion);

	// Blueprint function called when per-bullet reload is completed (player-induced cancel finished), playing the FadeOut animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPerBulletComplete();
	
	// Blueprint function called when per-bullet reload is cancelled without wind-down, playing the CancelIndication and then the FadeOut animation
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnPerBulletCancelledInstant();

	
protected:
	// Reticule

	// Texture for the reticule that represents the character's aim
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ReticuleWidget;

	
	// Reload indicators

	// General

	// Pointer to image object that is displayed when reload is cancelled
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ReloadCancelledIcon;

	// Pointer to image object that is displayed when reload is completed
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ReloadCompleteIcon;
	
	// Passive reload
	
    // Progress bar specific to passive reload indication that is filled as the initial delay period passes 
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* PassiveReloadIndicator;

	// Progress bar specific for passive reload indication that is filled successively as the bullet-in period passes
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* PassiveBulletsInBar;


	// Magazine reload
	
	// Basic texture for signalling that a magazine reload is taking place
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* MagazineReloadIcon;
	
	// Progress bar specific for magazine reload indication that is filled as the bullet-in period passes
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* MagazineBulletsInBar;

	// Progress bar specific for magazine reload indication that is filled as the completion period passes
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* MagazineCompleteBar;

	
	// Per-bullet reload
	
	//Progress bar specific to per-bullet reload indication that is filled as the initial delay period passes 
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* PerBulletIcon;
	
	// Progress bar specific for per-bullet reload indication that is filled successively as the bullet-in period passes
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* PerBulletBulletsInBar;

	// Progress bar specific for per-bullet reload indication that is filled as the completion period passes
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* PerBulletCompleteBar;

	
private:
	// Auxiliary function for hiding all reload-indicating widgets
	void HideAllReloadWidgets();

	// Auxiliary function for processing the unbinding of the previous weapon's reload system delegates
	void UnbindReloadDelegates(USAmmoSystemComponent* AmmoSysComp);
	
	// Auxiliary function for unbinding all passive reload system delegates
	void UnbindPassiveReloadDelegates(USAmmoSystemComponent* AmmoSysComp);

	// Auxiliary function for unbinding all magazine reload system delegates
	void UnbindMagazineReloadDelegates(USAmmoSystemComponent* AmmoSysComp);

	// Auxiliary function for unbinding all per-bullet reload system delegates
	void UnbindPerBulletReloadDelegates(USAmmoSystemComponent* AmmoSysComp);

	// Auxiliary function for processing of new weapon reload type and binding of appropriate delegates
	void BindReloadDelegates(USAmmoSystemComponent* AmmoSysComp);

	// Auxiliary function for binding all passive reload system delegates
	void BindPassiveReloadDelegates(USAmmoSystemComponent* AmmoSysComp);

	// Auxiliary function for binding all magazine reload system delegates
	void BindMagazineReloadDelegates(USAmmoSystemComponent* AmmoSysComp);

	// Auxiliary function for binding all per-bullet reload system delegates
	void BindPerBulletReloadDelegates(USAmmoSystemComponent* AmmoSysComp);

	// Reload type of the current weapon; this is written only at Weapon Change
	UReloadType CurrentWeaponReloadType;
	
};
