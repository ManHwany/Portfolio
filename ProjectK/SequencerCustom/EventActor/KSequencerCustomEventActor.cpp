// Fill out your copyright notice in the Description page of Project Settings.
#include "KSequencerCustomEventActor.h"

#include "Blueprint/UserWidget.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Engine/Public/DrawDebugHelpers.h"

#include "Runtime/Engine/Classes/Engine/AssetManager.h"

#include "Kismet/GameplayStatics.h"

#include "Holder/KSystemHolder.h"
#include "Holder/KMvcHolder.h"
#include "System/Interface/IKUiManager.h"

#include "GameFramework/Base/KGameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/Base/KPlayerController.h"
#include "GameFramework/Base/KPlayerCameraManager.h"
#include "GameFramework/Camera/KBaseCameraActor.h"

#include "GameModule/KAte/Interface/IKAteMv.h"
#include "GameModule/KAte/Common/KAteAgentDef.h"
#include "GameModule/KAte/Mvc/ModelView/Actor/Agent/KAteAgentBase.h"
#include "GameModule/KAte/Mvc/ModelView/Actor/Wave/KAteWaveSpawnType.h"

#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "UI/Subtitle/KUiSubtitleBox.h"

#include "Cinematic/KCinematicManager.h"

#include "Core/KLog.h"
#include "Weather/KWeatherManager.h"

AKSequencerCustomEventActor::AKSequencerCustomEventActor()
{
	PrimaryActorTick.bCanEverTick = true;	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComponent;
}

void AKSequencerCustomEventActor::BeginPlay()
{
	AActor::BeginPlay();	

	TWeakObjectPtr<AActor> sequenceMainActor = getMainSequenceActor();
	if (sequenceMainActor.IsValid())
	{
		AgentCharacter = Cast<ACharacter>(sequenceMainActor);
	}
}

void AKSequencerCustomEventActor::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	setTracedRotation();
}

void AKSequencerCustomEventActor::setTracedRotation()
{
	if (bAttachedToSocket)
	{
		return;
	}

	if (!AgentCharacter.IsValid())
	{
		return;
	}

	const FRotator tracedRotator = AgentCharacter->GetActorRotation() + FRotator(0, -90, 0);
	SetActorRotation(tracedRotator);
}

TWeakObjectPtr<AActor> AKSequencerCustomEventActor::getMainSequenceActor() const
{
	if (const auto& gameMode = GetWorld()->GetAuthGameMode<AKGameModeBase>())
	{
		const auto& cinematicManager = gameMode->GetCinematicManager();

		if (cinematicManager.IsValid()) 
		{
			return cinematicManager->GetSequenceMainActor();
		}
	}

	return TWeakObjectPtr<AActor>();
}

void AKSequencerCustomEventActor::Destroyed()
{
	for (const auto& widget : AnyWidgets)
	{
		if (widget.IsValid())
		{
			widget->RemoveFromParent();
		}
	}

	if (IsValid(SubtitleBox))
	{
		SubtitleBox->RemoveFromParent();
	}	

	if (IsValid(SpawnedHeroPositionType))
	{
		if (AgentCharacter.IsValid())
		{
			AgentCharacter->SetActorTransform(HeroOriginTransform);
		}

		SpawnedHeroPositionType->Destroy();
		SpawnedHeroPositionType = nullptr;
	}

	if (IsValid(SpawnedEnemyPositionType))
	{
		//위치 원복.
		TSharedPtr<IKAteMv> ateMv = FKMvcHolder::Get().GetAteModelView();
		if (ateMv.IsValid())
		{
			for (auto originTrKvp : EnemyOriginTransforms)
			{
			 	auto agent = ateMv->FindAgent(originTrKvp.Key);
				if (agent.IsValid() && agent->GetAgentComp().IsValid())
				{
					agent->GetAgentComp()->GetOwner()->SetActorTransform(originTrKvp.Value);
				}
			}
		}

		EnemyOriginTransforms.Empty();
		SpawnedEnemyPositionType->Destroy();
		SpawnedEnemyPositionType = nullptr;
	}

	StopSound();
		
	CachingSoundStruct.Sound = nullptr;

	Super::Destroyed();
}

