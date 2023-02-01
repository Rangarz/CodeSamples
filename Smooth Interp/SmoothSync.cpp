// Copyright Snowcastle Games 2022


#include "SmoothSync.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USmoothSync::USmoothSync()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	SetIsReplicatedByDefault(true);
}


void USmoothSync::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(USmoothSync, Location);
	DOREPLIFETIME(USmoothSync, Rotation);
}
// Called when the game starts
void USmoothSync::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USmoothSync::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!IsValid(GetOwner()))
	{
		return;
	}
	
	if(GetOwnerRole() == ROLE_Authority && AllowReplication)
	{
		TimeSinceLastUpdate += DeltaTime;

		if(TimeSinceLastUpdate >= 1.0f/UpdateRate)
		{
			TimeSinceLastUpdate -= 1.0f/UpdateRate;
			
			//Send Vectors to be Synced

			switch (ReplicationType)
			{
			case EReplicationType::LocationOnly:
				Location = GetOwner()->GetActorLocation();
				break;
			case EReplicationType::RotationOnly:
				Rotation = GetOwner()->GetActorRotation();
				break;
			case EReplicationType::LocationAndRotation:
				Location = GetOwner()->GetActorLocation();
				Rotation = GetOwner()->GetActorRotation();
				break;
			default:
				break;
			}
		
			GetOwner()->ForceNetUpdate();
		}
	}
	else
	{
		if(ShouldStartInterpolating)
		{
			//Start Sync
			switch(ReplicationType)
			{
			case EReplicationType::LocationOnly:
				GetOwner()->SetActorLocation(FMath::Lerp(GetOwner()->GetActorLocation(), Location, Speed * DeltaTime));
				break;
			case EReplicationType::RotationOnly:
				GetOwner()->SetActorRotation(FMath::Lerp(GetOwner()->GetActorRotation(), Rotation, Speed * DeltaTime));
				break;
			case EReplicationType::LocationAndRotation:
				GetOwner()->SetActorLocation(FMath::Lerp(GetOwner()->GetActorLocation(), Location, Speed * DeltaTime));
				GetOwner()->SetActorRotation(FMath::Lerp(GetOwner()->GetActorRotation(), Rotation, Speed * DeltaTime));
				break;
			default:
				break;
			}
		}
	}
}

void USmoothSync::OnLocationUpdated()
{
	ShouldStartInterpolating = true;
}

