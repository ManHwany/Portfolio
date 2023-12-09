using GameDB;
using Sirenix.OdinInspector;
using System;
using System.Collections.Generic;
using Spine.Unity;
using UnityEditor;
using UnityEngine;

[Serializable]
public class SkillSpec
{
    [ShowInInspector]
    public float LifeTime
    {
        get { return (float)SkillLifeTime * 0.025f; }
    }

    [HideLabel]
    [PreviewField(60, ObjectFieldAlignment.Left)]
    [HorizontalGroup("Split", 60)]
    public Sprite Icon;

    [VerticalGroup("Split/Right")]
    public string Description;

    [VerticalGroup("Split/Right")]
    public bool IsNormalAttack;

    [VerticalGroup("Split/Right")]
    public bool UseMainTarget;

    [VerticalGroup("Split/Right")]
    [InfoBox("평타시 모션이 끝나기전에 다음평타 발동할 만분율(100%)")]
    [ShowIf("IsNormalAttack", true)]
    [Range(0, 10000)]
    public int RevocableTime = 8108;

    [SerializeField]
    [InfoBox("캐스팅 틱 (*AnimDic에 있는 Cast_01 애님으로 작동. 비어있으면 작동 안함*)")]
    [HideIf("IsNormalAttack", true)]
    public int CastingTick;

    [SerializeField]
    [InfoBox("일반 공격이 아닌 스킬일때 사용할 애니메이션 이름 및 스킬 수명")]
    [HideIf("IsNormalAttack", true)]
    public string AnimationName;

    public bool IsLoopingAnim;

    [Space(10)]
    [SerializeField]
    [InfoBox("애니메이션 시작과 동시에 나올 사운드")]
    public ESoundKeyType AnimationSFX;

    [InfoBox("부착될 스파인이벤트 효과들 ID")]
    public FXSpec SKILL_FIRE1;

    public FXSpec SKILL_FIRE2;
    public FXSpec SKILL_TRAIL;

    [SerializeField]
    [HideIf("IsNormalAttack", true)]
    [Range(1, 2000)]
    public int SkillLifeTime;

    [VerticalGroup("Split/Right")]
    [Range(1, 1000)]
    public int SkillRange;

    [Space(15)]
    [HorizontalGroup("Coverage")]
    [SerializeField]
    public CoverageSpec[] Coverages;

    [Space(15)]
    //[VerticalGroup("Coverage")]
    [SerializeField]
    public MoveCoverageSpec[] MoveCoverages;

    [Space(15)]
    //[VerticalGroup("Coverage")]
    [InfoBox("스킬 발동 시 카메라를 줌 인/아웃 하는 효과, 1개만 실행")]
    [SerializeField]
    public BattleCameraAction[] BattleCameraZoomActions;

    [Space(15)]
    //[VerticalGroup("Coverage")]
    [InfoBox("피격 시 카메라를 흔드는 효과, 1개 만 실행")]
    [SerializeField]
    public BattleCameraAction[] BattleCameraShakeActions;

    [Space(15)]
    //[VerticalGroup("Coverage")]
    [InfoBox("스킬 발동 시 카메라를 흔드는 효과, 1개 만 실행")]
    [SerializeField]
    public BattleCameraAction[] BattleCameraShakeActions_Start;
}

[Serializable]
public class CoverageSpec
{
    public enum EProjectileSpeedType
    {
        Linear,
        EaseIn,
        EaseOut,
    }

    [ShowInInspector]
    public float Second
    {
        get { return (float)DelayTick * 0.025f; }
    }

    [Title("Covarage Desc", "", TitleAlignments.Centered)]
    public string CovarageDesc = "커버리지 그룹";

    [FoldoutGroup("$CovarageDesc")]
    [Title("보스 전투시 확률적 발동 여부 (100% 기준)", "", TitleAlignments.Left)]
    public bool ActiveChanceForBoss = false;

    [Space(5)]
    [FoldoutGroup("$CovarageDesc")]
    [ShowIf("ActiveChanceForBoss", true)]
    public int ChancePer = 100;

