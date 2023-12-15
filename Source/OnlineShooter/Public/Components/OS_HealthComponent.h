// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OS_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UOS_HealthComponent*, HealthComp, float, Health,
                                             float, HealthDelta, const class UDamageType*, DamageType,
                                             class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONLINESHOOTER_API UOS_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOS_HealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "HealthComponent")
	uint8 TeamNum = 255;

	UFUNCTION()
	void HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	                      class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	static bool IsFriendly(AActor* ActorA, AActor* AActorB);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Health(float OldHealth);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChangedDelegate;

protected:
	UPROPERTY(ReplicatedUsing =OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	float Health = 0.0f;

	bool bIsDead = false;
};
