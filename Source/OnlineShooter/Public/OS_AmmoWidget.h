// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OS_AmmoWidget.generated.h"

/**
 * 
 */


class UTextBlock;

UCLASS()
class ONLINESHOOTER_API UOS_AmmoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void UpdateWidgetAmount(int32 CurrentAmmoAmount,int32 MaxAmmoAmount);
	
	UPROPERTY(EditAnywhere, meta =(BindWidget))
	UTextBlock* MaxAmmoAmount_TextBlock = nullptr;
	
	UPROPERTY(EditAnywhere, meta =(BindWidget))
	UTextBlock* CurrentAmmoAmount_TextBlock = nullptr;

};
