// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "MHCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class MULTIHORROR_API UMHCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	friend class FSavedMove_MyMovement;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;


	UPROPERTY(EditAnywhere, Category = "Sprint")
	float SprintSpeedMultiplier = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Sprint")
	float SprintAccelerationMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Sneak")
	float SneakSpeedMultiplier = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Sprint")
	float SneakAccelerationMultiplier = 0.5f;

	///@brief Activate or deactivate sprint.
	void SetSprinting(bool bSprinting);

	///@brief Activate or deactivate sneak.
	void SetSneaking(bool bSneaking);

	///@brief Flag for activating sprint.
	uint8 bWantsToSprint : 1;

	///@brief Flag for activating sneak.
	uint8 bWantsToSneak : 1;

	///@brief Override maximum speed during sprint or sneak.
	virtual float GetMaxSpeed() const override;
	///@brief Override maximum acceleration for sprint or sneak.
	virtual float GetMaxAcceleration() const override;

	///@return Whether or not the character is currently moving in a forward direction.
	bool IsMovingForward() const;

};

class FSavedMove_MyMovement: public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	///@brief Resets all saved variables.
	virtual void Clear() override;

	///@brief Store input commands in the compressed flags.
	virtual uint8 GetCompressedFlags() const override;

	///@brief This is used to check whether or not two moves can be combined into one.
	///Basically you just check to make sure that the saved variables are the same.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	///@brief Sets up the move before sending it to the server. 
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	/////@brief Sets variables on character movement component before making a predictive correction.
	virtual void PrepMoveFor(class ACharacter* Character) override;

	uint8 bSavedWantsToSprint : 1;
	uint8 bSavedWantsToSneak : 1;
};

class FNetworkPredictionData_Client_MyMovement : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_MyMovement(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	///@brief Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};
