// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "SPlayerInMatchHUD.generated.h"



class USPlayerInfo;
class USCurrentWeaponStatus;
class USCrosshair;
class ASWeapon;



/**
 *
 *
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerInMatchHUD : public AHUD {

	GENERATED_BODY()


public:
	// Sets default values for this HUD's parameters
	ASPlayerInMatchHUD();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
	UFUNCTION()
	void OnLoadoutSpawned(FLoadoutInfo LoadoutInfo, ASWeapon* CurrentWeapon, uint8 InitialActiveWeaponSlotNumber);

	//
	UFUNCTION()
	void OnWeaponChange(ASWeapon* oldCurrentWeapon, ASWeapon* newCurrentWeapon, uint8 newSlot);

	// 
	UFUNCTION()
	void OnOwningCharacterDeath();

	// Subclass for the combined loadout + character status widget
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<USPlayerInfo> PlayerInfoWidgetClass;
	
	// Subclass for the current active weapon status display widget
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<USCurrentWeaponStatus> WeaponStatusWidgetClass;

	// Subclass for the crosshair widget to be displayed
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<USCrosshair> CrosshairWidgetClass;

	// Pointer to widget that displays the loadout, currently active weapon, and character status
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USPlayerInfo* PlayerInfoWidget;
	
	// Pointer to UMG widget that displays stats for the owning play pawn's current active weapon
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USCurrentWeaponStatus* WeaponStatusWidget;

	// Pointer to UMG widget that displays the crosshair and near-crosshair info
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USCrosshair* CrosshairWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters", meta = (ClampMin = 1.0, ClampMax = 10.0))
	float DeathAnimTime;
	
};
