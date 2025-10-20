#include "EnemyBossCombatController.h"
#include <memory>
#include <DxLib.h>
#include "../../../../../Manager/SceneManager.h"
#include "Attack/EAttackState.h"
#include "Attack/Pattern/ComboAttack.h"
#include "Attack/Pattern/ThrustAttack.h"
#include "Attack/Pattern/RoarAttack.h"
#include "Attack/Pattern/ExplosionAttack.h"
#include "Attack/Pattern/SetUpAttack.h"

EnemyBossCombatController::EnemyBossCombatController(EnemyBoss& boss)
    : 
    boss_(boss)
{
    // 各攻撃の初期クールダウンをゼロに設定
    for (const auto& pair : ATTACK_COOLDOWN_TIMES) 
    {
        attackCooldowns_[pair.first] = 0.0f;
    }
}

EnemyBoss::ATK_STATE EnemyBossCombatController::SelectAttackCandidate()
{
    // プレイヤーとの距離を取得
    float dist = boss_.GetPlayerDistanceXZ(); 

    // 連続攻撃が生成されていない場合、新たに生成する
    if (currentAttackSeries_.empty()) 
    {
        GenerateNewAttackSeries(dist);
        currentSeriesIndex_ = 0;
    }

    // 現在の連続攻撃から、次に実行すべき攻撃を選択
    if (currentSeriesIndex_ < currentAttackSeries_.size()) 
    {
        EnemyBoss::ATK_STATE candidate = currentAttackSeries_[currentSeriesIndex_];

        // 選択された攻撃がクールダウン中ではないか最終チェック
        if (!IsAttackOnCooldown(candidate)) 
        {
            return candidate;
        }
        else 
        {
            // クールダウン中のため連続攻撃を中断
            ResetContinuousAttackSeries(); 
            return EnemyBoss::ATK_STATE::NONE;
        }
    }

    // 連続攻撃が終了したか、選択できる攻撃がない場合
    ResetContinuousAttackSeries(); // シリーズをリセット
    return EnemyBoss::ATK_STATE::NONE;
}

// 攻撃が実行された後にクールダウンを設定
void EnemyBossCombatController::SetAttackCooldown(EnemyBoss::ATK_STATE attackType)
{
    if (ATTACK_COOLDOWN_TIMES.count(attackType)) 
    {
        attackCooldowns_[attackType] = ATTACK_COOLDOWN_TIMES.at(attackType);
    }
}

void EnemyBossCombatController::Update(float dt)
{
    // 全ての攻撃クールダウンを更新
    for (auto& pair : attackCooldowns_) 
    {
        if (pair.second > 0.0f) 
        {
            pair.second -= dt;
            if (pair.second < 0.0f) 
            {
                pair.second = 0.0f;
            }
        }
    }
}

bool EnemyBossCombatController::IsAttackOnCooldown(EnemyBoss::ATK_STATE attackType) const
{
    if (attackCooldowns_.count(attackType)) 
    {
        return attackCooldowns_.at(attackType) > 0.0f;
    }
    return false;
}

bool EnemyBossCombatController::AreAllAttacksOnCooldown() const
{
    for (const auto& pair : ATTACK_COOLDOWN_TIMES) 
    {
        if (attackCooldowns_.count(pair.first) == 0 || attackCooldowns_.at(pair.first) <= 0.0f) 
        {
            return false;
        }
    }
    return true;
}

void EnemyBossCombatController::ResetContinuousAttackSeries()
{
    currentAttackSeries_.clear();
    currentSeriesIndex_ = 0;
}

void EnemyBossCombatController::AdvanceContinuousAttackIndex()
{
    currentSeriesIndex_++;
}

int EnemyBossCombatController::GetCurrentContinuousAttackCount() const
{
    return currentSeriesIndex_ + 1;
}

int EnemyBossCombatController::GetTotalContinuousAttackCount() const
{
    return static_cast<int>(currentAttackSeries_.size());
}

