// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "SPlayerInfo.generated.h"



class USCharacterStatus;
class USPlayerLoadout;
class USAttributesComponent;


/**
 *
 *
 * 
 */
UCLASS()
class COOPGAME_API USPlayerInfo : public UUserWidget {
	
	GENERATED_BODY()

	
public:
	// 
	void Construct(USAttributesComponent* StatsComp);
	
	// 
	void OnLoadoutSpawned(FLoadoutInfo LoadoutInfo, uint8 InitialActiveWeaponSlotNumber);

	// 
	UFUNCTION()
	void OnWeaponChange(uint8 newSlot);

	// 
	void OnPlayerDeath(float DeathAnimTime);

	
protected:
	// Subclass for the character status widget
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<USCharacterStatus> CharStatusWidgetClass;

	// Subclass for the loadout display widget
	UPROPERTY(EditDefaultsOnly, Category = "Widget Classes")
	TSubclassOf<USPlayerLoadout> LoadoutWidgetClass;
	
	// Pointer to UMG widget that displays the HP and ATP stats of the player owned character
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USCharacterStatus* CharStatusWidget;
	
	// Pointer to UMG widget that displays stats for the owning play pawn's loadout
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USPlayerLoadout* LoadoutWidget;
	
};