    [FoldoutGroup("$CovarageDesc")]
    [Title("Target AttributeType", "", TitleAlignments.Left)]
    public GameDB.ETargetAttributeType TargetAttribute;

    [FoldoutGroup("$CovarageDesc")]
    [Title("Fire Knockback Type", "", TitleAlignments.Left)]
    public EFireKnockbackType FireKnockbackType;

    [FoldoutGroup("$CovarageDesc")]
    [HideIf("FireKnockbackType", EFireKnockbackType.None)]
    public int KnockbackDist;

    [FoldoutGroup("$CovarageDesc")]
    [Title("Target AttributeType", "", TitleAlignments.Left)]
    public EProjectileType ProjectileType;

    [FoldoutGroup("$CovarageDesc")]
    [ShowIf("ProjectileType", EProjectileType.Twist)]
    [InfoBox("곡사 옵션")]
    [Range(-20, 20)]
    public int PointA = 0;

    [FoldoutGroup("$CovarageDesc")]
    [ShowIf("ProjectileType", EProjectileType.Twist)]
    [Range(-20, 20)]
    public int PointB = 0;

    [Space(5)]
    [FoldoutGroup("$CovarageDesc")]
    [ShowIf("ProjectileType", EProjectileType.Twist)]
    [Title("곡사 각도 ( -180도 ~ 180도 )")]
    [Range(-180, 180)]
    public int AngleA = 0;

    [FoldoutGroup("$CovarageDesc")]
    [ShowIf("ProjectileType", EProjectileType.Twist)]
    [Range(-180, 180)]
    public int AngleB = 0;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    [InfoBox("커버리지 Target Count")]
    [Range(1, 20)]
    public int TargetCount;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    [InfoBox("스킬시전 후 해당 Tick 이후에 발사가 시작된다")]
    [Range(1, 2000)]
    public int DelayTick;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    [InfoBox("스킬 적중 후 해당 Tick 딜레이 이후 어빌리티 적용")]
    [Range(0, 2000)]
    public int DelayHitTick = 0;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    [InfoBox("위치X,Y (cm)")]
    [Range(-500, 500)]
    public int PosX = 0;

    [FoldoutGroup("$CovarageDesc")]
    [Range(-500, 500)]
    public int PosY = 0;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    [InfoBox("Look(우측)방향기준 회전각도")]
    [HideIf("ProjectileType", EProjectileType.Twist)]
    [Range(0, 360)]
    public int Degree = 0;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    [InfoBox("발사체속도(0인경우 타겟에 즉발)")]
    [Range(0, 50)]
    public int Speed;

    [FoldoutGroup("$CovarageDesc")]
    public bool CasterRenderingBack;

    [Space(10)]
    [Title("프로젝타일 프리펩", "", TitleAlignments.Left)]
    [FoldoutGroup("$CovarageDesc")]
    [InfoBox("휘두르기 이펙트")]
    public List<WieldEffect> WieldPrefabs;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    public GameObject BurstPrefab;

    [FoldoutGroup("$CovarageDesc")]
    public bool FollowCasterBurstPrefab = false;

    [FoldoutGroup("$CovarageDesc")]
    public bool BurstRenderingBack;

    [FoldoutGroup("$CovarageDesc")]
    public GameObject ProjectilePrefab;

    [FoldoutGroup("$CovarageDesc")]
    public GameObject HitPrefab;

    [FoldoutGroup("$CovarageDesc")]
    public bool DependOnHitPointDirection = true;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    public ESoundKeyType BurstSFX;

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    public ESoundKeyType HitSFX;

    public enum ECoverageResultType
    {
        Ability,
        Range,
        Muzzle
    }

    [Space(10)]
    [FoldoutGroup("$CovarageDesc")]
    [InfoBox("1.어빌리티그룹  /  2. Range Prefab")]
    public ECoverageResultType CoverageResult;

    [FoldoutGroup("$CovarageDesc")]
    [ShowIf("CoverageResult", ECoverageResultType.Ability)]
    public int AbilityGroupId;

