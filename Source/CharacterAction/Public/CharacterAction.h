#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "CharacterAction.generated.h"

class FCharacterActionModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

class ARtsPlayerController;
class ARtsCharacter;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CHARACTERACTION_API UCharacterAction : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UCharacterAction();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ActionName;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSlateBrush Icon;
	
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Click(UObject* InRoleContext);
	
};