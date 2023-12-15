// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuUserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ONLINESHOOTER_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ShowMainMenu();

	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void QuitGame();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UMainMenuUserWidget> UserMainMenuWidget = nullptr;

	UPROPERTY()
	UMainMenuUserWidget* MainMenuWidget = nullptr;
};
