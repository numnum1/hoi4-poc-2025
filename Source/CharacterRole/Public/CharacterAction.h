// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterActionEvent);

/**
 * 
 */
UCLASS(BlueprintType, Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class CHARACTERROLE_API UCharacterAction : public UObject
{
	GENERATED_BODY()

public:
	UCharacterAction();

	UPROPERTY(BlueprintAssignable)
	FCharacterActionEvent OnChanged;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	FText ActionName = FText::FromString("");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	FSlateBrush Icon = FSlateNoResource();
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Click();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void Setup(const FText& NewActionName, const FSlateBrush& NewIcon)
	{
		ActionName = NewActionName;
		Icon = NewIcon;
		OnChanged.Broadcast();
	}
	
};
