#include "KGenericArmPoint.h"

#include "Engine/World.h"
#include "Engine/Public/DrawDebugHelpers.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

#include "GameModule/KAte/Mvc/ModelView/Component/KAteAgentComponent.h"

#include "Components/SceneComponent.h"
#include "Curves/CurveFloat.h"

#include "Core/KMath.h"

void FKGenericArmPoint::SetOwner(USceneComponent* component, AActor* actor)
{
	OwnerComponent = component;
	OwnerActor = actor;

	if (!OwnerActor.IsValid())
	{
		return;
	}

	if (UKAteAgentComponent* agentComp = Cast<UKAteAgentComponent>(OwnerActor->GetComponentByClass(UKAteAgentComponent::StaticClass())))
	{
		const float agentScale = agentComp->GetAgentScale();
		OwnerActorScale = agentScale == 0 ? 1.f : agentScale;
	}

	// [6/4/2020 jhlee01] Owner 스케일에 맞게 자동으로 Length 늘려주기.
	ArmLength *= OwnerActorScale;
}

void FKGenericArmPoint::Initialize()
{	
	LagMaxTimeStep = 1.f / 60.f;
	LagMaxDistance = 0.f;
}

void FKGenericArmPoint::AddRotation(FRotator rotation, float blendTime, UCurveFloat* blendCurve)
{
	bDoRotation = true;
	BlendTime = blendTime;
	BlendTimeToGo = blendTime;
	StartRot = RelativeRotation;
	GoalRot = StartRot + rotation;
	BlendCurve = blendCurve;
}

void FKGenericArmPoint::Update(float deltaTime)
{
	if (OwnerActor == nullptr)
		return;	

	FRotator desiredRot = FRotator::ZeroRotator;
	//FRotator LookAtCameraRot = FRotator::ZeroRotator;

	//if (APController* PlayerController = Cast<APController>(OwnerActor->GetWorld()->GetFirstPlayerController()))
	//{
	//	if (AMainCamera* MainCamera = PlayerController->GetCameraManager()->GetMainCamera())
	//	{
	//		//LookAtCameraRot = UKismetMathLibrary::FindLookAtRotation(*GetLocation(), MainCamera->GetActorLocation());
	//		LookAtCameraRot = UKismetMathLibrary::MakeRotFromX(MainCamera->GetActorForwardVector() * -1);
	//		LookAtCameraRot.Pitch = 0.f;
	//		LookAtCameraRot.Roll = 0.f;
	//		//HWAN_ERROR(TEXT("%f, %f, %f"), LookAtCameraRot.Pitch, LookAtCameraRot.Yaw, LookAtCameraRot.Roll);

	//		RelativeRotation += LookAtCameraRot;			
	//	}
	//}

	// 회전이 종속된 Owner 에 의존적일 경우
	if (bUseDependenceOfOwnerRotation)
	{
		desiredRot = OwnerActor->GetActorRotation() + RelativeRotation;
	}
	else
	{
		desiredRot = RelativeRotation;
	}

	if (bDoRotation)
	{
		BlendTimeToGo -= deltaTime;

		if (BlendTimeToGo <= 0)
		{
			BlendTimeToGo = 0.f;		
			BlendCurve = nullptr;
			bDoRotation = false;
		}

		float blendPct = 0.f;
		float blendAlpha = (BlendTime - BlendTimeToGo) / BlendTime;
		if (BlendCurve != nullptr)
		{
			blendPct = BlendCurve->GetFloatValue(blendAlpha);
		}
		else
		{
			// 커브 없으면, 선형보간으로.
			blendPct = FMath::Lerp(0.f, 1.f, blendAlpha);
		}
			   		 		
		RelativeRotation = FKMath::RLerp(StartRot, GoalRot, blendPct, false);		
	}
	
	if (bEnableCameraRotationLag)
	{
		if (deltaTime > LagMaxTimeStep && RotationLagSpeed > 0.f)
		{
			const FRotator ArmRotStep = (desiredRot - PreviousDesiredRot).GetNormalized() * (1.f / deltaTime);
			FRotator LerpTarget = PreviousDesiredRot;
			float RemainingTime = deltaTime;
			while (RemainingTime > KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(LagMaxTimeStep, RemainingTime);
				LerpTarget += ArmRotStep * LerpAmount;
				RemainingTime -= LerpAmount;

				desiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(LerpTarget), LerpAmount, RotationLagSpeed));
				PreviousDesiredRot = desiredRot;
			}
		}
		else
		{
			desiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(desiredRot), deltaTime, RotationLagSpeed));
		}
		PreviousDesiredRot = desiredRot;
	}

	// Get the spring arm 'origin', the target we want to look at
	FVector armOrigin = OwnerComponent->GetComponentLocation() + Offset;
	// We lag the target, not the actual camera position, so rotating the camera around does not have lag
	FVector desiredLoc = armOrigin;

	if (bEnableCameraLag)
	{
		if (deltaTime > LagMaxTimeStep && LagSpeed > 0.f)
		{
			const FVector armMovementStep = (desiredLoc - PreviousDesiredLoc) * (1.f / deltaTime);
			FVector LerpTarget = PreviousDesiredLoc;

			float RemainingTime = deltaTime;
			while (RemainingTime > KINDA_SMALL_NUMBER)
			{
				const float lerpAmount = FMath::Min(LagMaxTimeStep, RemainingTime);
				LerpTarget += armMovementStep * lerpAmount;
				RemainingTime -= lerpAmount;

				desiredLoc = FMath::VInterpTo(PreviousDesiredLoc, LerpTarget, lerpAmount, LagSpeed);
				PreviousDesiredLoc = desiredLoc;
			}
		}
		else
		{
			desiredLoc = FMath::VInterpTo(PreviousDesiredLoc, desiredLoc, deltaTime, LagSpeed);
		}

		// Clamp distance if requested
		bool bClampedDist = false;
		if (LagMaxDistance > 0.f)
		{
			const FVector fromOrigin = desiredLoc - armOrigin;
			if (fromOrigin.SizeSquared() > FMath::Square(LagMaxDistance))
			{
				desiredLoc = armOrigin + fromOrigin.GetClampedToMaxSize(LagMaxDistance);
				bClampedDist = true;
			}
		}
	}

