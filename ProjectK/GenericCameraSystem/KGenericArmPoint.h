#pragma once
#include "CoreMinimal.h"
#include "KGenericArmPoint.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct FKGenericArmPoint
{
	GENERATED_USTRUCT_BODY()
	FKGenericArmPoint() {}

private:
	TWeakObjectPtr<AActor> OwnerActor;

	UPROPERTY()
	USceneComponent* OwnerComponent = nullptr;

	UPROPERTY()
	UCurveFloat* BlendCurve = nullptr;

private:
	FVector ArmLocation = FVector::ZeroVector;
	
	float LagMaxTimeStep = 0.f;	
	float LagMaxDistance = 0.f;

	FVector PreviousDesiredLoc = FVector::ZeroVector;
	FVector PreviousArmOrigin = FVector::ZeroVector;
	
	FRotator PreviousDesiredRot = FRotator::ZeroRotator;

	/** For Shpere Interpolation */
	bool bDoRotation = false;
	float BlendTime = 0.f;
	float BlendTimeToGo = 0.f;
	
	FRotator StartRot = FRotator::ZeroRotator;
	FRotator GoalRot = FRotator::ZeroRotator;

	float OwnerActorScale = 1.f;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ArmPoint")
	bool bUseDependenceOfOwnerRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ArmPoint")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ArmPoint")
	FRotator RelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ArmPoint")
	float ArmLength = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag")
	bool bEnableCameraLag = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag", meta = (ClampMin = "0.0", ClampMax = "1000.0", editcondition = "bEnableCameraLag"))
	float LagSpeed = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag")
	bool bEnableCameraRotationLag = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lag", meta = (ClampMin = "0.0", ClampMax = "1000.0", editcondition = "bEnableCameraRotationLag"))
	float RotationLagSpeed = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight")
	float WeightValue = 0.f;

public:
	void Update(float DeltaTime);
	void Initialize();
	void SetOwner(USceneComponent* InComponent, AActor* InActor);
	void AddRotation(FRotator InRotation, float InBlendTime = 1.f, UCurveFloat* InBlendCurve = nullptr);

	TWeakObjectPtr<AActor> GetOwnerActor() const { return OwnerActor; }
	const FVector GetOriginLocation();
	const FVector GetDirectionVector();
	const FRotator& GetRelativeRotation() { return RelativeRotation; }
	const FVector& GetLocation() { return ArmLocation; }
};