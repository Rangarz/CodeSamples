// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "MHCharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "MHCharacter.generated.h"


//Example for an enum the FGameplayAbiliyInputBinds may use to map input to ability slots.
//It's very important that this enum is UENUM, because the code will look for UENUM by the given name and crash if the UENUM can't be found. BlueprintType is there so we can use these in blueprints, too. Just in case. Can be neat to define ability packages.
UENUM(BlueprintType) 
enum class AbilityInput : uint8
{
	UseAbility1 UMETA(DisplayName = "Use Spell 1"), //This maps the first ability(input ID should be 0 in int) to the action mapping(which you define in the project settings) by the name of "UseAbility1". "Use Spell 1" is the blueprint name of the element.
	UseAbility2 UMETA(DisplayName = "Use Spell 2"), //Maps ability 2(input ID 1) to action mapping UseAbility2. "Use Spell 2" is mostly used for when the enum is a blueprint variable.
	UseAbility3 UMETA(DisplayName = "Use Spell 3"),
	UseAbility4 UMETA(DisplayName = "Use Spell 4"),
	WeaponAbility UMETA(DisplayName = "Use Weapon"), //This finally maps the fifth ability(here designated to be your weaponability, or auto-attack, or whatever) to action mapping "WeaponAbility".
	//You may also do something like define an enum element name that is not actually mapped to an input, for example if you have a passive ability that isn't supposed to have an input. This isn't usually necessary though as you usually grant abilities via input ID,
	//which can be negative while enums cannot. In fact, a constant called "INDEX_NONE" exists for the exact purpose of rendering an input as unavailable, and it's simply defined as -1.
	//Because abilities are granted by input ID, which is an int, you may use enum elements to describe the ID anyway however, because enums are fancily dressed up ints.
};


UCLASS()
class MULTIHORROR_API AMHCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMHCharacter(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	virtual void AddControllerYawInput(float Val) override;

	//Server only
	virtual void PossessedBy(AController* NewController) override;

	//Client only
	virtual void OnRep_PlayerState() override;

private:

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class AMHPlayerState* MHPlayerState;
	
	bool bCanRotate = true;

	void SetAllowRotationInternal(bool ShouldEnable, const FRotator& NewRotation);

	void SetAllowMovementInternal(bool ShouldEnable, const FVector& NewLocation);

	void InitializeAttributes();

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintCallable)
	void StartSprinting();
	UFUNCTION(BlueprintCallable)
	void StopSprinting();

	UFUNCTION(BlueprintCallable)
	void StartSneaking();
	UFUNCTION(BlueprintCallable)
	void StopSneaking();


	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void ServerSetAllowRotation(bool ShouldEnable, const FRotator& NewRotation);
	

	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void ServerSetAllowMovement(bool ShouldEnable, const FVector& NewLocation);

	
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientSetAllowRotation(bool ShouldEnable, const FRotator& NewRotation);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientSetAllowMovement(bool ShouldEnable, const FVector& NewLocation);
	

	UFUNCTION(BlueprintCallable)
	void GrantAbility(TSubclassOf<class UGameplayAbility> Ability, int InputID = 0);


	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilitySystemComponentReady();



	//Attributes
	UFUNCTION(BlueprintCallable)
	float GetHealth();
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float CurrentHealth);
	

	//Commands
	UFUNCTION(Exec)
	void GetPing();

	UFUNCTION(Exec, BlueprintImplementableEvent)
	void SetInvincible(bool IsInvincible);

	UFUNCTION(Exec, BlueprintImplementableEvent)
	void BurnItem();

	UFUNCTION(Exec, BlueprintImplementableEvent)
	void Respawn();
	
	UFUNCTION(Exec, BlueprintImplementableEvent)
	void DestroyAllMinions();

	UFUNCTION(Exec, BlueprintImplementableEvent)
	void ReviveMe();
};
