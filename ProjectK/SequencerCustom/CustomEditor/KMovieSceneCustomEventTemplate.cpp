// Fill out your copyright notice in the Description page of Project Settings.

#include "KMovieSceneCustomEventTemplate.h"

#include "MovieSceneSequence.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "EngineGlobals.h"
#include "MovieScene.h"
#include "Evaluation/MovieSceneEvaluation.h"
#include "IMovieScenePlayer.h"

#include "Cinematic/EventActor/KSequencerCustomEventActor.h"

#include "KMovieSceneCustomEventTrack.h"
#include "KMovieSceneCustomEventSection.h"

////////////////////////////////////////////
// FMovieSceneActorEventTemplate
////////////////////////////////////////////

struct FKMovieSceneActorEventData
{
	FKMovieSceneActorEventData(const FKSequencerSubtitleEventStruct& InPayload) : Payload(InPayload) {}

	FKSequencerSubtitleEventStruct Payload;
};

/** A movie scene execution token that stores a specific transform, and an operand */
struct FKEventTrackActionExecutionToken
	: IMovieSceneExecutionToken
{
	FKEventTrackActionExecutionToken(TArray<FKMovieSceneActorEventData> InEvents, const TArray<FMovieSceneObjectBindingID>& InEventReceivers) : Events(MoveTemp(InEvents)), EventReceivers(InEventReceivers) {}

	/** Execute this token, operating on all objects referenced by 'Operand' */
	virtual void Execute(const FMovieSceneContext& Context, const FMovieSceneEvaluationOperand& Operand, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) override
	{
		// Resolve event contexts to trigger the event on
		TArray<UObject*> EventContexts;		

		// If we have specified event receivers, use those
		if (EventReceivers.Num())
		{
			EventContexts.Reserve(EventReceivers.Num());
			for (FMovieSceneObjectBindingID ID : EventReceivers)
			{
				// Ensure that this ID is resolvable from the root, based on the current local sequence ID
				FMovieSceneSequenceID sequenceId = ID.ResolveSequenceID(Operand.SequenceID, Player);
				// Lookup the object(s) specified by ID in the player

				for (TWeakObjectPtr<> WeakEventContext : Player.FindBoundObjects(ID.GetGuid(), sequenceId))
				{
					if (UObject* EventContext = WeakEventContext.Get())
					{
						EventContexts.Add(EventContext);
					}
				}
			}
		}		
		else
		{
			// If we haven't specified event receivers, use the default set defined on the player
			EventContexts = Player.GetEventContexts();
		}

		for (UObject* EventContextObject : EventContexts)
		{
			auto KEventActor = Cast<AKSequencerCustomEventActor>(EventContextObject);
			if (!KEventActor)
			{
				UE_LOG(LogClass, Log, TEXT("FEventTrackActionExecutionToken::Execue - AEventActor Cast Fail : continued"));
				continue;
			}

			for (FKMovieSceneActorEventData& Event : Events)
			{
				//메인 모듈의 함수 호출
				KEventActor->OnSubtitleTrackEvent(Event.Payload);
			}
		}
	}

	TArray<FKMovieSceneActorEventData> Events;
	TArray<FMovieSceneObjectBindingID, TInlineAllocator<2>> EventReceivers;
};

FKMovieSceneActorEventTemplate::FKMovieSceneActorEventTemplate(const UKMovieSceneActorEventSection& Section, const UKMovieSceneActorEventTrack& Track)
	: EventData(Section.GetEventData())
	, EventReceivers(Track.EventReceivers)
	, bFireEventsWhenForwards(Track.bFireEventsWhenForwards)
	, bFireEventsWhenBackwards(Track.bFireEventsWhenBackwards)
{
}


//#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 22
void FKMovieSceneActorEventTemplate::EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const TRange<FFrameNumber>& sweptRange, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
//#else
//void FKMovieSceneActorEventTemplate::EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
//#endif


