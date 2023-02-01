// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "MHPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIHORROR_API AMHPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	AMHPlayerState();

	/** Our ability system */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UMHAttributeSet* AttributeSetBase;


	
	
	FDelegateHandle HealthChangedDelegateHandle;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UMHAttributeSet* GetAttributeSetBase() const;


	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;
	

	
};