    [FoldoutGroup("$CovarageDesc")]
    [ShowIf("CoverageResult", ECoverageResultType.Range)]
    public GameObject RangePrefab;

    [FoldoutGroup("$CovarageDesc")]
    [ShowIf("CoverageResult", ECoverageResultType.Muzzle)]
    public GameObject MuzzlePrefab;
}

[Serializable]
public class RangeSpec
{
    [System.Serializable]
    public struct RangeStateAniInfo
    {
        public string AnimName;
        public GameObject EffectPrefab;
        public ESoundKeyType SFX;
    }

    [ShowInInspector]
    public float DelaySecond
    {
        get { return (float)Delay * 0.025f; }
    }

    [ShowInInspector]
    public float LifeTimeSecond
    {
        get { return (float)LifeTime * 0.025f; }
    }

    [ShowInInspector]
    public float IndicateLifeTimeSecond
    {
        get { return (float)IndicateLifeTime * 0.025f; }
    }

    [Title("Range Desc", "", TitleAlignments.Centered)]
    public string RangeDesc = "Range 그룹";

    [FoldoutGroup("$RangeDesc")]
    [Title("Target AttributeType", "", TitleAlignments.Left)]
    public GameDB.ETargetAttributeType TargetAttribute;    

    [Space(10)]
    [InfoBox("Range 프리펩")]
    [FoldoutGroup("$RangeDesc")]    
    public GameObject RangePrefab;

    [FoldoutGroup("$RangeDesc")]    
    public bool RangeRenderingBack;

    [Space(10)]
    [InfoBox("스폰 연출용 스파인 사용 여부")]
    [FoldoutGroup("$RangeDesc")]
    public bool UseSpine;

    [FoldoutGroup("$RangeDesc")]
    [ShowIf("UseSpine", true)]
    public SkeletonAnimation SpineAnim;

