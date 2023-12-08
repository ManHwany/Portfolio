// Fill out your copyright notice in the Description page of Project Settings.
#include "KProjectileMovementComponent.h"

#include "Holder/KMvcHolder.h"
#include "GameModule/KAte/Interface/IKAteMv.h"
#include "GameModule/KAte/Mvc/ModelView/Animation/KAteAnimDef.h"
#include "GameModule/KAte/Mvc/ModelView/Helper/KAteMissileLauncher.h"
#include "GameModule/KAte/Mvc/ModelView/Actor/Missile/KAteMissile.h"

#include "Components/MeshComponent.h"
#include "Curves/CurveFloat.h"

#include "Core/KLog.h"
#include "Core/KMath.h"

#include "Engine/Public/DrawDebugHelpers.h"

UKProjectileMovementComponent::UKProjectileMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

#if WITH_EDITOR
void UKProjectileMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName propertyName = PropertyChangedEvent.GetPropertyName();

	if (propertyName.IsEqual("bUseBlendTime"))
	{
		bUseMovementSpeed = !bUseBlendTime;
	}
	else if (propertyName.IsEqual("bUseMovementSpeed"))
	{
		bUseBlendTime = !bUseMovementSpeed;
	}
}
#endif	

void UKProjectileMovementComponent::OnRegister()
{
	Super::OnRegister();
	OwnerActor = GetOwner();
}

void UKProjectileMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	TotalCurveArea = calcTotalCurveArea(SpeedCurveFloat);
}

void UKProjectileMovementComponent::MoveToLocation(const FVector& location)
{
	DesiredLocation = location;

	init();
}

void UKProjectileMovementComponent::MoveToActor(AActor* targetActor, FName socketName /**= "Root" */, const FVector locationOffset /** = FVector::ZeroVector*/)
{
	if (!targetActor)
		return;

	TargetActor = targetActor;
	SocketName = socketName;
	OffsetLocation = locationOffset;
	DesiredLocation = targetActor->GetActorLocation();
	TargetMeshComp = findTargetSkeletalMesh(targetActor);

	init();
	setTargetSocketInfo();
}

void UKProjectileMovementComponent::MoveToComponent(USceneComponent * targetComponent, FName socketName /**= NAME_None */, const FVector locationOffset /** = FVector::ZeroVector*/)
{
	if (!targetComponent)
		return;

	TargetComponent = targetComponent;
	SocketName = socketName;
	OffsetLocation = locationOffset;
	DesiredLocation = TargetComponent->GetComponentLocation();
	TargetMeshComp = findTargetSkeletalMesh(targetComponent);

	init();
	setTargetSocketInfo();
}

void UKProjectileMovementComponent::init()
{
	OwnerActor = GetOwner();
	if (!OwnerActor.IsValid())
	{
		return;
	}

	StartLocation = OwnerActor->GetActorLocation();

	BlendTime = bUseRandomBlendTime ? FMath::RandRange(BlendTimeMinMax.X, BlendTimeMinMax.Y) : BlendTime;
	MovementSpeed = bUseRandomSpeed ? FMath::RandRange(MovementSpeedMinMax.X, MovementSpeedMinMax.Y) : MovementSpeed;
	Amplitude = bUseRandomAmplitude ? FMath::RandRange(AmplitudeMinMax.X, AmplitudeMinMax.Y) : Amplitude;
	FireAngle = bUseRandomFireAngle ? FMath::RandRange(FireAngleMinMax.X, FireAngleMinMax.Y) : FireAngle;

	BlendTimeToGo = BlendTime;
	bStartMove = true;

	bCasterOnLeftFromTarget = IsCasterOnLeftFromDesiredLocation();
}

