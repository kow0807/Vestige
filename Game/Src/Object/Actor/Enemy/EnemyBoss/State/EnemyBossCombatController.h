#pragma once
#include <memory>
#include <map>
#include <vector>
#include <random>
#include <algorithm>
#include "AttackPatternStateMachine.h"
#include "Attack/Pattern/IEnemyAttackState.h"
class EnemyBoss;

// 攻撃状態遷移管理
class EnemyBossCombatController
{

public:

    const float COMBO_THRUST_VIEW_ANGLE_DEG = 30.0f;    // 正面から左右30度（合計60度）
    const float COMBO_THRUST_MAX_DISTANCE = 600.0f;     // この距離内でのみ視野判定を行う
    static constexpr float HP_RATIO_HALF = 0.5f;        // HPの半分の割合い
    static constexpr float HP_RATIO_0_4 = 0.4f;         // HPの４分の一の割合

    EnemyBossCombatController(EnemyBoss& boss);

    // プレイヤーとの距離とクールダウンを考慮して、実行可能な攻撃の中から最適なものを返す
    EnemyBoss::ATK_STATE SelectAttackCandidate();

    // 攻撃が実行された後にクールダウンを設定
    void SetAttackCooldown(EnemyBoss::ATK_STATE attackType);

    // 毎フレームクールダウンを更新
    void Update(float dt);

    // 特定の攻撃がクールダウン中か
    bool IsAttackOnCooldown(EnemyBoss::ATK_STATE attackType) const;
    
    // 全ての攻撃がクールダウン中か
    bool AreAllAttacksOnCooldown() const;

    // 連続攻撃を開始する際に、次の攻撃シリーズを生成する
    void GenerateNewAttackSeries(float dist);

    // シリーズ内の次の攻撃を取得
    EnemyBoss::ATK_STATE GetNextAttackInSeries() const;

    // 現在の連続攻撃シリーズをリセット
    void ResetContinuousAttackSeries();

    // 連続攻撃シリーズのインデックスを進める
    void AdvanceContinuousAttackIndex();
    
    // 現在の連続攻撃が何回目か (1-indexed)
    int GetCurrentContinuousAttackCount() const;
    
    // 現在の連続攻撃シリーズの総回数
    int GetTotalContinuousAttackCount() const;

private:
    EnemyBoss& boss_;                                           //　ボス

   
    std::map<EnemyBoss::ATK_STATE, float> attackCooldowns_;     // 各攻撃ごとのクールダウン

    // 各攻撃のクールダウン時間 (設定値)
    const std::map<EnemyBoss::ATK_STATE, float> ATTACK_COOLDOWN_TIMES = {
        {EnemyBoss::ATK_STATE::COMBO,  3.5f},
        {EnemyBoss::ATK_STATE::THRUST, 4.5f},
        {EnemyBoss::ATK_STATE::ROAR, 15.0f},
        {EnemyBoss::ATK_STATE::EXPLOSION, 20.0f},
        {EnemyBoss::ATK_STATE::SETUP, 60.0f},
    };

    // 各攻撃の推奨距離範囲 (min_dist, max_dist)
    const std::map<EnemyBoss::ATK_STATE, std::pair<float, float>> ATTACK_RANGES = {
        {EnemyBoss::ATK_STATE::COMBO, {0.0f, 180.0f}},
        {EnemyBoss::ATK_STATE::THRUST, {100.0f, 300.0f}},
        {EnemyBoss::ATK_STATE::ROAR, {0.0f, 180.0f}},
        {EnemyBoss::ATK_STATE::EXPLOSION, {0.0f, 800.0f}},
        {EnemyBoss::ATK_STATE::SETUP, {0.0f, 1000.0f}},
    };

    // 連続攻撃のために内部処理
    std::vector<EnemyBoss::ATK_STATE> currentAttackSeries_; // 現在の連続攻撃シリーズ
    int currentSeriesIndex_ = 0;                            // 現在の攻撃シリーズの番号
};

