// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUserWidget.generated.h"

/**
 * 
 */
class UButton;
class UTextBlock;

UCLASS()
class ONLINESHOOTER_API UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* StartGameButton = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* QuitGameButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StartGameTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuitGameTextBlock;
};
