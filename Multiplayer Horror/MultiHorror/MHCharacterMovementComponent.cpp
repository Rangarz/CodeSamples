// Fill out your copyright notice in the Description page of Project Settings.


#include "MHCharacterMovementComponent.h"

//============================================================================================
//Replication
//============================================================================================

//Set input flags on character from saved inputs
void UMHCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)//Client only
{
	Super::UpdateFromCompressedFlags(Flags);

	//The Flags parameter contains the compressed input flags that are stored in the saved move.
	//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	//It basically just resets the movement component to the state when the move was made so it can simulate from there.
	bWantsToSprint = (Flags&FSavedMove_Character::FLAG_Custom_0) != 0;

	bWantsToSneak = (Flags&FSavedMove_Character::FLAG_Custom_1) != 0;
}

void UMHCharacterMovementComponent::SetSprinting(bool bSprinting)
{
	bWantsToSprint = bSprinting;
}

void UMHCharacterMovementComponent::SetSneaking(bool bSneaking)
{
	bWantsToSneak = bSneaking;
}

float UMHCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	if (bWantsToSprint && IsMovingForward())
	{
		MaxSpeed *= SprintSpeedMultiplier;
	}
	else if(bWantsToSneak)
	{
		MaxSpeed *= SneakSpeedMultiplier;
	}

	return MaxSpeed;
}

float UMHCharacterMovementComponent::GetMaxAcceleration() const
{
	float MaxAccel = Super::GetMaxAcceleration();

	if (bWantsToSprint && IsMovingForward())
	{
		MaxAccel *= SprintAccelerationMultiplier;
	}
	else if(bWantsToSneak)
	{
		MaxAccel *= SneakAccelerationMultiplier;
	}

	return MaxAccel;
}

bool UMHCharacterMovementComponent::IsMovingForward() const
{
	if (!PawnOwner)
	{
		return false;
	}

	FVector Forward = PawnOwner->GetActorForwardVector();
	FVector MoveDirection = Velocity.GetSafeNormal();

	//Ignore vertical movement
	Forward.Z = 0.0f;
	MoveDirection.Z = 0.0f;

	float VelocityDot = FVector::DotProduct(Forward, MoveDirection);
	return VelocityDot > 0.7f;//Check to make sure difference between headings is not too great.
}

class FNetworkPredictionData_Client* UMHCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);
	//check(PawnOwner->GetLocalRole() < ROLE_Authority);
	if(PawnOwner->GetLocalRole() >= ROLE_Authority)
	{
		return nullptr;
	}

	if (!ClientPredictionData)
	{
		UMHCharacterMovementComponent* MutableThis = const_cast<UMHCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_MyMovement(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void FSavedMove_MyMovement::Clear()
{
	Super::Clear();


	//Clear variables back to their default values.
	bSavedWantsToSprint = false;

	bSavedWantsToSneak = false;
	
}

uint8 FSavedMove_MyMovement::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}

	if(bSavedWantsToSneak)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}


bool FSavedMove_MyMovement::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{

	//This pretty much just tells the engine if it can optimize by combining saved moves. There doesn't appear to be
	//any problem with leaving it out, but it seems that it's good practice to implement this anyways.
	if (bSavedWantsToSprint != ((FSavedMove_MyMovement*)&NewMove)->bSavedWantsToSprint)
	{
		return false;
	}

	if (bSavedWantsToSneak != ((FSavedMove_MyMovement*)&NewMove)->bSavedWantsToSneak)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_MyMovement::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UMHCharacterMovementComponent* CharMov = Cast<UMHCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharMov)
	{
		//This is literally just the exact opposite of UpdateFromCompressed flags. We're taking the input
		//from the player and storing it in the saved move.
		bSavedWantsToSprint = CharMov->bWantsToSprint;

		bSavedWantsToSneak = CharMov->bWantsToSneak;
	}
}

void FSavedMove_MyMovement::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UMHCharacterMovementComponent* CharMov = Cast<UMHCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharMov)
	{
		
	}
}

FNetworkPredictionData_Client_MyMovement::FNetworkPredictionData_Client_MyMovement(const UCharacterMovementComponent& ClientMovement)
: Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_MyMovement::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_MyMovement());
}