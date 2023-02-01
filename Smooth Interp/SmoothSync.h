// Copyright Snowcastle Games 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SmoothSync.generated.h"



UENUM(Blueprintable)
enum class EReplicationType : uint8
{
	LocationOnly,
	RotationOnly,
	LocationAndRotation
};
	


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CASTAWAY_API USmoothSync : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USmoothSync();

	//This is a server only property. Clients will await replication by default, but server can choose to send or not as it wish.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AllowReplication = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EReplicationType ReplicationType = EReplicationType::LocationAndRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldOptimize = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int UpdateRate = 10;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(ReplicatedUsing=OnLocationUpdated)
	FVector Location;

	UPROPERTY(replicated)
	FRotator Rotation;

	float TimeSinceLastUpdate = 0.0f;

	bool ShouldStartInterpolating = false;

	UFUNCTION()
	void OnLocationUpdated();
};
