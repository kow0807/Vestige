#pragma once
#include <DxLib.h>
#include "IEnemyAttackState.h"
class RoarAttack 
    : public IEnemyAttackState
{

public:


    static constexpr float STUN_TRIGGER_TIME = 0.5f;            // スタンさせ始める時間
    
    
    static constexpr float ATTACK_MAX_RANGE = 450.0f;           // 攻撃範囲

    
    static constexpr float SOUND_START_TIME = 0.5f;             // 音が再生され始める時間
    
    
    static constexpr int SOUND_VALUE = 255;                     // 音量
    
    static constexpr float TIME_LIMIT = 3.0f;                   // 咆哮が終わる時間
    static constexpr float EFFECT_SIZE = 25.0f;                 // エフェクトの大きさ
    static constexpr float F_EFFECT_POS_Y = 100.0f;             // ひとつめのエフェクトのy座標
    static constexpr float S_EFFECT_SIZE_Z = 15.0f;             // ふたつめのエフェクトのz方向のサイズ

    RoarAttack(EnemyBoss& boss);
    ~RoarAttack(void);

    void Enter(EnemyBoss& boss)override;
    void Update(EnemyBoss& boss)override;
    void Exit(EnemyBoss& boss)override;
    bool IsFinished()const override;
    void StopEffect(void) override;

private:
    EnemyBoss& boss_;		// ボス
    float waveTimer_;       // 当たり判定用時間
    float effectRadius_;    //エフェクト用の半径
    bool waveAtMax_;        // 半径が最大になったらtrueを返す
    bool hasStunnedPlayer_; //プレイヤーがスタンしているかどうか


    // エフェクト
    int effectRoarFPlayId_;     // 一つ目のエフェクトの再生ハンドル
    int effectRoarFResId_;      // 一つ目のエフェクトハンドル

    int effectRoarSPlayId_;     // 二つ目のエフェクトの再生ハンドル
    int effectRoarSResId_;      // 二つ目のエフェクトハンドル
    bool IsEffect_;             // 再生するかどうか


    // エフェクトの初期化
    void InitEffect(void);

    // エフェクトの更新
    void UpdateEffect(EnemyBoss& boss);

    // 音の初期化
    void InitSound(void)override;
};

