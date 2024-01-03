// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/DeveloperSettings.h"

#include "AudioMixTypes.h"

#include "GameplayTagContainer.h"

#include "AudioDeveloperSettings.generated.h"


/**
 * Settings for a audio mix.
 */
UCLASS(Config = "Game", DefaultConfig, meta = (DisplayName = "Game Audio Core"))
class GACORE_API UAudioDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UAudioDeveloperSettings();

public:
	//
	// The Default Base Control Bus Mix
	//
	UPROPERTY(Config, EditAnywhere, Category = "MixSettings", meta = (ForceInlineRow, AllowedClasses = "/Script/AudioModulation.SoundControlBusMix"))
	TMap<FGameplayTag, FSoftObjectPath> ControlBusMixMap;

	//
	// Mapping list of ControlBus to be assigned
	//
	UPROPERTY(Config, EditAnywhere, Category = "MixSettings", meta = (ForceInlineRow, AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	TMap<FGameplayTag, FSoftObjectPath> ControlBusMap;

	//
	// Submix Processing Chains to achieve high dynamic range audio output
	//
	UPROPERTY(Config, EditAnywhere, Category = "MixSettings")
	TArray<FAudioSubmixEffectChainMap> HDRAudioSubmixEffectChain;
	
	//
	// Submix Processing Chains to achieve low dynamic range audio output
	//
	UPROPERTY(Config, EditAnywhere, Category = "MixSettings")
	TArray<FAudioSubmixEffectChainMap> LDRAudioSubmixEffectChain;

public:
	FSoftObjectPath GetControlBusMixByTag(const FGameplayTag& Tag) const;
	FSoftObjectPath GetControlBusByTag(const FGameplayTag& Tag) const;

};
