// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "Curves/NameCurve.h"
#include "MovieSceneClipboard.h"
#include "Channels/MovieSceneChannel.h"
#include "Channels/MovieSceneChannelData.h"
#include "Channels/MovieSceneChannelHandle.h"
#include "Channels/MovieSceneChannelTraits.h"

#include "../EventActor/KSequencerCustomEventActor.h"
#include "../EventActor/KSequencerCustomCharacter.h"
#include "../EventActor/KSequencerCustomAttachActor.h"

#include "KMovieSceneCustomEventSection.generated.h"

struct FKSequencerSubtitleEventStruct;
struct FKSequencerSpeechBubbleEventStruct;
/**
*	자막 이벤트
*/

#if WITH_EDITOR

namespace MovieSceneClipboard
{
	template<>
	inline FName GetKeyTypeName<FKSequencerSubtitleEventStruct>()
	{
		return "FActorEventSectionKey";
	}
}

#endif

/** A curve of events */
USTRUCT()
struct FKMovieSceneActorEventSectionData : public FMovieSceneChannel
{
	GENERATED_BODY()

	/**
	* Called after this section data has been serialized to upgrade old data
	*/
	void PostSerialize(const FArchive& Ar);

	/**
	* Access a mutable interface for this channel's data
	*
	* @return An object that is able to manipulate this channel's data
	*/
	FORCEINLINE TMovieSceneChannelData<FKSequencerSubtitleEventStruct> GetData()
	{
		return TMovieSceneChannelData<FKSequencerSubtitleEventStruct>(&Times, &KeyValues, &KeyHandles);
	}

	/**
	* Access a constant interface for this channel's data
	*
	* @return An object that is able to interrogate this channel's data
	*/
	FORCEINLINE TMovieSceneChannelData<const FKSequencerSubtitleEventStruct> GetData() const
	{
		return TMovieSceneChannelData<const FKSequencerSubtitleEventStruct>(&Times, &KeyValues);
	}

	TArrayView<const FFrameNumber> GetKeyTimes() const
	{
		return Times;
	}

	TArrayView<const FKSequencerSubtitleEventStruct> GetKeyValues() const
	{
		return KeyValues;
	}

public:
	// ~ FMovieSceneChannel Interface
	virtual void GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles) override;
	virtual void GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes) override;
	virtual void SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes) override;
	virtual void DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles) override;
	virtual void DeleteKeys(TArrayView<const FKeyHandle> InHandles) override;
	virtual void ChangeFrameResolution(FFrameRate SourceRate, FFrameRate DestinationRate) override;
	virtual TRange<FFrameNumber> ComputeEffectiveRange() const override;
	virtual int32 GetNumKeys() const override;
	virtual void Reset() override;
	virtual void Offset(FFrameNumber DeltaPosition) override;
	virtual void Optimize(const FKeyDataOptimizationParams& InParameters) override {}
	virtual void ClearDefault() override {}

	TSharedPtr<FStructOnScope> GetKeyStruct(TMovieSceneChannelHandle<FKMovieSceneActorEventSectionData> Channel, FKeyHandle InHandle);


	inline bool Evaluate(FFrameTime InTime, FKSequencerSubtitleEventStruct& OutValue) const
	{
		if (Times.Num())
		{
			const int32 Index = FMath::Max(0, Algo::UpperBound(Times, InTime.FrameNumber) - 1);
			OutValue = KeyValues[Index];
			return true;
		}

		return false;
	}

private:
	UPROPERTY(meta = (KeyTimes))
	TArray<FFrameNumber> Times;

	/** Array of values that correspond to each key time */
	UPROPERTY(meta = (KeyValues))
	TArray<FKSequencerSubtitleEventStruct> KeyValues;

	FMovieSceneKeyHandleMap KeyHandles;


#if WITH_EDITORONLY_DATA

	UPROPERTY()
	TArray<float> KeyTimes_DEPRECATED;

#endif
};

template<>
struct TStructOpsTypeTraits<FKMovieSceneActorEventSectionData> : public TStructOpsTypeTraitsBase2<FKMovieSceneActorEventSectionData>
{
	enum { WithPostSerialize = true };
};


/**
* Implements a section in movie scene event tracks.
*/
UCLASS()
class KGAME_API UKMovieSceneActorEventSection : public UMovieSceneSection
{
	GENERATED_BODY()

	/** Default constructor. */
	UKMovieSceneActorEventSection();

public:

	// ~UObject interface
	virtual void PostLoad() override;

	/**
	* Get the section's event data.
	*
	* @return Event data.
	*/
	const FKMovieSceneActorEventSectionData& GetEventData() const { return EventData; }

private:

	UPROPERTY()
	FNameCurve Events_DEPRECATED;

	UPROPERTY()
	FKMovieSceneActorEventSectionData EventData;
};

template<>
struct TMovieSceneChannelTraits<FKMovieSceneActorEventSectionData> : TMovieSceneChannelTraitsBase<FKMovieSceneActorEventSectionData>
{
	enum { SupportsDefaults = false };
};

/** Stub out unnecessary functions */
inline bool EvaluateChannel(const FKMovieSceneActorEventSectionData* InChannel, FFrameTime InTime, FKSequencerSubtitleEventStruct& OutValue)
{
	// Can't evaluate event section data in the typical sense
	return false;
}

inline bool ValueExistsAtTime(const FKMovieSceneActorEventSectionData* InChannel, FFrameNumber Time, const FKSequencerSubtitleEventStruct& Value)
{
	// true if any value exists
	return InChannel->GetData().FindKey(Time) != INDEX_NONE;
}

////////////////////////////////////////////
// UMovieSceneSubtitleEventSection
////////////////////////////////////////////
// EventData는 위의 기존 자막 이벤트(~ActorEvent)것을 그대로 사용
////////////////////////////////////////////

/**
* Implements a section in movie scene event tracks.
*/
UCLASS()
class KGAME_API UKMovieSceneSubtitleEventSection
	: public UMovieSceneSection
{
	GENERATED_BODY()

	/** Default constructor. */
	UKMovieSceneSubtitleEventSection();

public:

	// ~UObject interface
	virtual void PostLoad() override;

	/**
	* Get the section's event data.
	*
	* @return Event data.
	*/
	const FKMovieSceneActorEventSectionData& GetEventData() const { return EventData; }

#if WITH_EDITOR
	// From UMovieSceneSection::SetRange
	// 새 자막 이벤트 테스트용 함수
	void SetRange_Override(const TRange<FFrameNumber>& NewRange);

	void MoveSection_Override();
#endif

	UPROPERTY(EditAnywhere, Category = "Event")
	FFrameNumber SubtitleStartTime;

	/** Array of values that correspond to each key time */
	UPROPERTY(EditAnywhere, Category = "Event")
	FKSequencerSubtitleEventStruct SubtitleDetail;

protected:

#if WITH_EDITOR
	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;
#endif

private:
	UPROPERTY()
	FNameCurve Events_DEPRECATED;

	UPROPERTY()
	FKMovieSceneActorEventSectionData EventData;
};