#pragma once
#include <DxLib.h>
#include "IEnemyAttackState.h"
class ThrustAttack :
    public IEnemyAttackState
{

public:

 
    static constexpr float DURATION = 1.0f;                 // 突進継続時間
    static constexpr float DISTANCE = 250.0f;               // 突進する距離
    static constexpr float INITIAL_SPEED = 5.0f;            // 初速度
    static constexpr float PEAK_SPEED = 20.0f;              // 最高速度
    static constexpr float DECELERATION_RATE = 0.5f;        // 減速開始までの時間割合
    static constexpr float DIST_LIMIT = 500.0f;             // 攻撃対象座標と自身の座標との距離の制限
    static constexpr float TARGETPOS_LIMIT = 200.0f;        // ターゲット座標の制限
    static constexpr float ATTACK_TIME_LIMIT = 0.95f;       // 攻撃時間の制限
    static constexpr int DAMAGE_VALUE = 20;                 // ダメージ値
    static constexpr int SOUND_VALUE = 255;                 // 音量

    ThrustAttack(EnemyBoss& boss);
    ~ThrustAttack(void);

    void Enter(EnemyBoss& boss)override;
    void Update(EnemyBoss& boss)override;
    void Exit(EnemyBoss& boss)override;
    bool IsFinished()const override;
    void StopEffect(void) override;

private:
    EnemyBoss& boss_;		    // ボス
    VECTOR initialPos_;         // 突進開始位置
    VECTOR chargeDirection_;    // 突進方向

    // 速度カーブの計算用
    float GetCurrentSpeed(float t);

    // サウンドの初期化
    void InitSound(void)override;
};