void UKProjectileMovementComponent::MovementTick(float deltaTime)
{
	if (!bStartMove)
	{
		return;
	}

	if (!OwnerActor.IsValid() || OwnerActor->IsPendingKill())
	{
		return;
	}

	const float prevElapsedTime = ElapsedTime;
	ElapsedTime += deltaTime;

	//도착위치 계산.
	DesiredLocation = calcDesireLocation();

	// 진행방향이 곧 forward 임.
	const FVector forwardDir = (DesiredLocation - StartLocation).GetSafeNormal();

	// 파형을 추가할 방향이다. forward 축을 기반으로, 설정된 발사각으로 파형을 만들어야함.
	const FVector waveformDir = FVector::UpVector.RotateAngleAxis(FireAngle, forwardDir);

	// 매틱 블렌딩 위치
	FVector resultLocation = FVector::ZeroVector;

	// ** 속도 기반 발세체 일 경우 **
	if (bUseMovementSpeed)
	{
		// 시간 = 거리 / 속도
		BlendTime = FVector::Dist(DesiredLocation, StartLocation) / MovementSpeed;

		if (bUseSpeedCurve  && SpeedCurveFloat)
		{
			if (TotalCurveArea > 0.f)
			{
				BlendTime = BlendTime * (1.f / TotalCurveArea);
			}
		}

		// 경과시간이 예측된 블렌딩 시간보다 커질경우 도착했다고 판단.
		if (ElapsedTime >= BlendTime)
		{
			OwnerActor->SetActorLocation(DesiredLocation);
			clear();
			return;
		}
		else
		{
			const float alpha = ElapsedTime / BlendTime;

			if (bUseSpeedCurve && SpeedCurveFloat)
			{
// 				const float prevAlpha = prevElapsedTime / BlendTime;
// 				const float moveDistance = calcCurveArea(SpeedCurveFloat, prevAlpha, alpha, GetWorld()->GetDeltaSeconds());
// 				// 위치 = 이전 위치 + 이번 틱에 이동한 거리
// 				resultLocation = OwnerActor->GetActorLocation() + forwardDir * (moveDistance * MovementSpeed * BlendTime);

				const float moveDistance = calcCurveArea(SpeedCurveFloat, 0.f, alpha, GetWorld()->GetDeltaSeconds());
				// 위치 = 시작위치 + (시작위치 기준 - 커브 이동거리) * 속도 * 총 시간
				resultLocation = StartLocation + forwardDir * (moveDistance * MovementSpeed * BlendTime);
			}
			else
			{
				// 위치 = 시작위치 + (속도 * 경과 시간)
				resultLocation = StartLocation + (forwardDir * MovementSpeed* ElapsedTime);
			}

			// 포물선 커브가 존재 할 경우,
			if (bUseParabolaMove && AmplitudeCurveFloat != nullptr)
			{
				const float moveMentOffset = AmplitudeCurveFloat->GetFloatValue(alpha) * Amplitude;
				resultLocation += waveformDir * moveMentOffset;
			}

			// 가이드 포물선 무브먼트 적용
			resultLocation += getAddedGuidedCurveMovement(alpha);
		}
	}
	// ** 시간 기반 발세체 일 경우 **
	else
	{
		BlendTimeToGo -= deltaTime;

		if (BlendTimeToGo > 0.f)
		{
			const float blendPct = getBlendPct(BlendTimeToGo, deltaTime);
			const float v = FVector::Dist(DesiredLocation, StartLocation) * blendPct / ElapsedTime;
			const float alpha = (BlendTime - BlendTimeToGo) / BlendTime;

			resultLocation = StartLocation + (forwardDir * (v * ElapsedTime));

			// 포물선 커브가 존재 할 경우,
			if (bUseParabolaMove && AmplitudeCurveFloat != nullptr)
			{
				const float moveMentOffset = AmplitudeCurveFloat->GetFloatValue(alpha) * Amplitude;
				resultLocation += waveformDir * moveMentOffset;
			}

			// 가이드 포물선 무브먼트 적용
			resultLocation += getAddedGuidedCurveMovement(alpha);
		}
		else
		{
			OwnerActor->SetActorLocation(DesiredLocation);
			clear();
			return;
		}
	}

	// 발사체가 바라보는 방향은, 현재 위치에서 다음틱의 위치로 바라보면 됨.
	const FRotator lookAtRot = FKMath::MakeLookAtRotation(OwnerActor->GetActorLocation(), resultLocation);

	OwnerActor->SetActorRotation(lookAtRot);
	OwnerActor->SetActorLocation(resultLocation);
}

