// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Characters/TargetDummy.h"
#include "Gameplay/Characters/Components/SAttributesComponent.h"



// Sets default values
ATargetDummy::ATargetDummy() {

	// Create attributes component
	StatsComp = CreateDefaultSubobject<USAttributesComponent>(TEXT("StatsComp"));
	
}


// Called when the game starts or when spawned
void ATargetDummy::BeginPlay() {
	
	Super::BeginPlay();
	
}

