// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Evaluation/MovieSceneEvalTemplate.h"
#include "MovieSceneObjectBindingID.h"

#include "KMovieSceneCustomEventSection.h"
#include "KMovieSceneCustomEventTemplate.generated.h"

class UKMovieSceneActorEventTrack;
class UKMovieSceneSubtitleEventTrack;

////////////////////////////////////////////
// FMovieSceneActorEventTemplate
////////////////////////////////////////////

USTRUCT()
struct KGAME_API FKMovieSceneActorEventTemplate : public FMovieSceneEvalTemplate
{
	GENERATED_BODY()

	FKMovieSceneActorEventTemplate() : 
		bFireEventsWhenForwards(1), 
		bFireEventsWhenBackwards(1)
	{}
	FKMovieSceneActorEventTemplate(const UKMovieSceneActorEventSection& Section, const UKMovieSceneActorEventTrack& Track);

	UPROPERTY()
	FKMovieSceneActorEventSectionData EventData;

	UPROPERTY()
	TArray<FMovieSceneObjectBindingID> EventReceivers;

	UPROPERTY()
	uint32 bFireEventsWhenForwards;

	UPROPERTY()
	uint32 bFireEventsWhenBackwards;

private:

	virtual UScriptStruct& GetScriptStructImpl() const override { return *StaticStruct(); }

	virtual void EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const TRange<FFrameNumber>& sweptRange, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;
};

////////////////////////////////////////////
// FMovieSceneSubtitleEventTemplate
////////////////////////////////////////////

USTRUCT()
struct KGAME_API FKMovieSceneSubtitleEventTemplate : public FMovieSceneEvalTemplate
{
	GENERATED_BODY()

	FKMovieSceneSubtitleEventTemplate() : 
		bFireEventsWhenForwards(1),
		bFireEventsWhenBackwards(1) {}

	FKMovieSceneSubtitleEventTemplate(const UKMovieSceneSubtitleEventSection& Section, const UKMovieSceneSubtitleEventTrack& Track);

	UPROPERTY()
	FKMovieSceneActorEventSectionData EventData;

	UPROPERTY()
	TArray<FMovieSceneObjectBindingID> EventReceivers;

	UPROPERTY()
	uint32 bFireEventsWhenForwards;

	UPROPERTY()
	uint32 bFireEventsWhenBackwards;

private:

	virtual UScriptStruct& GetScriptStructImpl() const override { return *StaticStruct(); }

	virtual void EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const TRange<FFrameNumber>& sweptRange, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;
};