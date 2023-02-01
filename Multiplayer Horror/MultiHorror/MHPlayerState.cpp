// Fill out your copyright notice in the Description page of Project Settings.


#include "MHPlayerState.h"

#include "MHAttributeSet.h"
#include "MHCharacter.h"
#include "Components/SphereComponent.h"


AMHPlayerState::AMHPlayerState()
{
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystem->SetIsReplicated(true);


	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UMHAttributeSet>(TEXT("AttributeSetBase"));

	NetUpdateFrequency = 100.0f;
}

void AMHPlayerState::BeginPlay()
{
	Super::BeginPlay();


	if(AbilitySystem)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AMHPlayerState::HealthChanged);
	}
}

UAbilitySystemComponent* AMHPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

UMHAttributeSet* AMHPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

void AMHPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	AMHCharacter* Char = Cast<AMHCharacter>(GetPawn());
	if(IsValid(Char))
	{
		Char->OnHealthChanged(GetHealth());
	}
}

float AMHPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();	
}

float AMHPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}


