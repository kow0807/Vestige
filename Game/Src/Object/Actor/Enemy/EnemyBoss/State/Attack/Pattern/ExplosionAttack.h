#pragma once
#include <DxLib.h>
#include "IEnemyAttackState.h"

class ExplosionAttack 
    : public IEnemyAttackState
{

public:

    // スタンする時間
    static constexpr float SELF_STUN_DURATION = 3.0f;
    
    // 攻撃対象に攻撃座標を固定するまでの時間
    static constexpr float TARGET_LOCK_TIME = 2.0f;

    // 回転の制限
    static constexpr float DIRECTION_VEC_LIMIT = 0.001f;
    
    // 音が再生される時間
    static constexpr float SOUND_START_TIME = 5.0f;
    
    // 攻撃判定時間
    static constexpr float ATTACK_DIRECTION_TIME = 6.0f;
    
    // 攻撃判定の半径
    static constexpr float ATTACK_RADIUS = 80.0f;
    
    // ダメージ値
    static constexpr int DAMEGE_VALUE = 100;
    
    // 攻撃が終了する時間
    static constexpr float EXPLOSION_END_TIME = 8.0f;

    // エフェクトのXZ軸方向の大きさ
    static constexpr float EFFECT_SIZE_XZ = 200.0f;
    
    // エフェクトのY軸方向の大きさ
    static constexpr float EFFECT_SIZE_Y = 150.0f;
    
    // エフェクトのY座標の調整
    static constexpr float EFFECT_POS_Y_ADJUSTMENT = 100.0f;

    // 音量
    static constexpr int SOUND_VALUE = 255;


    ExplosionAttack(EnemyBoss& boss);
    ~ExplosionAttack(void);

    void Enter(EnemyBoss& boss)override;
    void Update(EnemyBoss& boss)override;
    void Exit(EnemyBoss& boss)override;
    bool IsFinished()const override;
    void StopEffect(void) override;

private:
    EnemyBoss& boss_;		        // ボス
    float selfStunDuration_;        // 自身をスタンさせる時間
    VECTOR targetPosition_;         // 対象座標
    bool isAttack_;                 // 攻撃したかどうか
    bool targetPosionFixed_;        // 対象座標を固定するかどうか
    bool reachedTargetPos_;         // 攻撃座標が決まったかどうか

    bool hasSelfStunned_;           // 自身がスタン状態であるかどうか

    // エフェクト
    int effectExploPlayId_;         //　再生用ハンドル
    int effectBuffResId_;           // エフェクトハンドル
    bool IsEffect_;                 // 再生するかどうか

    // エフェクトの初期化
    void InitEffect(void);

    // エフェクトの更新
    void UpdateEffect(void);

    // 音の初期化
    void InitSound(void)override;
};

