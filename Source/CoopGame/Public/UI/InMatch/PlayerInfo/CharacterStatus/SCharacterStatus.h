// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SCharacterStatus.generated.h"



class UBorder;
class UProgressBar;
class UTextBlock;



/**
 *
 *
 * 
 */
UCLASS()
class COOPGAME_API USCharacterStatus : public UUserWidget {
	
	GENERATED_BODY()


public:
	// First operations upon construction
	virtual void NativeConstruct() override;

	// Ticks widget, used for interpolation of HP increase/loss as animation
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	// Function called for initial setup of progress bars
	UFUNCTION()
	virtual void OnCharacterSpawnComplete(float MaximumHP, float MaximumATP);
	
	// Function called once the current HP of the character is modified
	UFUNCTION()
	virtual void OnHPChanged(class USAttributesComponent* AttrComp, float CurrentHP, float DeltaHP
	, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// Function called once the player that owns this widget dies
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void K2_OnPlayerDeath(float DeathAnimTime);

	
protected:
	// HP Widgets

	// Texture for the frame of the HP bar
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* HPFrame;

	// Represents how much HP the local player-owned character has as a fraction of its maximum
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HPBar;

	// Represents how much HP the local player-owned character has as a numerical value
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HPNumber;


	// ATP Widgets

	// Texture for the frame of the ATP bar
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* ATPFrame;

	// Represents how much ATP the local player-owned character has as a fraction of its maximum
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ATPBar;

	// Represents how much ATP the local player-owned character has as a numerical value
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ATPNumber;

	
	// General

	// Process the values for the HP indicators to be displayed every frame
	virtual void ProcessHP(float DeltaTime);

	// Process the values for the ATP indicators to be displayed every frame
	virtual void ProcessATP(float DeltaTime);

	// The speed at which the HP and ATP loss is animated per Tick
	UPROPERTY(EditDefaultsOnly, Category = "Display", meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float StatDrainAnimSpeed;
	
	
private:
	// Basic function that generates an FText using the current and maximum HP/ATP values
	FText GenerateBarText(float Current, float Max);
	
	// Maximum HP for the character for whom this widget is displayed
	float MaximumHP;

	// The actual current HP of the character for whom this widget is displayed, used as a target for interpolation animations
	float TrueCurrentHP;
	
	// The current HP of the character for whom this widget is displayed as displayed through the animation, lags behind the actual current HP upon change
	float AnimCurrentHP;

	// Maximum ATP for the character for whom this widget is displayed
	float MaximumATP;

	// The actual current ATP of the character for whom this widget is displayed, used as a target for interpolation animations
	float TrueCurrentATP;
	
	// The current ATP of the character for whom this widget is displayed as displayed through the animation, lags behind the actual current HP upon change
	float AnimCurrentATP;
	
};
