#include "KGenericArmComponent.h"
#include "GameFramework/Actor.h"
#include "Core/KLog.h"
#include "Core/KTextHelper.h"

UKGenericArmComponent::UKGenericArmComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;

	FKGenericArmPoint armPoint;
	armPoint.RelativeRotation = FRotator(-90.f, 0.f, 0.f);
	AddArmPoint(E_KGenericArmDirection::Top, armPoint);

	armPoint.RelativeRotation = FRotator(90.f, 0.f, 0.f);
	AddArmPoint(E_KGenericArmDirection::Bottom, armPoint);

	armPoint.RelativeRotation = FRotator(0.f, -90.f, 0.f);
	AddArmPoint(E_KGenericArmDirection::Right, armPoint);

	armPoint.RelativeRotation = FRotator(0.f, 90.f, 0.f);
	AddArmPoint(E_KGenericArmDirection::Left, armPoint);

	armPoint.RelativeRotation = FRotator(180.f, 0.f, 0.f);
	AddArmPoint(E_KGenericArmDirection::Forward, armPoint);

	armPoint.RelativeRotation = FRotator(-0.f, 0.f, 0.f);
	AddArmPoint(E_KGenericArmDirection::Back, armPoint);
}

void UKGenericArmComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKGenericArmComponent::AddArmPoint(E_KGenericArmDirection key, FKGenericArmPoint armPoint)
{
	if (ArmPoints.Contains(key))
		return;

	ArmPoints.Add(key, armPoint);
}

void UKGenericArmComponent::OnRegister()
{
	for (auto& armPoint : ArmPoints)
	{
		armPoint.Value.SetOwner(this, GetOwner());
		armPoint.Value.Initialize();
	}

	Super::OnRegister();
}

void UKGenericArmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	for (auto& armPoint : ArmPoints)
	{		
		armPoint.Value.Update(DeltaTime);
	}	

	UpdateChildTransforms();
}

const TMap<E_KGenericArmDirection, FKGenericArmPoint>& UKGenericArmComponent::GetArmPoints()
{ 
	return ArmPoints; 
}

const FKGenericArmPoint& UKGenericArmComponent::GetArmPoint(E_KGenericArmDirection key)
{
	auto armPoint = ArmPoints.Find(key);
	checkf(armPoint, TEXT("'%s' arm point is not exist."), *FKTextHelper::EnumToString("E_KGenericArmDirection", key));
	return *armPoint;
}

void UKGenericArmComponent::SetArmPointWeightValue(float newValue, E_KGenericArmDirection key)
{	
	if (key == E_KGenericArmDirection::None)
	{
		for (auto& armPoint : ArmPoints)
		{
			armPoint.Value.WeightValue = newValue;
		}
	}
	else
	{
		if (auto armPoint = ArmPoints.Find(key))
		{
			armPoint->WeightValue = newValue;
		}
	}
}