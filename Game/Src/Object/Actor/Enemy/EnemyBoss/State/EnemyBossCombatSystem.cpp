#include <EffekseerForDXLib.h>
#include "EnemyBossCombatSystem.h"
#include "../EnemyBoss.h"
#include "../../../Player/Player.h"
#include "../../../../../Manager/ResourceManager.h"
#include "../../../../../Utility/AsoUtility.h"

EnemyBossCombatSystem::EnemyBossCombatSystem(EnemyBoss& boss) 
    :
    boss_(boss), 
    comboRestTime_(COMBO_RESET_TIME),
    attackCooldown_(ATTACK_COOLDOWN),
    currentCooldown_(0.0f),
    isAttacking_(false),
    attackDuration_(ATTACK_DURATION),
    attackTimer_(0.0f),
    comboCount_(0),
    maxCombo_(MAX_COMBO)
{
    InitEffect();
}

EnemyBossCombatSystem::~EnemyBossCombatSystem()
{
}

void EnemyBossCombatSystem::Update(float deltaTime)
{
    if (currentCooldown_ > 0.0f)
    {
        currentCooldown_ -= deltaTime;
    }

    if (!isAttacking_ && comboCount_ > 0)
    {
        comboResetTimer_ += deltaTime;
        if (comboResetTimer_ >= comboRestTime_)
        {
            comboCount_ = 0;
            comboResetTimer_ = 0.0f;
        }
    }

    if (isAttacking_)
    {
        attackTimer_ += deltaTime;
        if (attackTimer_ >= attackDuration_)
        {
            isAttacking_ = false;
            attackTimer_ = 0.0f;
            ClearHitRecord();
        }
    }
}

void EnemyBossCombatSystem::ExcuteAttack(EnemyBoss::ATK_STATE pattern)
{
    if (!CanAttack() || comboCount_ >= maxCombo_)
    {
        return; //攻撃できない状態また攻撃カウントが上限に達したときは通さない
    }

    isAttacking_ = true;
    attackTimer_ = 0.0f;
    currentCooldown_ = attackCooldown_;

    comboCount_++;
    comboResetTimer_ = 0.0f;    // 攻撃したのでタイマーリセット
}

bool EnemyBossCombatSystem::CanAttack() const
{
    return currentCooldown_ <= 0.0f;
}

void EnemyBossCombatSystem::InitEffect(void)
{
    // エフェクト
    effectHitResId_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::HIT_EFFECT).handleId_;

    // 大きさ
    SetScalePlayingEffekseer3DEffect(effectHitPlayId_, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);
}

bool EnemyBossCombatSystem::IsHit()
{
    MV1RefreshCollInfo(boss_.GetPlayer().GetTransform().modelId);

    // カプセルとの衝突判定
    auto hits = MV1CollCheck_Capsule(
        boss_.GetPlayer().GetTransform().modelId, -1,
        boss_.GetTopPos(), boss_.GetDownPos(), EnemyBoss::CAPSULE_RADIUS);

    bool isHit = false;
    bool isUseEffect = false;

    if (hits.HitNum > 0)
    {
        int playerId = boss_.GetPlayer().GetID();
        if (hitPlayers_.find(playerId) == hitPlayers_.end())
        {
            for (int i = hits.HitNum - 1; i >= 0; i--)
            {
                auto ePos = hits.Dim[i].HitPosition;
                VECTOR wCenter = VScale(VAdd(boss_.GetTopPos(), boss_.GetDownPos()), 0.5f);
                if (AsoUtility::IsHitSpheres(wCenter, SPHERE_RADIUS, ePos, 1.0f))
                {
                    if (isUseEffect == false)
                    {
                        if (boss_.GetPlayer().GetRollingCnt() <= 0)
                        {
                            // エフェクト
                            effectHitPlayId_ = PlayEffekseer3DEffect(effectHitResId_);
                        }
                        // 大きさ
                        SetScalePlayingEffekseer3DEffect(effectHitPlayId_, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);
                        SetPosPlayingEffekseer3DEffect(effectHitPlayId_, hits.Dim->HitPosition.x, hits.Dim->HitPosition.y, hits.Dim->HitPosition.z);

                        isUseEffect = true;
                    }
                }
            }
            if (isUseEffect == false)
            {
                if (boss_.GetPlayer().GetRollingCnt() <= 0) 
                {
                    // エフェクト
                    effectHitPlayId_ = PlayEffekseer3DEffect(effectHitResId_);
                }
                // 大きさ
                SetScalePlayingEffekseer3DEffect(effectHitPlayId_, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);
                SetPosPlayingEffekseer3DEffect(effectHitPlayId_, boss_.GetTopPos().x, boss_.GetTopPos().y, boss_.GetTopPos().z);
                isUseEffect = true;
            }
            hitPlayers_.insert(playerId);
            isHit = true;
        }
    }

    MV1CollResultPolyDimTerminate(hits);
    return isHit;
}

void EnemyBossCombatSystem::ClearHitRecord()
{
    hitPlayers_.clear();
}
