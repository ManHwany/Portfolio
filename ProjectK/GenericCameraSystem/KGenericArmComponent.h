// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraUtil/Define/KCameraDefine.h"
#include "CameraUtil/GenericCameraSystem/ArmComponent/KGenericArmPoint.h"
#include "Components/SceneComponent.h"
#include "KGenericArmComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KGAME_API UKGenericArmComponent : public USceneComponent
{
	GENERATED_BODY()

public:		
	UKGenericArmComponent();

protected:	
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	bool bRootGenericArm = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	TMap<E_KGenericArmDirection, FKGenericArmPoint> ArmPoints;

public:
	void AddArmPoint(E_KGenericArmDirection key, FKGenericArmPoint armPoint);
	void SetArmPointWeightValue(float newValue, E_KGenericArmDirection key = E_KGenericArmDirection::None);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		

public:
	UFUNCTION(BlueprintPure)
	const TMap<E_KGenericArmDirection, FKGenericArmPoint>& GetArmPoints();

	UFUNCTION(BlueprintPure)
	const FKGenericArmPoint& GetArmPoint(E_KGenericArmDirection key);

	UFUNCTION(BlueprintPure)
	const bool IsRootGenericArm() { return bRootGenericArm; }
};