    [FoldoutGroup("$RangeDesc")]
    [ShowIf("UseSpine", true)]
    public RangeStateAniInfo SpawnAniInfo;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("인터벌 Range 프리팹")]
    public GameObject RangeIntervalPrefab;

    [FoldoutGroup("$RangeDesc")]
    public bool RangeIntervalRenderingBack;

    [Space(10)]
    [InfoBox("Range Hit 프리펩 정보")]
    [FoldoutGroup("$RangeDesc")]
    public GameObject RangeHitPrefab;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    public Vector3 HitOffset;

    [FoldoutGroup("$RangeDesc")]
    public bool UseRandomRotation;

    /**
     * -jaehwan Effect 출력위치 요소들. 비쥬얼적 사용이라 Determistic과는 무관
     */

    [FoldoutGroup("$RangeDesc")]
    [ShowIf("UseRandomRotation", true)]
    [MinMaxSlider(-10, 10)]
    public Vector2 RandomXRotRange;

    [FoldoutGroup("$RangeDesc")]
    [MinMaxSlider(-10, 10)]
    [ShowIf("UseRandomRotation", true)]
    public Vector2 RandomYRotRange;

    [FoldoutGroup("$RangeDesc")]
    [MinMaxSlider(-10, 10)]
    [ShowIf("UseRandomRotation", true)]
    public Vector2 RandomZRotRange;

    [FoldoutGroup("$RangeDesc")]
    [HideIf("UseRandomRotation", true)]
    public Quaternion HitRotation;

    /**
     * ----------------------------------------------------------
     */

    [FoldoutGroup("$RangeDesc")]
    public ESoundKeyType RangeHitSFX;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("Range 타입")]
    public ERangeType RangeType;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("캐스터가 죽거나 가드브레이크시 죽일지 말지 여부")]
    public bool LifeDependOnCaster = false;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [ShowIf("RangeType", ERangeType.RectangleArea)]
    public int Width;

    [FoldoutGroup("$RangeDesc")]
    [ShowIf("RangeType", ERangeType.RectangleArea)]
    public int Height;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [ShowIf("RangeType", ERangeType.RectangleArea)]
    public ERangeFixedPosDirection RangeDirection;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("랜덤 위치 여부 ( X, Y 범위 )")]
    public bool UseRandomPos = false;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [ShowIf("UseRandomPos", true)]
    [Range(-1000, 1000)]
    public int RandomXRangeMin;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [ShowIf("UseRandomPos", true)]
    [Range(-1000, 1000)]
    public int RandomXRangeMax;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [ShowIf("UseRandomPos", true)]
    [Range(-1000, 1000)]
    public int RandomYRangeMin;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [ShowIf("UseRandomPos", true)]
    [Range(-1000, 1000)]
    public int RandomYRangeMax;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [HideIf("IsFollowCaster", true)]
    [InfoBox("위치 X, Y (cm)")]
    [HideIf("UseRandomPos", true)]
    [Range(-500, 500)]
    public int PosX = 0;

    [FoldoutGroup("$RangeDesc")]
    [HideIf("IsFollowCaster", true)]
    [HideIf("UseRandomPos", true)]
    [Range(-500, 500)]
    public int PosY = 0;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("타겟 카운트 (-1 이면 범위 모든 적 피해 )")]
    [Range(-1, 20)]
    public int TargetCount;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("해당 Tick 이후에 Range가 시작된다")]
    [Range(1, 2000)]
    public int Delay;

    [FoldoutGroup("$RangeDesc")]
    [Range(1, 2000)]
    public int Interval = 0;

    [FoldoutGroup("$RangeDesc")]
    [Range(1, 2000)]
    public int LifeTime = 0;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("Indicate 프리펩")]
    public GameObject IndicatePrefab;

    [FoldoutGroup("$RangeDesc")]
    [Range(1, 2000)]
    public int IndicateLifeTime = 0;

    [FoldoutGroup("$RangeDesc")]
    public bool IsFollowCaster = false;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("범위 (cm)")]
    [HideIf("RangeType", ERangeType.RectangleArea)]
    [Range(0, 5000)]
    public int ExplosionRange = 150;

    [FoldoutGroup("$RangeDesc")]
    [ShowIf("RangeType", ERangeType.RingArea)]
    public int ExclusiveRange = 50;

    [Space(10)]
    [FoldoutGroup("$RangeDesc")]
    [InfoBox("적용 될 어빌리티 GroupId")]
    public int AbilityGroupId;
}

[Serializable]
public class FXSpec
{
    public string VFXID = "";
    public ESoundKeyType SFXID;
}

[Serializable]
public class MoveCoverageSpec
{
    [Title("Move Covarage Desc", "", TitleAlignments.Centered)]
    public string MoveCoverageDesc = "무브 커버리지 그룹";

    [Space(10)]
    [PropertyOrder(1)]
    [FoldoutGroup("$MoveCoverageDesc")]
    public GameObject MovePrefab;

    [PropertyOrder(1)]
    [FoldoutGroup("$MoveCoverageDesc")]
    public bool BurstRenderingBack = false;

    [Space(10)]
    [PropertyOrder(1)]
    [FoldoutGroup("$MoveCoverageDesc")]
    public GameObject MoveHitPrefab;

    [Space(10)]
    [PropertyOrder(1)]
    [FoldoutGroup("$MoveCoverageDesc")]
    public bool BaseOnTime = false;