void EnemyBossCombatController::GenerateNewAttackSeries(float dist)
{
    currentAttackSeries_.clear();
    std::vector<EnemyBoss::ATK_STATE> availableAttacks;

    // HPの割合を取得
	float hpRatio = boss_.GetHpRatio(); 

    // HP5割以下で一度だけ、Roar , Explosion のどちらかを発動させる
    if (hpRatio <= HP_RATIO_HALF)
    {
        if (!boss_.HasUsedRoarBelow50Percent() && !IsAttackOnCooldown(EnemyBoss::ATK_STATE::ROAR))
        {
            currentAttackSeries_.push_back(EnemyBoss::ATK_STATE::ROAR);
            boss_.SetHasUsedRoarBelow50Percent(true);
            return;
        }
        if (!boss_.HasUsedExplosionBelow50Percent() && !IsAttackOnCooldown(EnemyBoss::ATK_STATE::EXPLOSION))
        {
            currentAttackSeries_.push_back(EnemyBoss::ATK_STATE::EXPLOSION);
            boss_.SetHasUsedExplosionBelow50Percent(true);
            return;
        }
    }
    
    // Roar, Explosion が発動しなかった場合にチェックされる
    if (hpRatio <= HP_RATIO_0_4 && !boss_.HasUsedSetupBelow40Percent() && !IsAttackOnCooldown(EnemyBoss::ATK_STATE::SETUP))
    {
        currentAttackSeries_.push_back(EnemyBoss::ATK_STATE::SETUP);
        boss_.SetHasUsedSetupBelow40Percent(true);
        return;
    }

    for (const auto& pair : ATTACK_RANGES) 
    {
        EnemyBoss::ATK_STATE attackType = pair.first;
        float minDist = pair.second.first;
        float maxDist = pair.second.second;

        if ((attackType == EnemyBoss::ATK_STATE::ROAR && (hpRatio > HP_RATIO_HALF || boss_.HasUsedRoarBelow50Percent())) ||
            (attackType == EnemyBoss::ATK_STATE::EXPLOSION && (hpRatio > HP_RATIO_HALF || boss_.HasUsedExplosionBelow50Percent())))
        {
            continue;
        }

        if ((attackType == EnemyBoss::ATK_STATE::SETUP))
        {
            continue;
        }

        bool meetsDistance = (dist >= minDist && dist <= maxDist);

        if (attackType == EnemyBoss::ATK_STATE::COMBO || attackType == EnemyBoss::ATK_STATE::THRUST) 
        {
            // 距離条件と視野角条件の両方を満たす場合
            if (meetsDistance && boss_.IsPlayerInFront(COMBO_THRUST_VIEW_ANGLE_DEG, maxDist)) 
            {
                availableAttacks.push_back(attackType);
            }
        }
        // その他の通常攻撃 (視野角条件なし、距離条件のみ)
        else 
        {
            if (meetsDistance) 
            {
                availableAttacks.push_back(attackType);
            }
        }
    }

    if (availableAttacks.empty()) 
    {
        currentAttackSeries_.clear();
        return;
    }

    // 連続攻撃回数を決定 (1回から最大2回まで)
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());

    // 1から3回連続攻撃
    std::uniform_int_distribution<int> numAttacksDist(1, 2); 
    int numAttacksInSeries = numAttacksDist(engine);

    // 連続攻撃シリーズを生成
    for (int i = 0; i < numAttacksInSeries; ++i)
    {
        // 利用可能な攻撃の中からランダムに選択して連続攻撃に追加
        std::uniform_int_distribution<int> attackTypeDist(0, static_cast<int>(availableAttacks.size() - 1));
        currentAttackSeries_.push_back(availableAttacks[attackTypeDist(engine)]);
    }
}

EnemyBoss::ATK_STATE EnemyBossCombatController::GetNextAttackInSeries() const
{
    if (currentSeriesIndex_ < currentAttackSeries_.size()) 
    {
        return currentAttackSeries_[currentSeriesIndex_];
    }
    else
    {
        return EnemyBoss::ATK_STATE::NONE;
    }
}