void AKSequencerCustomEventActor::OnSubtitleTrackEvent(FKSequencerSubtitleEventStruct subtitleEvent)
{
	//if ((GetWorld() && (GetWorld()->WorldType != EWorldType::Game && GetWorld()->WorldType != EWorldType::PIE)))
	//{
	//	const FString subtitleKey = subtitleEvent.SubtitleKey;

	//	// Runtime 이 아닐때 테스트 출력 로그용.
	//	if (!subtitleKey.IsEmpty())
	//	{
	//		const float duration = subtitleEvent.Duration;
	//		GEngine->AddOnScreenDebugMessage(-1, duration, FColor::Green, FString::Printf(TEXT("%s"), *subtitleKey), true, FVector2D(1.1, 1.1));						
	//	}
	//}

	if (!IsValid(SubtitleBox))
	{
		const FString assetPath = TEXT("/Game/UIs/Subtitle/");
		const FString assetName = TEXT("WB_Subtitle_Box");
		const FString loadedBpName = assetPath + assetName + TEXT(".") + assetName + TEXT("_C");

		FStreamableManager& streamableManager = UAssetManager::GetStreamableManager();

		TSharedPtr<FStreamableHandle> loadingHandle = streamableManager.RequestSyncLoad(FSoftObjectPath(loadedBpName));

		if (UClass* syncBpAsset = Cast<UClass>(loadingHandle->GetLoadedAsset()))
		{
			SubtitleBox = CreateWidget<UKUiSubtitleBox>(GetWorld(), syncBpAsset);
			
			if (IsValid(SubtitleBox))
			{
				SubtitleBox->AddToViewport();
			}			
		}		
	}

	if (IsValid(SubtitleBox))
	{
		SubtitleBox->SetPosition(subtitleEvent.SubtitlePosition);
		SubtitleBox->AddSubtitle(subtitleEvent);
	}
}

void AKSequencerCustomEventActor::OnAttachCameraTrackEvent(FKSequencerAttachCameraEventStruct attachCameraEvent)
{
	TWeakObjectPtr<AActor> sequenceMainActor = getMainSequenceActor();

	if (!sequenceMainActor.IsValid())
	{
		return;
	}

	FAttachmentTransformRules param(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);

	if (ACharacter* agentCharacter = Cast<ACharacter>(sequenceMainActor))
	{
		AgentCharacter = agentCharacter;
		bAttachedToSocket = attachCameraEvent.bAttachedToSocket;

		if (bAttachedToSocket)
		{
			AttachToComponent(agentCharacter->GetMesh(), param, attachCameraEvent.SocketName);
		}
		else
		{
			FVector desiredLocation = AgentCharacter->GetActorLocation();
			float capsuleHalfHeight = AgentCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			desiredLocation.Z -= capsuleHalfHeight;

			setTracedRotation();
			SetActorLocation(desiredLocation);			
		}
	}
}

void AKSequencerCustomEventActor::OnCompleteWaveTrackEvent(FKSequencerOnCompleteWaveEventStruct screenFadeEvent)
{
	if (auto ateMv = FKMvcHolder::Get().GetAteModelView())
	{
		ateMv->ShowResultDirector();
	}	
}

void AKSequencerCustomEventActor::OnOpenUiTrackEvent(FKSequencerOpenUiEventStruct openUiEvent)
{
#if WITH_ENGINE
	if ((GetWorld() && (GetWorld()->WorldType != EWorldType::Game && GetWorld()->WorldType != EWorldType::PIE)))
	{
		return;		
	}
#endif

	auto widgetClass = openUiEvent.UiClass.LoadSynchronous();

	if (UUserWidget* openUi = CreateWidget<UUserWidget>(GetWorld(), widgetClass))
	{
		AnyWidgets.Emplace(openUi);
		openUi->AddToViewport();
	}
}

void AKSequencerCustomEventActor::StopSound()
{
	for (const auto& audioComp : AudioComponents)
	{
		if (audioComp.IsValid())
		{
			audioComp->Stop();
		}		
	}	

	AudioComponents.Empty();
}

void AKSequencerCustomEventActor::SkipSound(const float skipToTime)
{
	for (const auto& audioComp : AudioComponents)
	{
		if (audioComp.IsValid())
		{
			audioComp->Play(skipToTime);
		}
	}
}

void AKSequencerCustomEventActor::SetSoundStruct(FKSequencerPlaySoundStruct newSoundStruct)
{
	CachingSoundStruct = newSoundStruct;	
}

