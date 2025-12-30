// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/LightActor.h"

#include "Components/PointLightComponent.h"
#include "GameMode/Team10GameMode.h"
#include "GameState/Team10GameState.h"

// Sets default values
ALightActor::ALightActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	Light->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALightActor::BeginPlay()
{
	Super::BeginPlay();
	
	ATeam10GameState* Team10GameState = GetWorld()->GetGameState<ATeam10GameState>();
	if (!Team10GameState)
	{
		return;
	}

	Team10GameState->OnGamePhaseChanged.AddDynamic(this, &ALightActor::SwitchLight);
}

void ALightActor::SwitchLight(EGamePhase GamePhase)
{
	if (!Light)
	{
		return;
	}
	
	Light->SetVisibility(GamePhase == EGamePhase::DayPhase);
}