const FVector UKProjectileMovementComponent::getAddedGuidedCurveMovement(const float alpha)
{
	if (bUseGuidedParabola && GuidedCurveFloat)
	{
		const float dotValueAbs = FMath::Abs(DotProductResult);

		// [5/20/2020 jhlee01] 왼쪽 오른쪽 가릴필요없이, 거의 수평으로 서있다면 보정 하지말고.
		if (dotValueAbs < 0.1f)
		{
			return FVector::ZeroVector;
		}

		// 왼쪽에 있으면, 옆으로 눕힐거니 90도 출발.
		const float guidedFireAngle = 90.f;

		const FVector forwardDir = (DesiredLocation - StartLocation).GetSafeNormal();

		const FVector guidedformDir = FVector::UpVector.RotateAngleAxis(guidedFireAngle, forwardDir);

		// 내적 값에 의존하여 휘는정도를 세팅한다.
		const float moveMentOffset = GuidedCurveFloat->GetFloatValue(alpha) * DotProductResult * GuidedAmplitude;

		return guidedformDir * moveMentOffset;
	}

	return FVector::ZeroVector;
}

USkeletalMeshComponent * UKProjectileMovementComponent::findTargetSkeletalMesh(AActor * targetActor)
{
	if (!IsValid(targetActor))
	{
		return nullptr;
	}
	USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(targetActor->GetComponentByClass(UMeshComponent::StaticClass()));
	return mesh;
}

USkeletalMeshComponent * UKProjectileMovementComponent::findTargetSkeletalMesh(USceneComponent * targetComp)
{
	AActor* ownerActor = targetComp->GetOwner();
	return findTargetSkeletalMesh(ownerActor);
}

float UKProjectileMovementComponent::getBlendPct(const float blendTimeToGo, const float deltaTime)
{
	float blendPct = 0.f;
	float blendTime = BlendTime;
	float DurationPct = (blendTime - blendTimeToGo) / blendTime;

	if (bUseSpeedCurve && SpeedCurveFloat)
	{
		blendPct = SpeedCurveFloat->GetFloatValue(DurationPct);
	}
	else
	{
		switch (SpeedBlendFunction)
		{
			case EViewTargetBlendFunction::VTBlend_Linear:
				blendPct = FMath::Lerp(0.f, 1.f, DurationPct);
				break;
			case EViewTargetBlendFunction::VTBlend_Cubic:
				blendPct = FMath::CubicInterp(0.f, 0.f, 1.f, 0.f, DurationPct);
				break;
			case EViewTargetBlendFunction::VTBlend_EaseIn:
				blendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, SpeedBlendExp));
				break;
			case EViewTargetBlendFunction::VTBlend_EaseOut:
				blendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 1.f / SpeedBlendExp));
				break;
			case EViewTargetBlendFunction::VTBlend_EaseInOut:
				blendPct = FMath::InterpEaseInOut(0.f, 1.f, DurationPct, SpeedBlendExp);
				break;
			default:
				break;
		}
	}

	return blendPct;
}

void UKProjectileMovementComponent::clear()
{
	bStartMove = false;
	OnCompletedMovement.ExecuteIfBound(DesiredLocation);
}

void UKProjectileMovementComponent::SetCompletedCallback(FKOnCompletedMovement onCompleted)
{
	OnCompletedMovement = onCompleted;
}

const FVector UKProjectileMovementComponent::getSocketLocation(TWeakObjectPtr<AActor> actor, FName socketName)
{
	FVector resultLocation = FVector::ZeroVector;

	if (actor != nullptr)
	{
		if (UMeshComponent* mesh = Cast<UMeshComponent>(actor->GetComponentByClass(UMeshComponent::StaticClass())))
		{
			resultLocation = mesh->GetSocketLocation(socketName);
		}
	}

	return resultLocation;
}

FVector UKProjectileMovementComponent::getLandLocation(TWeakObjectPtr<USceneComponent> component)
{
	if (!component.IsValid())
		return FVector::ZeroVector;

	FHitResult hitResult;
	const FVector componentLoc = component->GetComponentLocation();

	//
	if (component->GetWorld()->LineTraceSingleByChannel(hitResult, componentLoc + (FVector::UpVector * 1000.f), componentLoc - (FVector::UpVector * 1000.f), ECC_GameTraceChannel9))
	{
		return hitResult.ImpactPoint;
	}

	return component->GetComponentLocation();
}

