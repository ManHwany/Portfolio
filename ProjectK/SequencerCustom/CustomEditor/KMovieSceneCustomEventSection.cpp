// Fill out your copyright notice in the Description page of Project Settings.

#include "KMovieSceneCustomEventSection.h"

#include "EngineGlobals.h"
#include "IMovieScenePlayer.h"
#include "UObject/ReleaseObjectVersion.h"
#include "UObject/LinkerLoad.h"
#include "MovieSceneFwd.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Serialization/MemoryArchive.h"
#include "Engine/UserDefinedStruct.h"
#include "MovieSceneFrameMigration.h"

/**
 * 자막 이벤트 관련 코드
 */

void FKMovieSceneActorEventSectionData::PostSerialize(const FArchive& Ar)
{
#if WITH_EDITORONLY_DATA

	if (KeyTimes_DEPRECATED.Num())
	{
		FFrameRate LegacyFrameRate = GetLegacyConversionFrameRate();

		TArray<FKSequencerSubtitleEventStruct> OldValues = KeyValues;
		Times.Reset(KeyTimes_DEPRECATED.Num());
		KeyValues.Reset(KeyTimes_DEPRECATED.Num());
		for (int32 Index = 0; Index < KeyTimes_DEPRECATED.Num(); ++Index)
		{
			FFrameNumber KeyTime = UpgradeLegacyMovieSceneTime(nullptr, LegacyFrameRate, KeyTimes_DEPRECATED[Index]);
			ConvertInsertAndSort<FKSequencerSubtitleEventStruct>(Index, KeyTime, OldValues[Index], Times, KeyValues);
		}
		KeyTimes_DEPRECATED.Empty();
	}
#endif
}

void FKMovieSceneActorEventSectionData::GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles)
{
	GetData().GetKeys(WithinRange, OutKeyTimes, OutKeyHandles);
}

void FKMovieSceneActorEventSectionData::GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes)
{
	GetData().GetKeyTimes(InHandles, OutKeyTimes);
}

void FKMovieSceneActorEventSectionData::SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes)
{
	GetData().SetKeyTimes(InHandles, InKeyTimes);
}

void FKMovieSceneActorEventSectionData::DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles)
{
	GetData().DuplicateKeys(InHandles, OutNewHandles);
}

void FKMovieSceneActorEventSectionData::DeleteKeys(TArrayView<const FKeyHandle> InHandles)
{
	GetData().DeleteKeys(InHandles);
}

void FKMovieSceneActorEventSectionData::ChangeFrameResolution(FFrameRate SourceRate, FFrameRate DestinationRate)
{
	GetData().ChangeFrameResolution(SourceRate, DestinationRate);
}

TRange<FFrameNumber> FKMovieSceneActorEventSectionData::ComputeEffectiveRange() const
{
	return GetData().GetTotalRange();
}

int32 FKMovieSceneActorEventSectionData::GetNumKeys() const
{
	return Times.Num();
}

void FKMovieSceneActorEventSectionData::Reset()
{
	Times.Reset();
	KeyValues.Reset();
	KeyHandles.Reset();
}

void FKMovieSceneActorEventSectionData::Offset(FFrameNumber DeltaPosition)
{
	GetData().Offset(DeltaPosition);
}

/* UMovieSceneSection structors
*****************************************************************************/

UKMovieSceneActorEventSection::UKMovieSceneActorEventSection()
{
#if WITH_EDITORONLY_DATA
	bIsInfinite_DEPRECATED = true;
#endif
	bSupportsInfiniteRange = true;
	SetRange(TRange<FFrameNumber>::All());

#if WITH_EDITOR

	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(EventData, FMovieSceneChannelMetaData());

#else

	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(EventData);

#endif
}

void UKMovieSceneActorEventSection::PostLoad()
{
	if (Events_DEPRECATED.GetKeys().Num())
	{
		TMovieSceneChannelData<FKSequencerSubtitleEventStruct> ChannelData = EventData.GetData();

		FFrameRate LegacyFrameRate = GetLegacyConversionFrameRate();

		for (auto EventKey : Events_DEPRECATED.GetKeys())
		{
			FFrameNumber KeyTime = UpgradeLegacyMovieSceneTime(this, LegacyFrameRate, EventKey.Time);
			ChannelData.AddKey(KeyTime, FKSequencerSubtitleEventStruct());
		}

		MarkAsChanged();
	}

	Super::PostLoad();
}

