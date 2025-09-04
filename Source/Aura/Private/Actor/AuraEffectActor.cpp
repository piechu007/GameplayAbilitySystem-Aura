// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
// #include "Components/StaticMeshComponent.h"
// #include "Components/SphereComponent.h"
// #include "AbilitySystem/AuraAttributeSet.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor *TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (IAbilitySystemInterface *ASCInterface = Cast<IAbilitySystemInterface>(TargetActor))
	{
		if (UAbilitySystemComponent *TargetASC = ASCInterface->GetAbilitySystemComponent())
		{
			check(GameplayEffectClass);

			FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
			EffectContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
			FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

			const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
			if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
			{
				ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
			}
		}
	}
}

void AAuraEffectActor::OnOverlap(AActor *TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor *TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	// Remove Infinite Effect
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		if (IAbilitySystemInterface *ASCInterface = Cast<IAbilitySystemInterface>(TargetActor))
		{
			if (UAbilitySystemComponent *TargetASC = ASCInterface->GetAbilitySystemComponent())
			{
				TArray<FActiveGameplayEffectHandle> HandlesToRemove;
				for (auto HandlePair : ActiveEffectHandles)
				{
					if (TargetASC == HandlePair.Value)
					{
						TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
						HandlesToRemove.Add(HandlePair.Key);
					}
				}
				for (auto& Handle : HandlesToRemove)
				{
					ActiveEffectHandles.FindAndRemoveChecked(Handle);
				}
			}
		}
	}
}
