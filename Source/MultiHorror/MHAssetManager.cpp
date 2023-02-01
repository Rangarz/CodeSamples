// Fill out your copyright notice in the Description page of Project Settings.


#include "MHAssetManager.h"

#include "AbilitySystemGlobals.h"

void UMHAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Needed to use TargetData with GAS
	UAbilitySystemGlobals::Get().InitGlobalData();
}