TSharedPtr<FStructOnScope> FKMovieSceneActorEventSectionData::GetKeyStruct(TMovieSceneChannelHandle<FKMovieSceneActorEventSectionData> Channel, FKeyHandle InHandle)
{
	int32 KeyValueIndex = Channel.Get()->GetData().GetIndex(InHandle);
	return TSharedPtr<FStructOnScope>();
}

UKMovieSceneSubtitleEventSection::UKMovieSceneSubtitleEventSection()
{
#if WITH_EDITORONLY_DATA
	bIsInfinite_DEPRECATED = true;
#endif
	bSupportsInfiniteRange = false;

	SetRange(TRange<FFrameNumber>::All());

#if WITH_EDITOR

	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(EventData, FMovieSceneChannelMetaData());

#else

	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(EventData);

#endif
}

void UKMovieSceneSubtitleEventSection::PostLoad()
{
	if (Events_DEPRECATED.GetKeys().Num())
	{
		TMovieSceneChannelData<FKSequencerSubtitleEventStruct> ChannelData = EventData.GetData();

		FFrameRate LegacyFrameRate = GetLegacyConversionFrameRate();

		for (auto EventKey : Events_DEPRECATED.GetKeys())
		{
			FFrameNumber KeyTime = UpgradeLegacyMovieSceneTime(this, LegacyFrameRate, EventKey.Time);
			ChannelData.AddKey(KeyTime, FKSequencerSubtitleEventStruct());
		}

		MarkAsChanged();
	}

	Super::PostLoad();
}

#if WITH_EDITOR

void UKMovieSceneSubtitleEventSection::SetRange_Override(const TRange<FFrameNumber>& NewRange)
{
	// Do not modify for objects that still need initialization (i.e. we're in the object's constructor)
	bool bCanSetRange = HasAnyFlags(RF_NeedInitialization) || TryModify();
	if (bCanSetRange)
	{
		check(NewRange.GetLowerBound().IsOpen() || NewRange.GetUpperBound().IsOpen() || NewRange.GetLowerBoundValue() <= NewRange.GetUpperBoundValue());

		TMovieSceneChannelData<FKSequencerSubtitleEventStruct> ChannelData = EventData.GetData();
		
		// 키가 없으면(섹션 생성시) 새 키 추가하기
		if (EventData.GetNumKeys() == 0)
		{
			// LowerBound가 무한(Open)이면 0번에 추가
			if (NewRange.GetLowerBound().IsOpen())
			{
				ChannelData.AddKey(0, FKSequencerSubtitleEventStruct());
			}
			else
			{
				ChannelData.AddKey(NewRange.GetLowerBoundValue(), FKSequencerSubtitleEventStruct());
			}

		}
		// 키가 있었으면 변경된 섹션 길이 적용해주기
		else
		{
			TArray<FKeyHandle> KeyHandles;
			ChannelData.GetKeys(TRange<FFrameNumber>::All(), nullptr, &KeyHandles);

			if (KeyHandles.Num() == 1)
			{
				// 기존 키 삭제 후 Duration, 위치(section 가장 앞=LowerBound) 적용된 키 추가해주기
				FKSequencerSubtitleEventStruct SubtitleEventData = EventData.GetKeyValues()[0];

				UMovieScene* OuterMovieScene = this->GetTypedOuter<UMovieScene>();
				if (OuterMovieScene)
				{
					ChannelData.DeleteKeys(KeyHandles);

					if (NewRange.HasLowerBound())
					{
						SubtitleEventData.Duration = OuterMovieScene->GetTickResolution().AsSeconds(NewRange.Size<FFrameTime>());
						ChannelData.AddKey(NewRange.GetLowerBoundValue(), SubtitleEventData);
					}
					// LowerBound가 무한(Open)이면 0번에 추가
					else
					{
						SubtitleEventData.Duration = OuterMovieScene->GetTickResolution().AsSeconds(SectionRange.Value.GetUpperBoundValue());
						ChannelData.AddKey(0, FKSequencerSubtitleEventStruct());
					}

					// 변경된 duration 섹션 프로퍼티에도 적용
					SubtitleDetail.Duration = SubtitleEventData.Duration;
				}
			}
		}

		// 섹션 프로퍼티에도 변경 사항 적용 (자막 시작 시간 변경)
		if (NewRange.HasLowerBound())
		{
			SubtitleStartTime.Value = NewRange.GetLowerBoundValue().Value;
		}
		else
		{
			SubtitleStartTime.Value = 0;
		}

		SectionRange.Value = NewRange;
	}
}