{
	// Don't allow events to fire when playback is in a stopped state. This can occur when stopping 
	// playback and returning the current position to the start of playback. It's not desireable to have 
	// all the events from the last playback position to the start of playback be fired.
	if (Context.GetStatus() == EMovieScenePlayerStatus::Stopped || Context.IsSilent())
	{
		return;
	}

	const bool bBackwards = Context.GetDirection() == EPlayDirection::Backwards;

	if ((!bBackwards && !bFireEventsWhenForwards) ||
		(bBackwards && !bFireEventsWhenBackwards))
	{
		return;
	}

	TArray<FKMovieSceneActorEventData> Events;

//#if !(ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 22)
	const TRange<FFrameNumber>& SweptRange = Context.GetFrameNumberRange();
//#endif

	TArrayView<const FFrameNumber>  KeyTimes = EventData.GetKeyTimes();
	TArrayView<const FKSequencerSubtitleEventStruct> KeyValues = EventData.GetKeyValues();

	const int32 First = bBackwards ? KeyTimes.Num() - 1 : 0;
	const int32 Last = bBackwards ? 0 : KeyTimes.Num() - 1;
	const int32 Inc = bBackwards ? -1 : 1;

	//const float PositionInSeconds = Context.GetTime() * Context.GetRootToSequenceTransform().Inverse() / Context.GetFrameRate();

	if (bBackwards)
	{
		// Trigger events backwards
		for (int32 KeyIndex = KeyTimes.Num() - 1; KeyIndex >= 0; --KeyIndex)
		{
			FFrameNumber Time = KeyTimes[KeyIndex];
			if (SweptRange.Contains(Time))
			{
				Events.Add(FKMovieSceneActorEventData(KeyValues[KeyIndex]));
			}
		}
	}
	// Trigger events forwards
	else for (int32 KeyIndex = 0; KeyIndex < KeyTimes.Num(); ++KeyIndex)
	{
		FFrameNumber Time = KeyTimes[KeyIndex];
		if (SweptRange.Contains(Time))
		{
			Events.Add(FKMovieSceneActorEventData(KeyValues[KeyIndex]));
		}
	}


	if (Events.Num())
	{
		ExecutionTokens.Add(FKEventTrackActionExecutionToken(MoveTemp(Events), EventReceivers));
	}
}

////////////////////////////////////////////
// FMovieSceneSubtitleEventTemplate
////////////////////////////////////////////
// EventData는 위의 기존 자막 이벤트(~ActorEvent)것을 그대로 사용
////////////////////////////////////////////

FKMovieSceneSubtitleEventTemplate::FKMovieSceneSubtitleEventTemplate(const UKMovieSceneSubtitleEventSection& Section, const UKMovieSceneSubtitleEventTrack& Track)
	: EventData(Section.GetEventData())
	, EventReceivers(Track.EventReceivers)
	, bFireEventsWhenForwards(Track.bFireEventsWhenForwards)
	, bFireEventsWhenBackwards(Track.bFireEventsWhenBackwards)
{
}

//#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 22
void FKMovieSceneSubtitleEventTemplate::EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const TRange<FFrameNumber>& sweptRange, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
//#else
//void FKMovieSceneSubtitleEventTemplate::EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
//#endif
{
	// Don't allow events to fire when playback is in a stopped state. This can occur when stopping 
	// playback and returning the current position to the start of playback. It's not desireable to have 
	// all the events from the last playback position to the start of playback be fired.
	if (Context.GetStatus() == EMovieScenePlayerStatus::Stopped || Context.IsSilent())
	{
		return;
	}

	const bool bBackwards = Context.GetDirection() == EPlayDirection::Backwards;

	if ((!bBackwards && !bFireEventsWhenForwards) ||
		(bBackwards && !bFireEventsWhenBackwards))
	{
		return;
	}

	TArray<FKMovieSceneActorEventData> Events;

//#if !(ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 22)
	TRange<FFrameNumber> SweptRange = Context.GetFrameNumberRange();
//#endif

	TArrayView<const FFrameNumber>  KeyTimes = EventData.GetKeyTimes();
	TArrayView<const FKSequencerSubtitleEventStruct> KeyValues = EventData.GetKeyValues();

	const int32 First = bBackwards ? KeyTimes.Num() - 1 : 0;
	const int32 Last = bBackwards ? 0 : KeyTimes.Num() - 1;
	const int32 Inc = bBackwards ? -1 : 1;

	//const float PositionInSeconds = Context.GetTime() * Context.GetRootToSequenceTransform().Inverse() / Context.GetFrameRate();

	if (bBackwards)
	{
		// Trigger events backwards
		for (int32 KeyIndex = KeyTimes.Num() - 1; KeyIndex >= 0; --KeyIndex)
		{
			FFrameNumber Time = KeyTimes[KeyIndex];
			if (SweptRange.Contains(Time))
			{
				Events.Add(FKMovieSceneActorEventData(KeyValues[KeyIndex]));
			}
		}
	}
	// Trigger events forwards
	else for (int32 KeyIndex = 0; KeyIndex < KeyTimes.Num(); ++KeyIndex)
	{
		FFrameNumber Time = KeyTimes[KeyIndex];
		if (SweptRange.Contains(Time))
		{
			Events.Add(FKMovieSceneActorEventData(KeyValues[KeyIndex]));
		}
	}

	if (Events.Num())
	{
		ExecutionTokens.Add(FKEventTrackActionExecutionToken(MoveTemp(Events), EventReceivers));
	}
}