#pragma once

#include "CoreMinimal.h"
#include "OS_GameState.h"
#include "OS_PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "OS_GameMode.generated.h"

enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor,
											   AController*, KillerController);

UCLASS()
class ONLINESHOOTER_API AOS_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOS_GameMode();

	virtual void StartPlay() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	//Start spawning bots
	void StartWave();

	//End spawning bots
	void EndWave();

	// Set time to next wave of bots
	void PrepareToNextWave();

	void SpawnBotTimerElapsed();

	void CheckAnyPlayerAlive();

	void GameOver();

	void CheckGameState();

	void SetWaveState(EWaveState NewState);

	void RestartAllPlayers();

	
public:
	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled_Delegate;

	// Amount of bots need to spawn in a current wave
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	int32 NumOfBotsToSpawn = 0;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float IntervalOfSpawningBots = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
    TSubclassOf<AOS_PlayerController> PlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves = 5.0f;

	int32 WaveCount = 0;

	void CheckWaveState();

private:
	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_NextWaveStart;
	FTimerHandle TimerHandle_CheckGameState;

	bool bIsGameOver = false;

	int32 NumOfSpawnedBots = 0.0;
};
