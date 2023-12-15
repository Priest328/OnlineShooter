#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OS_PowerUpActor.generated.h"

UCLASS()
class ONLINESHOOTER_API AOS_PowerUpActor : public AActor
{
	GENERATED_BODY()

public:
	AOS_PowerUpActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnTickPowerUp();

	UFUNCTION()
	void ActivatePowerUp(AActor* ActivateFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnActivated(AActor* ActivateFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerupTicked();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnExpired();

	UFUNCTION()
	void OnRep_PowerupActivate();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpStateChanged(bool bIsActive);

public:
	// Time between powerup ticks
	UPROPERTY(EditDefaultsOnly,  Category = "PowerUps" )
	float PowerUpInterval = 0.0f;

	// Total times we apply the powerups
	UPROPERTY(EditDefaultsOnly,  Category = "PowerUps" )
	int32 TotalNrOfTicks = 0;

	// Total number of ticks applied
	int32 TicksProcessed = 0;

	// Keeps the state of PowerUp
	UPROPERTY(ReplicatedUsing = OnRep_PowerupActivate)
	bool bIsPowerUpActive = false;
private:
	
	FTimerHandle TimerHandle_PowerUpTick;

};
