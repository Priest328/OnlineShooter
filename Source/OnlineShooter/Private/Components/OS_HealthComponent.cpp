#include "Components/OS_HealthComponent.h"

#include "OS_GameMode.h"
#include "Net/UnrealNetwork.h"



UOS_HealthComponent::UOS_HealthComponent()
{
     SetIsReplicated(true);
}


void UOS_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hook in Server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UOS_HealthComponent::HandleTakeDamage);
		}
	}
	Health = DefaultHealth;
}

void UOS_HealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;
	OnHealthChangedDelegate.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}


void UOS_HealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
                                           class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	if (DamageCauser !=DamagedActor && IsFriendly(DamagedActor,DamageCauser))
	{
		return;
	}
	
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	bIsDead = Health <= 0;
	
	OnHealthChangedDelegate.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	//UE_LOG(LogTemp, Log, TEXT("Health: %f"), Health);

	AOS_GameMode* GameMode = Cast<AOS_GameMode>(GetWorld()->GetAuthGameMode());
	if (bIsDead)
	{
		if (GameMode)
		{
			GameMode->OnActorKilled_Delegate.Broadcast(GetOwner(),DamageCauser,InstigatedBy);
		}
	}
}

void UOS_HealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0 || Health <= 0)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Chamged: %s"), *FString::SanitizeFloat(Health));

	OnHealthChangedDelegate.Broadcast(this,Health,-HealAmount,nullptr,nullptr,nullptr);
	
}

float UOS_HealthComponent::GetHealth() const
{
	return Health;
}

bool UOS_HealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}
	
	UOS_HealthComponent* HealthCompA = Cast<UOS_HealthComponent>(ActorA->GetComponentByClass(UOS_HealthComponent::StaticClass()));
	UOS_HealthComponent* HealthCompB = Cast<UOS_HealthComponent>(ActorB->GetComponentByClass(UOS_HealthComponent::StaticClass()));
	
	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return true;
	}
	
	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}

void UOS_HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOS_HealthComponent, Health);
	//DOREPLIFETIME(UOS_HealthComponent, TeamNum);
}
