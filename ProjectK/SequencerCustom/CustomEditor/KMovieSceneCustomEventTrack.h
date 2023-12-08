// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "MovieSceneNameableTrack.h"
#include "Tracks/MovieSceneSpawnTrack.h"
#include "MovieSceneObjectBindingID.h"
#include "Runtime/MovieSceneTracks/Public/Tracks/MovieSceneEventTrack.h"

#include "Compilation/IMovieSceneTrackTemplateProducer.h"

#include "KMovieSceneCustomEventTrack.generated.h"

struct FMovieSceneEvaluationTrack;

////////////////////////////////////////////
// UMovieSceneActorEventTrack
////////////////////////////////////////////

UCLASS()
class KGAME_API UKMovieSceneActorEventTrack : public UMovieSceneNameableTrack, public IMovieSceneTrackTemplateProducer
{
	GENERATED_BODY()

public:

	/** Default constructor. */
	UKMovieSceneActorEventTrack()
		: bFireEventsWhenForwards(true)
		, bFireEventsWhenBackwards(true)
		, EventPosition(EFireEventsAtPosition::AfterSpawn)
	{
#if WITH_EDITORONLY_DATA
		TrackTint = FColor(41, 98, 41, 150);
#endif
	}

public:

	// UMovieSceneTrack interface

	virtual void AddSection(UMovieSceneSection& Section) override;
	virtual UMovieSceneSection* CreateNewSection() override;
	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
	virtual bool HasSection(const UMovieSceneSection& Section) const override;
	virtual bool IsEmpty() const override;
	virtual void RemoveAllAnimationData() override;
	virtual void RemoveSection(UMovieSceneSection& Section) override;
	virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection(const UMovieSceneSection& InSection) const override;
	virtual void PostCompile(FMovieSceneEvaluationTrack& Track, const FMovieSceneTrackCompilerArgs& Args) const override;

#if WITH_EDITORONLY_DATA
	virtual FText GetDefaultDisplayName() const override;
#endif

public:

	/** If events should be fired when passed playing the sequence forwards. */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	uint32 bFireEventsWhenForwards : 1;

	/** If events should be fired when passed playing the sequence backwards. */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	uint32 bFireEventsWhenBackwards : 1;

	/** Defines where in the evaluation to trigger events */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	EFireEventsAtPosition EventPosition;

	/** Defines a list of object bindings on which to trigger the events in this track. When empty, events will trigger in the default event contexts for the playback environment (such as the level blueprint, or widget). */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	TArray<FMovieSceneObjectBindingID> EventReceivers;

private:
	/** The track's sections. */
	UPROPERTY()
	TArray<UMovieSceneSection*> Sections;
};


////////////////////////////////////////////
// UMovieSceneSubtitleEventTrack
////////////////////////////////////////////

UCLASS()
class KGAME_API UKMovieSceneSubtitleEventTrack : public UMovieSceneNameableTrack, public IMovieSceneTrackTemplateProducer
{
	GENERATED_BODY()

public:

	/** Default constructor. */
	UKMovieSceneSubtitleEventTrack()
		: bFireEventsWhenForwards(true)
		, bFireEventsWhenBackwards(true)
		, EventPosition(EFireEventsAtPosition::AfterSpawn)
	{
#if WITH_EDITORONLY_DATA
		TrackTint = FColor(41, 98, 41, 150);
#endif
	}

public:

	// UMovieSceneTrack interface

	//UMovieSceneSection* CreateNewSection_Override(FQualifiedFrameTime CursorTime);

	virtual void AddSection(UMovieSceneSection& Section) override;
	virtual UMovieSceneSection* CreateNewSection() override;
	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
	virtual bool HasSection(const UMovieSceneSection& Section) const override;
	virtual bool IsEmpty() const override;
	virtual void RemoveAllAnimationData() override;
	virtual void RemoveSection(UMovieSceneSection& Section) override;
	virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection(const UMovieSceneSection& InSection) const override;
	virtual void PostCompile(FMovieSceneEvaluationTrack& Track, const FMovieSceneTrackCompilerArgs& Args) const override;

#if WITH_EDITORONLY_DATA
	virtual FText GetDefaultDisplayName() const override;
#endif

#if WITH_EDITOR
	/**
	 * Called if the section is moved in Sequencer.
	 * 섹션 이동이 끝났을때 (마우스 땠을때) 호출됨
	 *
	 * @param Section The section that moved.
	 */

	virtual EMovieSceneSectionMovedResult OnSectionMoved(UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params) override;

#endif

public:
	/** If events should be fired when passed playing the sequence forwards. */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	uint32 bFireEventsWhenForwards : 1;

	/** If events should be fired when passed playing the sequence backwards. */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	uint32 bFireEventsWhenBackwards : 1;

	/** Defines where in the evaluation to trigger events */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	EFireEventsAtPosition EventPosition;

	/** Defines a list of object bindings on which to trigger the events in this track. When empty, events will trigger in the default event contexts for the playback environment (such as the level blueprint, or widget). */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	TArray<FMovieSceneObjectBindingID> EventReceivers;

private:

	/** The track's sections. */
	UPROPERTY()
	TArray<UMovieSceneSection*> Sections;
};