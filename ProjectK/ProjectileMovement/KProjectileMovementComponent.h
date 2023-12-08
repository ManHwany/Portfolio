// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SceneComponent.h"
#include "KProjectileMovementComponent.generated.h"

DECLARE_DELEGATE_OneParam(FKOnCompletedMovement, const FVector&);

class UCurveFloat;
class USkeletalMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), hidecategories = (Transform, Mobility, Socket, Rendering, Tags, ComponentReplication, Activation, Cooking, Physics, LOD, AssetUserData, Collision, "Components|Activation"))
class KGAME_API UKProjectileMovementComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	FKOnCompletedMovement OnCompletedMovement;

	FVector StartLocation = FVector::ZeroVector;
	FVector DesiredLocation = FVector::ZeroVector;

	bool bStartMove = false;

	float BlendTimeToGo = 0.f;
	float ElapsedTime = 0.f;

	// source 액터
	TWeakObjectPtr<AActor> OwnerActor;

	// 타겟 액터
	TWeakObjectPtr<AActor> TargetActor;

	// 타겟 컴포넌트
	TWeakObjectPtr<USceneComponent> TargetComponent;

	//타겟 액터와 컴포넌트의 타겟이 되는 메쉬 컴포넌트.
	TWeakObjectPtr<USkeletalMeshComponent> TargetMeshComp;

	// 타겟 액터의 소켓이름
	FName SocketName = NAME_None;

	bool bRootSocket = false;

	bool bRootOverrideSocket = false;

	// 종료 지점에 가중될 오프셋 위치
	FVector OffsetLocation = FVector::ZeroVector;

	// 캐스터가 타겟보다 왼쪽에 있는지 여부.
	bool bCasterOnLeftFromTarget = false;

	// 왼쪽 판별 Value. ( 내적 값 )
	float DotProductResult = 0.f;

