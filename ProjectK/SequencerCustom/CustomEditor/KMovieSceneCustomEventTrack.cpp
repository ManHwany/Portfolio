// Fill out your copyright notice in the Description page of Project Settings.

#include "KMovieSceneCustomEventTrack.h"

#include "MovieSceneCommonHelpers.h"
#include "Sections/MovieSceneEventSection.h"
#include "Evaluation/MovieSceneEvaluationTrack.h"
#include "Compilation/IMovieSceneTemplateGenerator.h"
#include "IMovieSceneTracksModule.h"

#include "KMovieSceneCustomEventTemplate.h"

#if WITH_EDITOR
#include "KMovieSceneCustomEventSection.h"
#endif

////////////////////////////////////////////
// UMovieSceneActorEventTrack
////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "UMovieSceneActorEventTrack"

void UKMovieSceneActorEventTrack::AddSection(UMovieSceneSection& Section)
{
	Sections.Add(&Section);
}

UMovieSceneSection* UKMovieSceneActorEventTrack::CreateNewSection()
{
	UE_LOG(LogClass, Warning, TEXT("* KGame Sequencer::Add Subtitle Events ! *"));
	return NewObject<UKMovieSceneActorEventSection>(this, UKMovieSceneActorEventSection::StaticClass(), NAME_None, RF_Transactional);
}

const TArray<UMovieSceneSection*>& UKMovieSceneActorEventTrack::GetAllSections() const
{
	return Sections;
}

bool UKMovieSceneActorEventTrack::HasSection(const UMovieSceneSection& Section) const
{
	return Sections.Contains(&Section);
}

bool UKMovieSceneActorEventTrack::IsEmpty() const
{
	return (Sections.Num() == 0);
}

void UKMovieSceneActorEventTrack::RemoveAllAnimationData()
{
	Sections.Empty();
}

void UKMovieSceneActorEventTrack::RemoveSection(UMovieSceneSection& Section)
{
	Sections.Remove(&Section);
}

FMovieSceneEvalTemplatePtr UKMovieSceneActorEventTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const
{
	return FKMovieSceneActorEventTemplate(*CastChecked<UKMovieSceneActorEventSection>(&InSection), *this);
}

void UKMovieSceneActorEventTrack::PostCompile(FMovieSceneEvaluationTrack& Track, const FMovieSceneTrackCompilerArgs& Args) const
{
	switch (EventPosition)
	{
	case EFireEventsAtPosition::AtStartOfEvaluation:
		Track.SetEvaluationGroup(IMovieSceneTracksModule::GetEvaluationGroupName(EBuiltInEvaluationGroup::PreEvaluation));
		break;

	case EFireEventsAtPosition::AtEndOfEvaluation:
		Track.SetEvaluationGroup(IMovieSceneTracksModule::GetEvaluationGroupName(EBuiltInEvaluationGroup::PostEvaluation));
		break;

	default:
		Track.SetEvaluationGroup(IMovieSceneTracksModule::GetEvaluationGroupName(EBuiltInEvaluationGroup::SpawnObjects));
		Track.SetEvaluationPriority(UMovieSceneSpawnTrack::GetEvaluationPriority() - 100);
		break;
	}

	Track.SetEvaluationMethod(EEvaluationMethod::Swept);
}

#if WITH_EDITORONLY_DATA

FText UKMovieSceneActorEventTrack::GetDefaultDisplayName() const
{
	return LOCTEXT("TrackName", "Events");
}

#endif

#undef LOCTEXT_NAMESPACE


#define LOCTEXT_NAMESPACE "UMovieSceneSubtitleEventTrack"

void UKMovieSceneSubtitleEventTrack::AddSection(UMovieSceneSection& Section)
{
	Sections.Add(&Section);
}

UMovieSceneSection* UKMovieSceneSubtitleEventTrack::CreateNewSection()
{
	UE_LOG(LogClass, Warning, TEXT("* KGame Sequencer::Add New Subtitle Events ! *"));
	return NewObject<UKMovieSceneSubtitleEventSection>(this, UKMovieSceneSubtitleEventSection::StaticClass(), NAME_None, RF_Transactional);
}

const TArray<UMovieSceneSection*>& UKMovieSceneSubtitleEventTrack::GetAllSections() const
{
	return Sections;
}

bool UKMovieSceneSubtitleEventTrack::HasSection(const UMovieSceneSection& Section) const
{
	return Sections.Contains(&Section);
}

bool UKMovieSceneSubtitleEventTrack::IsEmpty() const
{
	return (Sections.Num() == 0);
}

void UKMovieSceneSubtitleEventTrack::RemoveAllAnimationData()
{
	Sections.Empty();
}

void UKMovieSceneSubtitleEventTrack::RemoveSection(UMovieSceneSection& Section)
{
	Sections.Remove(&Section);
}

FMovieSceneEvalTemplatePtr UKMovieSceneSubtitleEventTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const
{
	return FKMovieSceneSubtitleEventTemplate(*CastChecked<UKMovieSceneSubtitleEventSection>(&InSection), *this);
}

void UKMovieSceneSubtitleEventTrack::PostCompile(FMovieSceneEvaluationTrack& Track, const FMovieSceneTrackCompilerArgs& Args) const
{
	switch (EventPosition)
	{
	case EFireEventsAtPosition::AtStartOfEvaluation:
		Track.SetEvaluationGroup(IMovieSceneTracksModule::GetEvaluationGroupName(EBuiltInEvaluationGroup::PreEvaluation));
		break;

	case EFireEventsAtPosition::AtEndOfEvaluation:
		Track.SetEvaluationGroup(IMovieSceneTracksModule::GetEvaluationGroupName(EBuiltInEvaluationGroup::PostEvaluation));
		break;

	default:
		Track.SetEvaluationGroup(IMovieSceneTracksModule::GetEvaluationGroupName(EBuiltInEvaluationGroup::SpawnObjects));
		Track.SetEvaluationPriority(UMovieSceneSpawnTrack::GetEvaluationPriority() - 100);
		break;
	}

	Track.SetEvaluationMethod(EEvaluationMethod::Swept);
}

#if WITH_EDITORONLY_DATA

FText UKMovieSceneSubtitleEventTrack::GetDefaultDisplayName() const
{
	return LOCTEXT("TrackName", "Events");
}

#endif

#if WITH_EDITOR
EMovieSceneSectionMovedResult UKMovieSceneSubtitleEventTrack::OnSectionMoved(UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& /*Params*/)
{
	if (UKMovieSceneSubtitleEventSection* SubtitleSection = Cast<UKMovieSceneSubtitleEventSection>(&Section))
	{
		SubtitleSection->MoveSection_Override();
	}

	return EMovieSceneSectionMovedResult::None;
}

#endif

#undef LOCTEXT_NAMESPACE