// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EGameArea : uint8
{
	None,
	Area1 UMETA(DisplayName = "Area1"),
	Area2 UMETA(DisplayName = "Area2"),
	Area3 UMETA(DisplayName = "Area3"),
	Exit UMETA(DisplayName = "Exit"),

};

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	None,
	Lobby UMETA(DisplayName = "Lobby"),
	//AssignInfectedPhase UMETA(DisplayName = "TeamSelection"),
	DayPhase UMETA(DisplayName = "DayPhase"),
	NightPhase UMETA(DisplayName = "NightPhase"),
	TrapIn UMETA(DisplayName = "TrapIn"),
	GameEnd UMETA(DisplayName = "GameEnd"),
};

UENUM(BlueprintType)
enum class EGameResult : uint8
{
	None,
	CitizenWin UMETA(DisplayName = "CitizenWin"),
	InfectedWin  UMETA(DisplayName = "InfectedWin"),
};