//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
//	DrawDebugSphere(OwnerActor->GetWorld(), ArmOrigin, 5.f, 8, FColor::Green);
//	DrawDebugSphere(OwnerActor->GetWorld(), DesiredLoc, 5.f, 8, FColor::Yellow);
//
//	const FVector ToOrigin = ArmOrigin - DesiredLoc;
//	DrawDebugDirectionalArrow(OwnerActor->GetWorld(), DesiredLoc, DesiredLoc + ToOrigin * 0.5f, 7.5f, bClampedDist ? FColor::Red : FColor::Green);
//	DrawDebugDirectionalArrow(OwnerActor->GetWorld(), DesiredLoc + ToOrigin * 0.5f, ArmOrigin, 7.5f, bClampedDist ? FColor::Red : FColor::Green);
//#endif	

	PreviousArmOrigin = armOrigin;
	PreviousDesiredLoc = desiredLoc;

	// Now offset camera position back along our rotation
	desiredLoc -= desiredRot.Vector() * ArmLength;
	// Add socket offset in local space
	desiredLoc += FRotationMatrix(desiredRot).TransformVector(Offset);

	// Do a sweep to ensure we are not penetrating the world	
	ArmLocation = desiredLoc;

#if WITH_ENGINE
	if ((OwnerActor->GetWorld() && (OwnerActor->GetWorld()->WorldType != EWorldType::Game && OwnerActor->GetWorld()->WorldType != EWorldType::PIE)))
	{		
		DrawDebugSphere(OwnerActor->GetWorld(), ArmLocation, 5.f, 8, FColor::Red);
		DrawDebugLine(OwnerActor->GetWorld(), armOrigin, ArmLocation, FColor::Green);
	}
#endif
}

const FVector FKGenericArmPoint::GetOriginLocation()
{
	return OwnerComponent->GetComponentLocation() + Offset;
}

const FVector FKGenericArmPoint::GetDirectionVector()
{
	return (GetLocation() - GetOriginLocation()).GetSafeNormal();
}