// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"
#include "GameFramework/Actor.h"
#include "CameraUtil/KCameraStruct.h"
#include "Cinematic/Define/KCinematicDefine.h"
#include "Core/KNoncopyable.h"
//#include "Runtime/MovieScene/Public/MovieSceneObjectBindingID.h"
#include "KSequencerCustomEventActor.generated.h"

class AKAteWaveSpawnType;

enum class E_KWeatherType : uint8;
enum class E_KWeatherSubType : uint8;

USTRUCT(BlueprintType)
struct KGAME_API FKSequencerSubtitleEventStruct
{
	GENERATED_USTRUCT_BODY()

public:
	K_CLASS_DEFAULT_COPYOP_ADD_DC(FKSequencerSubtitleEventStruct)

	UPROPERTY(EditAnywhere, Category = "-KGame|Subtitle")
	E_KSubtitlePosition SubtitlePosition = E_KSubtitlePosition::Down;

	UPROPERTY(EditAnywhere, Category = "-KGame|Subtitle")
	TSoftClassPtr<class UKUiSubtitleSlot> SubtitleSlotWidget = TSoftClassPtr<class UKUiSubtitleSlot>();

	/** 출력되어 보여질 텍스트 입력. */
	UPROPERTY(EditAnywhere, Category = "-KGame|Subtitle")
	FString SubtitleKey;

	/** 자막 지속 시간 */
	UPROPERTY(EditAnywhere, Category = "-KGame|Subtitle")
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, Category = "-KGame|Subtitle")
	bool bUseAlphaBlend = false;

	UPROPERTY(EditAnywhere, Category = "-KGame|Subtitle", meta = (editcondition = "bUseAlphaBlend"))
	float AlphaBlendSpeed = 5.f;
};

USTRUCT(BlueprintType)
struct KGAME_API FKSequencerAttachCameraEventStruct
{
	GENERATED_USTRUCT_BODY()

public:
	K_CLASS_DEFAULT_COPYOP_ADD_DC(FKSequencerAttachCameraEventStruct)

	/** 소켓에 붙일 것인지여부 ( false 시에는 최초 타겟 에이전트의 최초 위치에 이동만 해놓는다. ) */
	UPROPERTY(EditAnywhere, Category = "Attach Camera")
	bool bAttachedToSocket = false;

	/** 붙일 소켓 이름 */
	UPROPERTY(EditAnywhere, Category = "Attach Camera", meta = (editcondition = "!bUseBlendCurve"))
	FName SocketName = "Root";	
};

USTRUCT(BlueprintType)
struct KGAME_API FKSequencerOnCompleteWaveEventStruct
{
	GENERATED_USTRUCT_BODY()

public:
	K_CLASS_DEFAULT_COPYOP_ADD_DC(FKSequencerOnCompleteWaveEventStruct)
};

USTRUCT(BlueprintType)
struct KGAME_API FKSequencerOpenUiEventStruct
{
	GENERATED_USTRUCT_BODY()

public:
	K_CLASS_DEFAULT_COPYOP_ADD_DC(FKSequencerOpenUiEventStruct)
	   	
	/** Umg Class */
	UPROPERTY(EditAnywhere, Category = "Open Ui")
	TSoftClassPtr<class UUserWidget> UiClass;

	/** Umg Name */
	UPROPERTY(EditAnywhere, Category = "Open Ui")
	FName UiName = "";
};

USTRUCT(BlueprintType)
struct KGAME_API FKSequencerPlaySoundStruct
{
	GENERATED_USTRUCT_BODY()

public:
	K_CLASS_DEFAULT_COPYOP_ADD_DC(FKSequencerPlaySoundStruct)

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "KPlay Sound")
	bool is3DSound = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "KPlay Sound")
	class USoundBase* Sound = nullptr;
};

USTRUCT(BlueprintType)
struct KGAME_API FKSequencerSkipNotifyStruct
{
	GENERATED_USTRUCT_BODY()

public:
	K_CLASS_DEFAULT_COPYOP_ADD_DC(FKSequencerSkipNotifyStruct)
};