bool UKProjectileMovementComponent::IsCasterOnLeftFromDesiredLocation()
{
	auto missile = Cast<AKAteMissile>(OwnerActor);

	if (!missile)
	{
		return false;
	}

	auto caster = missile->GetCasterActor();

	if (!caster.IsValid())
	{
		return false;
	}

	const FVector upVector = FVector::UpVector;
	const FVector casterForward = caster->GetActorForwardVector();
	const FVector toCasterDir = (caster->GetActorLocation() - DesiredLocation).GetSafeNormal();

	DotProductResult = FVector::DotProduct(upVector, FVector::CrossProduct(toCasterDir, casterForward));
	GuidedAmplitude = bUseRandomGuidedAmplitude ? FMath::RandRange(GuidedAmplitudeMinMax.X, GuidedAmplitudeMinMax.Y) : GuidedAmplitude;

	return DotProductResult < 0 ? false : true;
}

void UKProjectileMovementComponent::setTargetSocketInfo()
{
	bRootSocket = SocketName.IsEqual(KAteAgentSocketName::Root);
	if(bRootSocket)
	{
		//타겟 소켓의 이름이 Root일때 RootOverride 소켓이있다면 해당 소켓으로 타겟을 덮어 씌운다.
		if (TargetMeshComp.IsValid())
		{
			bRootOverrideSocket = TargetMeshComp->DoesSocketExist(KAteAgentSocketName::Root_Override);
			if (bRootOverrideSocket)
			{
				SocketName = KAteAgentSocketName::Root_Override;
			}
		}
	}
}


float UKProjectileMovementComponent::calcTotalCurveArea(UCurveFloat* curveFloat)
{
	if (curveFloat == nullptr)
		return 0.f;

	float minTime = 0.f;
	float maxTime = 0.f;

	curveFloat->GetTimeRange(minTime, maxTime);

	return calcCurveArea(curveFloat, minTime, maxTime, GetWorld()->GetDeltaSeconds());
}

float UKProjectileMovementComponent::calcCurveArea(UCurveFloat* curveFloat, float minTime, float maxTime, float deltaTime)
{
	if (curveFloat == nullptr)
		return 0.f;

	if (deltaTime <= 0.f)
		return 0.f;

	float area = 0.f;

	for (float time = minTime; time < maxTime; time += deltaTime)
	{
		const float nextTime = FMath::Min<float>(time + deltaTime, maxTime);
		const float curValue = curveFloat->GetFloatValue(time);

		area += curValue * (nextTime - time);
	}

	return area;
}

FVector UKProjectileMovementComponent::calcDesireLocation()
{
	FVector desireLoc = FVector::ZeroVector;
	if (TargetActor.IsValid() && !TargetActor->IsPendingKill())
	{
		// 타겟 액터가 존재하면, 목표위치는 타겟액터의 특정 소켓 부위 + Offset 이다.
		desireLoc = FKAteMissileLauncher::GetLocalTransform(TargetActor.Get(), SocketName, OffsetLocation).GetLocation();
	}
	else if (TargetComponent.IsValid() && !TargetComponent->IsPendingKill())
	{
		//타겟 컴포넌트가 존재할때.
		if (bRootOverrideSocket)
		{
			if (TargetMeshComp.IsValid())
			{
				const FTransform rootTr = TargetMeshComp->GetSocketTransform(KAteAgentSocketName::Root_Override); 
				FVector compLoc = TargetComponent->GetComponentTransform().GetLocation();
				compLoc.Z = rootTr.GetLocation().Z;
				desireLoc = compLoc + FKAteMissileLauncher::ChangeToLocalVector(TargetComponent.Get(), OffsetLocation);
			}
		}
		else if (bRootSocket)
		{
			//만약 소켓위치가 Root라면 바닥에 출력한다.
			desireLoc = getLandLocation(TargetComponent) + FKAteMissileLauncher::ChangeToLocalVector(TargetComponent.Get(), OffsetLocation);
		}
		else
		{
			desireLoc = TargetComponent->GetComponentTransform().GetLocation() + FKAteMissileLauncher::ChangeToLocalVector(TargetComponent.Get(), OffsetLocation);
		}
	}
	return desireLoc;
}
