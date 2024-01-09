// Copyright (C) 2024 owoDra

#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "AudioMixTypes.h"

#include "GameplayTagContainer.h"

#include "AudioMixSubsystem.generated.h"

class USoundControlBus;
class USoundControlBusMix;
class USoundEffectSubmixPreset;
class USoundSubmix;


/**
 * Subsystem to register and manage ControlBus, ControlBuxMix, Submix, etc. of Audio
 * 
 * Tips:
 *	This subsystem can be subclassed to suit the project, and processes such as volume initialization can be added.
 */
UCLASS()
class GACORE_API UAudioMixSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UAudioMixSubsystem() {}

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;


protected:
	//
	// Sound Control Bus Mix retrieved from the Devloper settings
	//
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<USoundControlBusMix>> ControlBusMixMap;

	//
	// Sound Control Bus retrieved from the Devloper settings
	//
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<USoundControlBus>> ControlBusMap;

	//
	// Submix Effect Chain Overrides to apply when HDR Audio is turned on
	//
	UPROPERTY(Transient)
	TArray<FAudioSubmixEffectsChain> HDRSubmixEffectChain;

	//
	// Submix Effect hain Overrides to apply when HDR Audio is turned off
	//
	UPROPERTY(Transient)
	TArray<FAudioSubmixEffectsChain> LDRSubmixEffectChain;

public:
	/** 
	 * Set whether the HDR Audio Submix Effect Chain Override settings are applied 
	 */
	void ApplyDynamicRangeEffectsChains(bool bHDRAudio);

	/**
	 * Enable or disable specific ControlBusMixes
	 */
	void ApplyControlBusMix(FGameplayTag ControlBusMixTag, bool NewActivate = true);

	/**
	 * Enable or disable specific ControlBusMixes
	 */
	void ApplyControlBusVolume(FGameplayTag ControlBusTag, float NewVolume);

};