void AKSequencerCustomEventActor::OnPlaySoundTrackEvent(FKSequencerPlaySoundStruct playSoundEvent)
{
	USoundBase* resultSound = nullptr;
	bool is3DSound = false;

	if (!CachingSoundStruct.Sound)
	{
		is3DSound = playSoundEvent.is3DSound;
		resultSound = playSoundEvent.Sound;
	}
	else
	{
		is3DSound = CachingSoundStruct.is3DSound;
		resultSound = CachingSoundStruct.Sound;
	}
	
	if (is3DSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), resultSound, GetActorLocation());
	}
	else
	{
		auto audioComp = UGameplayStatics::CreateSound2D(GetWorld(), resultSound);
		if(audioComp)
		{
			audioComp->Play();
			AudioComponents.Emplace(MoveTemp(audioComp));
		}
	}
}

void AKSequencerCustomEventActor::OnSkipNotifyTrackEvent(FKSequencerSkipNotifyStruct skipNotifyEvent)
{
	if (IsValid(SubtitleBox))
	{
		SubtitleBox->Close();
	}
}

void AKSequencerCustomEventActor::OnWeatherChangeTrackEvent(FKSequencerWeatherChangeStruct weatherChangeEvent)
{
	AKGameModeBase* kGameMode = Cast<AKGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(kGameMode) && kGameMode->GetWeatherManager().IsValid())
	{
		kGameMode->GetWeatherManager()->ChangeWeatherWithBlend(weatherChangeEvent.WeatherType, weatherChangeEvent.WeatherSubType,
			weatherChangeEvent.DecreaseDelay, weatherChangeEvent.DesireWeatherWeight, weatherChangeEvent.IncreaseDelay);
	}
}

void AKSequencerCustomEventActor::OnAgentRepositioningTrackEvent(FKSequencerAgentRepositioningStruct agentRepositioningEvent)
{
	HeroOriginTransform = FTransform::Identity;
	EnemyOriginTransforms.Empty();

	TSharedPtr<IKAteMv> ateMv = FKMvcHolder::Get().GetAteModelView();
	if (!ateMv.IsValid())
	{
		return;
	}

	if (agentRepositioningEvent.HeroPositionType)
	{
		if (AgentCharacter.IsValid())
		{
			SpawnedHeroPositionType = GetWorld()->SpawnActor<AKAteWaveSpawnType>(agentRepositioningEvent.HeroPositionType);
			if (IsValid(SpawnedHeroPositionType))
			{
				AKAteAgentBase* agentBase = Cast<AKAteAgentBase>(AgentCharacter);
				if (agentBase)
				{
					UKAteAgentComponent* AgentComp = agentBase->GetAgentComponent();
					if (AgentComp)
					{
						AgentComp->DisableGoalRotate();
					}
				}
				
				SpawnedHeroPositionType->InitData(0);
				FTransform newTr;
				SpawnedHeroPositionType->GetSpawnMarkTransform(newTr, 0);

				HeroOriginTransform = AgentCharacter->GetActorTransform();
				AgentCharacter->SetActorTransform(newTr);
			}
		}
	}

	if (agentRepositioningEvent.EnemyPositionType)
	{
		SpawnedEnemyPositionType = GetWorld()->SpawnActor<AKAteWaveSpawnType>(agentRepositioningEvent.EnemyPositionType);
		if (IsValid(SpawnedEnemyPositionType))
		{
			SpawnedEnemyPositionType->InitData(0);

			for (auto& agentKvp : ateMv->GetModelAgents())
			{
				auto agent = agentKvp.Value;
				if (!agent.IsValid() || agent->IsDie())
				{
					continue;
				}

				if (agent->GetTeamType() != E_KAteTeam::Enemy)
				{
					continue;
				}

				auto agentComp = agent->GetAgentComp();
				if (!agentComp.IsValid())
				{
					continue;
				}

				const FTransform originTr = agentComp->GetActorTransform();
				EnemyOriginTransforms.Emplace(agentKvp.Key, originTr);
				
				const int32 spawnId = agent->GetTeamSpawnIndex();
				FTransform newTr;
				const int32 index = SpawnedEnemyPositionType->GetSpawnMarkTransform(newTr, spawnId);
				if (index != INDEX_NONE)
				{
					agentComp->SetActorLocation(newTr.GetLocation(), true);
					agentComp->SetActorRotation(newTr.GetRotation());
				}
			}
		}
	}
}
