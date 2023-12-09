using diverseshared;
using GameDB;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class SkillSelector
{
	// 예약 스킬 ID
    public uint ReservedSkillTid { get; set; }

    private PlayerEntity _ownerEntity;
    private FieldBase _field;
    public SkillPresetInfo SkillPresetInfo { get; set; }

    private List<uint> _skillList = new List<uint>() { 0, 0, 0, 0, 0, 0 };

    private uint _heroTid = 0;
    private uint _curSkillPresetTid = 0;

    public BaseEntity MainTarget { get; set; }

    public bool IsAutoSkill
    {
        get
        {
            return _isAutoSkill;
        }
        set
        {
            _isAutoSkill = value;
            if (_isAutoSkill)
            {
                RefreshSkillQueue();
            }
            else
            {
                ReservedSkillTid = 0;
            }
        }
    }

    private bool _isAutoSkill = true;

    public void Init(PlayerEntity ownerEntity)
    {
        _ownerEntity = ownerEntity;
        _heroTid = _ownerEntity.Tid;
        _field = _ownerEntity.GetField();

        var playerModel = SgModel.Instance.GetPlayer(_ownerEntity.PlayerNo);
        if (playerModel == null)
        {
            return;
        }

        _curSkillPresetTid = playerModel.EquippedSkillPresetTid;
        SkillPresetInfo = playerModel.GetSkillPresetInfo(_curSkillPresetTid, _heroTid);

        RefreshSkillQueue();
    }

    public void Tick(F64 delta)
    {
        if (IsAutoSkill || (_ownerEntity.PlayerNo != 0 && _ownerEntity.PlayerNo != GamePlayData.i.UserAccount.PlayerNo))
        {
            ReservedSkillTid = GetTopSkill();

            //예약된 스킬이 없으면 0 (평타의미)
            if (ReservedSkillTid == 0)
            {
                ProcessNormalAttack();
            }
            else
            {
                ProcessSkill(ReservedSkillTid);
            }
        }
        else
        {
			// 기본적으로 일반공격을 계속 수행하도록
            ProcessNormalAttack();
        }
    }

	// 스킬 프리셋 Refresh
    public void RefreshSkillPreset()
    {
        _curSkillPresetTid = SgModel.Instance.GetPlayer(_ownerEntity.PlayerNo).EquippedSkillPresetTid;
        SkillPresetInfo = SgModel.Instance.GetPlayer(_ownerEntity.PlayerNo).GetSkillPresetInfo(_curSkillPresetTid, _heroTid);

        RefreshSkillQueue();
    }

    private List<uint> _normalSkillTids = new List<uint>();
    private int _currentNormalSkillIndex = 0;
    private int _nextNormalAttackTick = 0;

    public void ChangeBasicSkillTid(uint weaponID)
    {
        if (GameDB.GameDBContainer.Instance.WeaponList.TryGetValue(weaponID, out GameDB.Weapon sheet))
        {
            _normalSkillTids.Clear();

            if (sheet.BasicSkillTid > 0)
            {
                _normalSkillTids.Add(sheet.BasicSkillTid);
            }

            if (sheet.BasicSkillTid2 > 0)
            {
                _normalSkillTids.Add(sheet.BasicSkillTid2);
            }

            if (sheet.BasicSkillTid3 > 0)
            {
                _normalSkillTids.Add(sheet.BasicSkillTid3);
            }
        }
    }

	// 일반 공격 수행
    public void ProcessNormalAttack()
    {    
        if (_normalSkillTids.Count == 0)
        {
            return;
        }

        if (_field.CurrentTick < _nextNormalAttackTick)
        {
            return;
        }

        var sightRect = GameDB.GameDBContainer.Instance.config.BasicEnemyAISight / 100;

        // 메인타겟 설정
        SetMainTarget();

        _currentNormalSkillIndex = Mathf.Min(_normalSkillTids.Count - 1,
       (_currentNormalSkillIndex + 1) % _normalSkillTids.Count);

        var normalAttackTid = _normalSkillTids[_currentNormalSkillIndex];

        _ownerEntity.LaunchSkill(normalAttackTid, MainTarget);
    }

    public void OnNormalAttackEnter(int endNormalTick)
    {
        _nextNormalAttackTick = endNormalTick;
    }

    public void ProcessSkill(uint skillTid)
    {
        if (GameDBContainer.Instance.SkillSheetList.TryGetValue(skillTid, out SkillSheet skillSheet) == false)
        {
            DebugManager.LogError("NoSkill Tid:" + skillTid);
            return;
        }
            

        BaseEntity resTarget = null;

        if (skillSheet.TargetType == ETargetAttributeType.Self || skillSheet.TargetType == ETargetAttributeType.None)
        {
            resTarget = _ownerEntity;
        }
        else if (skillSheet.TargetType == ETargetAttributeType.Ally)
        {
            var searchPos = _ownerEntity.Pos;
            List<ISectorMember> candidates = new List<ISectorMember>();
            _field.GetCandidatesOnSector(SectorType.Entity, searchPos, F64.Ratio10(100), candidates);
            // todo: 여러 모드에 따라 sort기준필요(가까운, 먼, 랜덤)
            // 일단은 거리순으로 정렬 및 카운트만큼 take
            var sortList = candidates
                .Where(candidate =>
                {
                    var target = (BaseEntity)candidate;
                    // 이미 죽었으면 넘어간다
                    if (target.IsDead)
                    {
                        return false;
                    }
                    // 나인지 체크
                    if (target == _ownerEntity)
                    {
                        return false;
                    }
                    // faction체크
                    if (target.Faction == _ownerEntity.Faction)
                    {
                        return true;
                    }

                    return false;
                })
                .OrderBy(target => F64Vec2.Distance(target.GetPosition(), searchPos)).Take(1).ToList();

            if (sortList.Count > 0)
            {
                resTarget = sortList[0] as BaseEntity;
            }
        }
        else
        {
            SetMainTarget();
            resTarget = MainTarget;
        }

        if (resTarget == null)
        {
            return;
        }

        _ownerEntity.LaunchSkill(skillTid, resTarget);
    }

	// 스킬 Queue Refresh
    public void RefreshSkillQueue()
    {
        if (SkillPresetInfo == null)
        {
            return;
        }

        var heroInfo = _ownerEntity.Stats.GetHeroInfo();

        if (heroInfo == null)
        {
            return;
        }
    
        _skillList[0] = heroInfo.AutoSkillFlag == 0 || BitMask.Check(heroInfo.AutoSkillFlag, 1) ?
                        SkillPresetInfo.SkillTid1 :
                        0;        
       
        _skillList[1] = heroInfo.AutoSkillFlag == 0 || BitMask.Check(heroInfo.AutoSkillFlag, 2) ?
                        SkillPresetInfo.SkillTid2 :
                        0;
       
        _skillList[2] = heroInfo.AutoSkillFlag == 0 || BitMask.Check(heroInfo.AutoSkillFlag, 3) ?
                        SkillPresetInfo.SkillTid3 :
                        0;        
        
        _skillList[3] = heroInfo.AutoSkillFlag == 0 || BitMask.Check(heroInfo.AutoSkillFlag, 4) ?
                        SkillPresetInfo.SkillTid4 :
                        0;
      
        _skillList[4] = heroInfo.AutoSkillFlag == 0 || BitMask.Check(heroInfo.AutoSkillFlag, 5) ?
                        SkillPresetInfo.SkillTid5 :
                        0;
       
        _skillList[5] = heroInfo.AutoSkillFlag == 0 || BitMask.Check(heroInfo.AutoSkillFlag, 6) ?
                        SkillPresetInfo.SkillTid6 :
                        0;        

        ReservedSkillTid = GetTopSkill();
    }

    public uint GetTopSkill()
    {
        uint resSkillTid = 0;

        foreach (var skillTid in _skillList)
        {
            if (skillTid == 0)
                continue;

            var endCool = _field.GetSkillEndCool(_ownerEntity.PlayerNo, skillTid);
            if (endCool <= _field.CurrentTick)
            {
                resSkillTid = skillTid;
                break;
            }
        }

        return resSkillTid;
    }

    public void SetMainTarget()
    {
        if (MainTarget != null && !MainTarget.IsDead)
        {
            return;
        }

        var sightRect = GameDB.GameDBContainer.Instance.config.BasicEnemyAISight / 100;
        // 가까운 타겟을 찾는다
        MainTarget = (BaseEntity)_field.FindClosestOnSector(SectorType.Entity, _ownerEntity, sightRect);

        _currentNormalSkillIndex = 0;
    }
}