private:
	/** 블렌딩 시간을 이용하여 날릴지에 대한 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Time Blend", AllowPrivateAccess = true))
	bool bUseBlendTime = false;

	/** 랜덤 블렌딩 시간 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Time Blend", AllowPrivateAccess = true, editcondition = "!bUseMovementSpeed && bUseBlendTime"))
	bool bUseRandomBlendTime = false;

	/** 랜덤 블렌딩 시간 범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Time Blend", AllowPrivateAccess = true, editcondition = "!bUseMovementSpeed && bUseBlendTime && bUseRandomBlendTime"))
	FVector2D BlendTimeMinMax = FVector2D::ZeroVector;

	/** 블렌딩 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Time Blend", AllowPrivateAccess = true, editcondition = "!bUseMovementSpeed && bUseBlendTime && !bUseRandomBlendTime"))
	float BlendTime = 0.f;

	/** 블렌딩 Function */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Time Blend", AllowPrivateAccess = true, editcondition = "!bUseCustomCurve &&bUseBlendTime && !bUseMovementSpeed"))
	TEnumAsByte<enum EViewTargetBlendFunction> SpeedBlendFunction;

	/** 블렌딩 가중치 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Time Blend", AllowPrivateAccess = true, editcondition = "!bUseCustomCurve && bUseBlendTime && !bUseMovementSpeed"))
	float SpeedBlendExp = 0.f;

	/************************************************************************/

	/** 무브 스피드를 이용하여 날릴지에 대한 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Speed", AllowPrivateAccess = true))
	bool bUseMovementSpeed = true;

	/** 랜덤 스피드 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Speed", AllowPrivateAccess = true, editcondition = "!bUseBlendTime && bUseMovementSpeed"))
	bool bUseRandomSpeed = false;

	/** 랜덤 스피드 Min Max 범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Speed", AllowPrivateAccess = true, editcondition = "!bUseBlendTime && bUseMovementSpeed && bUseRandomSpeed"))
	FVector2D MovementSpeedMinMax = FVector2D::ZeroVector;

	/** 무브 스피드 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Move by Speed", AllowPrivateAccess = true, editcondition = "!bUseBlendTime && bUseMovementSpeed && !bUseRandomSpeed"))
	float MovementSpeed = 1600.f;

	/************************************************************************/

	/** 포물선 움직임 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile ParabolaMove", AllowPrivateAccess = true))
	bool bUseParabolaMove = false;

	/** 포물선 움직임 커브 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile ParabolaMove", AllowPrivateAccess = true, editcondition = "bUseParabolaMove"))
	UCurveFloat* AmplitudeCurveFloat = nullptr;

	/** 포물선 랜덤 진폭 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile ParabolaMove", AllowPrivateAccess = true, editcondition = "bUseParabolaMove"))
	bool bUseRandomAmplitude = false;

	/** 랜덤 진폭 범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile ParabolaMove", AllowPrivateAccess = true, editcondition = "bUseParabolaMove && bUseRandomAmplitude"))
	FVector2D AmplitudeMinMax = FVector2D::ZeroVector;

	/** 고정 진폭 값 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile ParabolaMove", AllowPrivateAccess = true, editcondition = "bUseParabolaMove && !bUseRandomAmplitude"))
	float Amplitude = 1.f;

	/** 유도 포물선 움직임 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Guided ParabolaMove", AllowPrivateAccess = true))
	bool bUseGuidedParabola = false;

	/** 유도 포물선 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Guided ParabolaMove", AllowPrivateAccess = true, editcondition = "bUseGuidedParabola"))
	UCurveFloat* GuidedCurveFloat = nullptr;

	/** 유도 포물선 랜덤 진폭 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Guided ParabolaMove", AllowPrivateAccess = true, editcondition = "bUseGuidedParabola"))
	bool bUseRandomGuidedAmplitude = false;

	/** 유도 포물선 랜덤 진폭 범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Guided ParabolaMove", AllowPrivateAccess = true, editcondition = "bUseGuidedParabola && bUseRandomGuidedAmplitude"))
	FVector2D GuidedAmplitudeMinMax = FVector2D::ZeroVector;

	/** 유도 포물선 고정 진폭 값 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Guided ParabolaMove", AllowPrivateAccess = true, editcondition = "bUseGuidedParabola && !bUseRandomGuidedAmplitude"))
	float GuidedAmplitude = 1.f;

	/************************************************************************/

	/** 랜덤 발사 각도 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Fire Option", AllowPrivateAccess = true))
	bool bUseRandomFireAngle = false;

	/** 랜덤 발사 각도 범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Fire Option", AllowPrivateAccess = true, editcondition = "bUseRandomFireAngle"))
	FVector2D FireAngleMinMax = FVector2D::ZeroVector;

	/** 발사각 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Fire Option", AllowPrivateAccess = true, editcondition = "!bUseRandomFireAngle"))
	float FireAngle = 0.f;
		
	/************************************************************************/

	/** 커스텀 스피드 커브 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Speed Curve", AllowPrivateAccess = true))
	bool bUseSpeedCurve = false;

	/** 커스텀 스피드 커브 ( For move speed ) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "KProjectile Speed Curve", AllowPrivateAccess = true, editcondition = "bUseSpeedCurve"))
	UCurveFloat* SpeedCurveFloat = nullptr;

	float TotalCurveArea = 0.f;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "KProjectile Fire Option", AllowPrivateAccess = true))
	//float AreaDeltaTime = 0.01f;

protected:
	UKProjectileMovementComponent();
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif	

private:
	void init();
	void clear();
	float getBlendPct(const float blendTimeToGo, const float deltaTime);
	const FVector getSocketLocation(TWeakObjectPtr<AActor> actor, FName socketName);
	FVector getLandLocation(TWeakObjectPtr<USceneComponent> component);
	bool IsCasterOnLeftFromDesiredLocation();

	// 소켓에 관한 데이터 셋팅.
	void setTargetSocketInfo();

	// 커브 영역 계산
	float calcTotalCurveArea(UCurveFloat* curveFloat);

	// 커브 영역 계산
	float calcCurveArea(UCurveFloat* curveFloat, float minTime, float maxTime, float deltaTime);

	FVector calcDesireLocation();

	// 유도 커브 무브먼트 얻어오기
	const FVector getAddedGuidedCurveMovement(const float alpha);

	USkeletalMeshComponent* findTargetSkeletalMesh(AActor* targetActor);

	USkeletalMeshComponent* findTargetSkeletalMesh(USceneComponent* targetComp);

public:
	UFUNCTION(BlueprintCallable)
	void MoveToLocation(const FVector& location);

	UFUNCTION(BlueprintCallable)
	void MoveToActor(AActor* targetActor, FName socketName = "Root", const FVector locationOffset = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable)
	void MoveToComponent(USceneComponent* targetComponent, FName socketName = NAME_None, const FVector locationOffset = FVector::ZeroVector);

	void MovementTick(float deltaTime);

	void SetCompletedCallback(FKOnCompletedMovement onCompleted = nullptr);
};