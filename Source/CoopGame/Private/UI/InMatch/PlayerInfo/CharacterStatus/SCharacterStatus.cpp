// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/InMatch/PlayerInfo/CharacterStatus/SCharacterStatus.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"



// First operations upon construction
void USCharacterStatus::NativeConstruct() {
	
	Super::NativeConstruct();

	StatDrainAnimSpeed = 10;
	
}


// Ticks widget, used for interpolation of HP increase/loss as animation
void USCharacterStatus::NativeTick(const FGeometry& MyGeometry, float DeltaTime) {
	
	Super::NativeTick(MyGeometry, DeltaTime);

	// Process bar and number values (for pseudo-animation)
	ProcessHP(DeltaTime);
	ProcessATP(DeltaTime);
	
}


void USCharacterStatus::OnCharacterSpawnComplete(float BaseHP, float BaseATP) {

	// Set bars and numbers to maximum always
	HPBar->SetPercent(1.0f);
	HPNumber->SetText(GenerateBarText(BaseHP, BaseHP));
	ATPBar->SetPercent(1.0f);
	ATPNumber->SetText(GenerateBarText(BaseATP, BaseATP));

	// Set internal variables
	MaximumHP = BaseHP;
	TrueCurrentHP = BaseHP;
	AnimCurrentHP = BaseHP;
	MaximumATP = BaseATP;
	TrueCurrentATP = BaseATP;
	AnimCurrentATP = BaseATP;
	
}


// Function called once the current HP of the character is modified
void USCharacterStatus::OnHPChanged(class USAttributesComponent* AttrComp, float CurrentHP, float DeltaHP
	, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {

	// only update the true HP, the bar value shall be adapted at Tick time
	TrueCurrentHP = CurrentHP;

}


// Basic function that generates an FText using the current and maximum HP/ATP values
FText USCharacterStatus::GenerateBarText(float Current, float Max) {

	FString tempCurrent = FString::SanitizeFloat(FMath::FloorToFloat(Current), 0);
	FString tempMax = FString::SanitizeFloat(FMath::FloorToFloat(Max), 0);
	FString tempFull = tempCurrent + " / " + tempMax;

	return FText::FromString(tempFull);
	
}


// Process the values for the HP indicators to be displayed every frame
void USCharacterStatus::ProcessHP(float DeltaTime) {

	// Check if we need to play a HP change animation, and if we do, play it
	if (TrueCurrentHP != AnimCurrentHP) {
		
		AnimCurrentHP = FMath::FInterpTo(AnimCurrentHP, TrueCurrentHP, DeltaTime, StatDrainAnimSpeed);
		HPNumber->SetText(GenerateBarText(AnimCurrentHP, MaximumHP));
		if (MaximumHP != 0.0f) {
			
			HPBar->SetPercent((AnimCurrentHP / MaximumHP));
			
		}
		else {
			
			HPBar->SetPercent(0.0f);
			UE_LOG(LogTemp, Error, TEXT("MaximumHP is 0!"));
			
		}
		
	}
	
}


// Process the values for the ATP indicators to be displayed every frame
void USCharacterStatus::ProcessATP(float DeltaTime) {

	// Check if we need to play a ATP change animation, and if we do, play it
	if (TrueCurrentATP != AnimCurrentATP) {
		
		AnimCurrentATP = FMath::FInterpTo(AnimCurrentATP, TrueCurrentATP, DeltaTime, StatDrainAnimSpeed);
		ATPNumber->SetText(GenerateBarText(AnimCurrentATP, MaximumATP));
		if (MaximumATP != 0.0f) {
			
			ATPBar->SetPercent((AnimCurrentATP / MaximumATP));
			
		}
		else {
			
			ATPBar->SetPercent(0.0f);
			UE_LOG(LogTemp, Error, TEXT("MaximumATP is 0!"));
			
		}
		
	}
	
}