void UKMovieSceneSubtitleEventSection::MoveSection_Override()
{
	// 움직인 시작 지점을 섹션이 가진 자막 이벤트 시작 시간으로 변경해줌 (키 이벤트 시간은 알아서 변경됨)
	if (SectionRange.Value.HasLowerBound())
	{
		SubtitleStartTime = SectionRange.Value.GetLowerBoundValue();
	}
	else
	{
		SubtitleStartTime = 0;

		// 좌측이 open인 경우 직접 바꿔줘야함
		TMovieSceneChannelData<FKSequencerSubtitleEventStruct> ChannelData = EventData.GetData();

		TArray<FKeyHandle> KeyHandles;
		ChannelData.GetKeys(TRange<FFrameNumber>::All(), nullptr, &KeyHandles);

		if (KeyHandles.Num() == 1)
		{
			// 기존 키 삭제 후 Duration, 위치(section 가장 앞=LowerBound) 적용된 키 추가해주기
			FKSequencerSubtitleEventStruct SubtitleEventData = EventData.GetKeyValues()[0];

			UMovieScene* OuterMovieScene = this->GetTypedOuter<UMovieScene>();
			if (OuterMovieScene)
			{
				SubtitleEventData.Duration = OuterMovieScene->GetTickResolution().AsSeconds(SectionRange.Value.GetUpperBoundValue());

				ChannelData.DeleteKeys(KeyHandles);

				ChannelData.AddKey(0, FKSequencerSubtitleEventStruct());

				// 변경된 duration 섹션 프로퍼티에도 적용
				SubtitleDetail.Duration = SubtitleEventData.Duration;
			}
		}
	}
}


