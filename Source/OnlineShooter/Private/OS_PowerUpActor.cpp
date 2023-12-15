#include "OS_PowerUpActor.h"

#include "Math/UnitConversion.h"
#include "Net/UnrealNetwork.h"

AOS_PowerUpActor::AOS_PowerUpActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
}

void AOS_PowerUpActor::BeginPlay()
{
	Super::BeginPlay();
}

void AOS_PowerUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOS_PowerUpActor::OnTickPowerUp()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNrOfTicks)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUpTick);
	}
}

void AOS_PowerUpActor::ActivatePowerUp(AActor* ActivateFor)
{
	OnActivated(ActivateFor);

	bIsPowerUpActive = true;
	OnRep_PowerupActivate();
	
	if (PowerUpInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUpTick, this, &AOS_PowerUpActor::OnTickPowerUp,
		                                PowerUpInterval,
		                                true);
	}
	else
	{
		OnTickPowerUp();
	}
}

void AOS_PowerUpActor::OnRep_PowerupActivate()
{
	OnPowerUpStateChanged(bIsPowerUpActive);
}

void AOS_PowerUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOS_PowerUpActor, bIsPowerUpActive);
}
