// Fill out your copyright notice in the Description page of Project Settings.


#include "MHCharacter.h"

#include <string>

#include "MHPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"


AMHCharacter::AMHCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UMHCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	//Do normal stuff in constructor...
	PrimaryActorTick.bCanEverTick = true;

	bCanRotate = true;
}

// Called when the game starts or when spawned
void AMHCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMHCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
	UMHCharacterMovementComponent* MoveComp = Cast<UMHCharacterMovementComponent>(GetCharacterMovement());

	if(IsValid(MoveComp))
	{
		MoveComp->bIgnoreClientMovementErrorChecksAndCorrection = true;
		MoveComp->bServerAcceptClientAuthoritativePosition = true;
	}
}

void AMHCharacter::AddControllerYawInput(float Val)
{
	if(bCanRotate)
	{
		Super::AddControllerYawInput(Val);
	}
}

void AMHCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AMHPlayerState* PS = GetPlayerState<AMHPlayerState>();
	if(PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = PS->GetAbilitySystemComponent();

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set our PS
		MHPlayerState = PS;

		InitializeAttributes();
	}

	OnAbilitySystemComponentReady();
	
}

void AMHCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AMHPlayerState* PS = GetPlayerState<AMHPlayerState>();
	if(PS)
	{

		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = PS->GetAbilitySystemComponent();

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		
		// Set our PS
		MHPlayerState = PS;
	}
}

UAbilitySystemComponent* AMHCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMHCharacter::SetAllowRotationInternal(bool ShouldEnable, const FRotator& NewRotation)
{
	bCanRotate = ShouldEnable;

	if(!bCanRotate)
	{
		if(IsValid(Controller))
		{
			Controller->SetControlRotation(NewRotation);
		}
	}
}

void AMHCharacter::SetAllowMovementInternal(bool ShouldEnable, const FVector& NewLocation)
{
	UMHCharacterMovementComponent* CMC = Cast<UMHCharacterMovementComponent>(GetCharacterMovement());

	if(IsValid(CMC))
	{
		if(ShouldEnable)
		{
			CMC->SetMovementMode(MOVE_Walking);
		}
		else
		{
			CMC->SetMovementMode(MOVE_None);
			SetActorLocation(NewLocation);
		}
	}
}

void AMHCharacter::InitializeAttributes()
{
	if(DefaultAttributes)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}
}

// Called every frame
void AMHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
}

void AMHCharacter::StartSprinting()
{
	UMHCharacterMovementComponent* MoveComp = Cast<UMHCharacterMovementComponent>(GetCharacterMovement());

	if(MoveComp)
	{
		MoveComp->SetSprinting(true);
	}
}

void AMHCharacter::StopSprinting()
{
	UMHCharacterMovementComponent* MoveComp = Cast<UMHCharacterMovementComponent>(GetCharacterMovement());

	if(MoveComp)
	{
		MoveComp->SetSprinting(false);
	}
}

void AMHCharacter::StartSneaking()
{
	UMHCharacterMovementComponent* MoveComp = Cast<UMHCharacterMovementComponent>(GetCharacterMovement());

	if(MoveComp)
	{
		MoveComp->SetSneaking(true);
	}
}

void AMHCharacter::StopSneaking()
{
	UMHCharacterMovementComponent* MoveComp = Cast<UMHCharacterMovementComponent>(GetCharacterMovement());

	if(MoveComp)
	{
		MoveComp->SetSneaking(false);
	}

	
	
}

void AMHCharacter::ServerSetAllowRotation(bool ShouldEnable, const FRotator& NewRotation)
{
	SetAllowRotationInternal(ShouldEnable, NewRotation);

	ClientSetAllowRotation(ShouldEnable, NewRotation);
}

void AMHCharacter::ServerSetAllowMovement(bool ShouldEnable, const FVector& NewLocation)
{
	SetAllowMovementInternal(ShouldEnable, NewLocation);

	ClientSetAllowMovement(ShouldEnable, NewLocation);
}

void AMHCharacter::GetPing()
{
	APlayerState* playerState = GetPlayerState();
	if(IsValid(playerState))
	{
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Magenta, FString::FromInt(playerState->GetPingInMilliseconds()));
	}
}

void AMHCharacter::ClientSetAllowMovement_Implementation(bool ShouldEnable, const FVector& NewLocation)
{
	SetAllowMovementInternal(ShouldEnable, NewLocation);
}

void AMHCharacter::GrantAbility(TSubclassOf<UGameplayAbility> Ability, int InputID)
{
	if(AbilitySystemComponent)
	{
		if(HasAuthority() && Ability)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability.GetDefaultObject(), 1, InputID));
		}
		AMHPlayerState* PS = GetPlayerState<AMHPlayerState>();
		if(PS)
		{
			PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		}
	}
}

float AMHCharacter::GetHealth()
{
	if(IsValid(MHPlayerState))
	{
		return MHPlayerState->GetHealth();
	}
	return -1.0f;
}

float AMHCharacter::GetMaxHealth()
{
	if(IsValid(MHPlayerState))
	{
		return MHPlayerState->GetMaxHealth();
	}
	return -1.0f;
}

void AMHCharacter::ClientSetAllowRotation_Implementation(bool ShouldEnable, const FRotator& NewRotation)
{
	SetAllowRotationInternal(ShouldEnable, NewRotation);
}