    [Space(10)]
    [PropertyOrder(1)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [ShowIf("BaseOnTime", true)]
    public int RangeRadius = 300;

    [Space(10)]
    [PropertyOrder(1)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [Range(0, 1000)]
    public int StartTick;

    [ShowInInspector]
    [FoldoutGroup("$MoveCoverageDesc")]
    [PropertyOrder(2)]
    public float StartSecond
    {
        get { return (float)StartTick * 0.025f; }
    }

    [PropertyOrder(3)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [Range(1, 1000)]
    public int EndTick;

    [ShowInInspector]
    [FoldoutGroup("$MoveCoverageDesc")]
    [PropertyOrder(4)]
    public float EndSecond
    {
        get { return (float)EndTick * 0.025f; }
    }

    [Space(10)]
    [PropertyOrder(5)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [HideIf("BaseOnTime", true)]
    [Title("무브 스피드", "", TitleAlignments.Left)]
    [Range(0, 10000)]
    public int MoveSpeed;

    [Space(10)]
    [PropertyOrder(6)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [HideIf("BaseOnTime", true)]
    public int DesiredDist = 0;

    [Space(5)]
    [Title("타겟 타입", "", TitleAlignments.Left)]
    [PropertyOrder(7)]
    [FoldoutGroup("$MoveCoverageDesc")]
    public GameDB.ETargetAttributeType TargetAttribute;

    [Space(10)]
    [PropertyOrder(7)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [InfoBox("타겟 카운트 (-1 이면 범위 모든 타겟 적용 )")]
    [Range(-1, 20)]
    public int TargetCount;

    [Space(5)]
    [Title("어빌리티 GroupId", "", TitleAlignments.Left)]
    [PropertyOrder(7)]
    [FoldoutGroup("$MoveCoverageDesc")]
    public int AbilityGroupId;

    [Space(10)]
    [PropertyOrder(5)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [Title("히트 원 크기", "", TitleAlignments.Left)]
    public int Radius;

    public enum EMoveHitCalcType
    {
        NoneHit,
        Tick,
        Instant
    }

    [Title("히트 계산 방법 설정", "", TitleAlignments.Left)]
    [PropertyOrder(8)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [EnumToggleButtons, GUIColor(0f, 1f, 0)]
    public EMoveHitCalcType HitCalcType;

    [Title("히트 틱 설정", "", TitleAlignments.Left)]
    [Space(5)]
    [PropertyOrder(8)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [HideIf("HitCalcType", EMoveHitCalcType.NoneHit)]
    [Range(1, 10)]
    public int MaxHitCount;

    [Space(5)]
    [PropertyOrder(8)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [HideIf("HitCalcType", EMoveHitCalcType.NoneHit)]
    [Range(1, 20)]
    public int HitInterval;

    [Space(5)]
    [PropertyOrder(8)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [HideIf("HitCalcType", EMoveHitCalcType.NoneHit)]
    [Range(0, 500)]
    public int HitStartTick;

    [PropertyOrder(8)]
    [FoldoutGroup("$MoveCoverageDesc")]
    [ShowIf("HitCalcType", EMoveHitCalcType.Tick)]
    [Range(1, 500)]
    public int HitEndTick;

    [PropertyOrder(9)]
    [Space(10)]
    [FoldoutGroup("$MoveCoverageDesc")]
    public ESoundKeyType MoveBurstSFX;

    [PropertyOrder(9)]
    [Space(10)]
    [FoldoutGroup("$MoveCoverageDesc")]
    public ESoundKeyType MoveHitSFX;

#if UNITY_EDITOR

    private int HitTickArray(int value, GUIContent label)
    {
        return (int)EditorGUILayout.Slider(value, 0, 1000);
    }

#endif
}

[Serializable]
public class BattleCameraAction
{
    public BattleCameraAction(ScriptableData_DOTween data)
    {
        Data = data;
    }

    [Title("Camera Action Desc", "", TitleAlignments.Centered)]
    public string ActionDesc = "카메라 액션 설명";

    [Space(10)]
    [PropertyOrder(1)]
    [FoldoutGroup("ActionDesc")]
    public ScriptableData_DOTween Data;

    [Space(10)]
    [PropertyOrder(1)]
    [FoldoutGroup("ActionDesc")]
    [Range(1, 1000)]
    public int StartTick;

    [Space(10)]
    [PropertyOrder(2)]
    [FoldoutGroup("ActionDesc")]
    public bool AutoRecovery = false;

    [ShowInInspector]
    [FoldoutGroup("ActionDesc")]
    [PropertyOrder(3)]
    public float StartSecond
    {
        get { return (float)StartTick * 0.025f; }
    }
}