USTRUCT(BlueprintType)
struct KGAME_API FKSequencerWeatherChangeStruct
{
	GENERATED_USTRUCT_BODY()

public:
	K_CLASS_DEFAULT_COPYOP_ADD_DC(FKSequencerWeatherChangeStruct)

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	E_KWeatherType WeatherType;

	/** 날씨 서브 타입 - 같은 날씨이지만, 속성이 다를경우, (ex. 노을진 밤) */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	E_KWeatherSubType WeatherSubType;

	//현재 날씨 감쇄 블렌딩 시간
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float DecreaseDelay = 2.f;
	//목표 날씨 WeightValue
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float DesireWeatherWeight = 1.f;
	//목표 날씨 증감 블렌딩 시간
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float IncreaseDelay = 2.f;
};

USTRUCT(BlueprintType)
struct KGAME_API FKSequencerAgentRepositioningStruct
{
	GENERATED_USTRUCT_BODY()

public:
	K_CLASS_DEFAULT_COPYOP_ADD_DC(FKSequencerAgentRepositioningStruct)

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AKAteWaveSpawnType> HeroPositionType;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AKAteWaveSpawnType> EnemyPositionType;
};

/********************************************************/

class USceneComponent;
class UAudioComponent;
class UUserWidget;
class UKUiSubtitleBox;
class ACharacter;
class USoundBase;

UCLASS(Blueprintable)
class KGAME_API AKSequencerCustomEventActor : public AActor
{
	GENERATED_BODY()

private:	
	TArray<TWeakObjectPtr<UUserWidget>> AnyWidgets;	

protected:
	AKSequencerCustomEventActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SceneComponent;

protected:
	//~Begin For Subtitle
	UPROPERTY(Transient)
	UKUiSubtitleBox* SubtitleBox;
	//~End 

	//~Begin For Attach
	bool bAttachedToSocket = false;
	TWeakObjectPtr<ACharacter> AgentCharacter;
	//~End

	//~Begin For Sound
	FKSequencerPlaySoundStruct CachingSoundStruct;
	TArray<TWeakObjectPtr<UAudioComponent>> AudioComponents;
	//~End	

	AKAteWaveSpawnType* SpawnedHeroPositionType;
	AKAteWaveSpawnType* SpawnedEnemyPositionType;
	FTransform HeroOriginTransform;
	TMap<int32, FTransform> EnemyOriginTransforms;

private:
	TWeakObjectPtr<AActor> getMainSequenceActor() const;
	void setTracedRotation();	

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;

public:
	UFUNCTION(BlueprintCallable)
	void StopSound();

	UFUNCTION(BlueprintCallable)
	void SkipSound(const float skipToTime);

	UFUNCTION(BlueprintCallable)
	void SetSoundStruct(FKSequencerPlaySoundStruct newSoundStruct);

public:

	/** 자막 이벤트 호출 함수 */
	UFUNCTION()
	void OnSubtitleTrackEvent(FKSequencerSubtitleEventStruct subtitleEvent);

	/** 어태치 카메라 호출 함수 */
	UFUNCTION()
	void OnAttachCameraTrackEvent(FKSequencerAttachCameraEventStruct attachCameraEvent);

	/** 웨이브 완료 이벤트 */
	UFUNCTION()
	void OnCompleteWaveTrackEvent(FKSequencerOnCompleteWaveEventStruct screenFadeEvent);

	/** Ui Open 이벤트 */
	UFUNCTION()
	void OnOpenUiTrackEvent(FKSequencerOpenUiEventStruct openUiEvent);

	/** Play Sound 이벤트 */
	UFUNCTION()
	void OnPlaySoundTrackEvent(FKSequencerPlaySoundStruct playSoundEvent);

	/** Skip Notify 이벤트 */
	UFUNCTION()
	void OnSkipNotifyTrackEvent(FKSequencerSkipNotifyStruct skipNotifyEvent);

	UFUNCTION()
	void OnWeatherChangeTrackEvent(FKSequencerWeatherChangeStruct weatherChangeEvent);

	UFUNCTION()
	void OnAgentRepositioningTrackEvent(FKSequencerAgentRepositioningStruct agentRepositioningEvent);
};