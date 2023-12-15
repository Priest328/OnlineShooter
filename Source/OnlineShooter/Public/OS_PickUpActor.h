#pragma once

#include "CoreMinimal.h"
#include "OS_PowerUpActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "OS_PickUpActor.generated.h"

UCLASS()
class ONLINESHOOTER_API AOS_PickUpActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	AOS_PickUpActor();
protected:
	
	virtual void BeginPlay() override;

public:
 
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
protected:

	void Respawn();

public:
	
	UPROPERTY(EditInstanceOnly, Category = "PickUpActor")
	float CooldownDuration = 10.0f;
	
protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UDecalComponent* DecalComp = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "PickUpActor")
	TSubclassOf<AOS_PowerUpActor> PowerUpClass = nullptr;

	UPROPERTY()
	AOS_PowerUpActor* PowerUpInstance = nullptr;





	
private:

	FTimerHandle TimerHandle_RespawnTimer;
};