void UKMovieSceneSubtitleEventSection::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);

	FName PropertyName = e.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(FFrameNumber, Value))
	{
		//// 새로 들어온 시간으로 섹션 옮겨주기
		MoveSection(SubtitleStartTime.Value);

		// SectionRange 변경(길이만 유지)
		FFrameNumber SectionRangeSize = SectionRange.Value.Size<FFrameNumber>();

		SectionRange.Value.SetLowerBoundValue(SubtitleStartTime);
		SectionRange.Value.SetUpperBoundValue(SubtitleStartTime + SectionRangeSize);

	}
	else if (PropertyName == TEXT("SectionRange"))
	{
		// 그려지는 Range 변경 (FSequencerSection에서 해줌)
		// duration 변경

		TMovieSceneChannelData<FKSequencerSubtitleEventStruct> ChannelData = EventData.GetData();

		TArray<FKeyHandle> KeyHandles;
		ChannelData.GetKeys(TRange<FFrameNumber>::All(), nullptr, &KeyHandles);

		if (KeyHandles.Num() == 1)
		{
			// 기존 키 삭제 후 Duration, 위치(section 가장 앞=LowerBound) 적용된 키 추가해주기
			FKSequencerSubtitleEventStruct SubtitleEventData = EventData.GetKeyValues()[0];

			UMovieScene* OuterMovieScene = this->GetTypedOuter<UMovieScene>();
			if (OuterMovieScene)
			{
				SubtitleEventData.Duration = OuterMovieScene->GetTickResolution().AsSeconds(SectionRange.Value.Size<FFrameTime>());

				ChannelData.DeleteKeys(KeyHandles);

				ChannelData.AddKey(SectionRange.Value.GetLowerBoundValue(), FKSequencerSubtitleEventStruct());

				// 변경된 duration 섹션 프로퍼티에도 적용
				SubtitleDetail.Duration = SubtitleEventData.Duration;
			}
		}

		// StartTime 변경
		SubtitleStartTime = SectionRange.Value.GetLowerBoundValue();
	}
	// detail만 수정 -> 키 값만 변경해줌
	else
	{		
		if (PropertyName == GET_MEMBER_NAME_CHECKED(FKSequencerSubtitleEventStruct, Duration))
		{
			// 키 내부 duration 값 변경
			TMovieSceneChannelData<FKSequencerSubtitleEventStruct> ChannelData = EventData.GetData();

			TArray<FKeyHandle> KeyHandles;
			ChannelData.GetKeys(TRange<FFrameNumber>::All(), nullptr, &KeyHandles);

			if (KeyHandles.Num() > 0)
			{
				FKSequencerSubtitleEventStruct SubtitleEventData = EventData.GetKeyValues()[0];
				
				SubtitleEventData.SubtitlePosition = SubtitleDetail.SubtitlePosition;
				SubtitleEventData.SubtitleSlotWidget = SubtitleDetail.SubtitleSlotWidget;

				SubtitleEventData.SubtitleKey = SubtitleDetail.SubtitleKey;
				SubtitleEventData.Duration = SubtitleDetail.Duration;

				SubtitleEventData.bUseAlphaBlend = SubtitleDetail.bUseAlphaBlend;
				SubtitleEventData.AlphaBlendSpeed = SubtitleDetail.AlphaBlendSpeed;

				FFrameNumber SubtitleEventTime = EventData.GetKeyTimes()[0];

				// 기존 키 삭제
				EventData.DeleteKeys(KeyHandles);

				// 섹션이 가지는 SectionRange 변경
				UMovieScene* OuterMovieScene = this->GetTypedOuter<UMovieScene>();
				if (OuterMovieScene)
				{
					if (SectionRange.Value.HasLowerBound())
					{
						// Duration을 변경한 새 키 삽입
						ChannelData.AddKey(SubtitleEventTime, SubtitleEventData);

						SectionRange.Value.SetUpperBoundValue(SectionRange.GetLowerBound().GetValue() + OuterMovieScene->GetTickResolution().AsFrameNumber(SubtitleDetail.Duration));
					}
					else
					{
						ChannelData.AddKey(0, SubtitleEventData);
						SectionRange.Value.SetUpperBoundValue(OuterMovieScene->GetTickResolution().AsFrameNumber(SubtitleDetail.Duration));
					}
				}
			}
		}
		else// if (PropertyName == GET_MEMBER_NAME_CHECKED(FKSequencerSubtitleEventStruct, SubtitleKey))
		{
			TMovieSceneChannelData<FKSequencerSubtitleEventStruct> ChannelData = EventData.GetData();

			TArray<FKeyHandle> KeyHandles;
			ChannelData.GetKeys(TRange<FFrameNumber>::All(), nullptr, &KeyHandles);

			if (KeyHandles.Num() > 0)
			{
				FKSequencerSubtitleEventStruct SubtitleEventData = EventData.GetKeyValues()[0];

				SubtitleEventData.SubtitlePosition = SubtitleDetail.SubtitlePosition;
				SubtitleEventData.SubtitleSlotWidget = SubtitleDetail.SubtitleSlotWidget;

				SubtitleEventData.SubtitleKey = SubtitleDetail.SubtitleKey;
				SubtitleEventData.Duration = SubtitleDetail.Duration;

				SubtitleEventData.bUseAlphaBlend = SubtitleDetail.bUseAlphaBlend;
				SubtitleEventData.AlphaBlendSpeed = SubtitleDetail.AlphaBlendSpeed;

				FFrameNumber SubtitleEventTime = EventData.GetKeyTimes()[0];

				// 기존 키 삭제
				EventData.DeleteKeys(KeyHandles);

				// Duration을 변경한 새 키 삽입
				ChannelData.AddKey(SubtitleEventTime, SubtitleEventData);
			}
		}
	}

}
#endif