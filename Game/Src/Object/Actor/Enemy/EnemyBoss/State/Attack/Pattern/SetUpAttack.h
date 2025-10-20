#pragma once
#include "IEnemyAttackState.h"
class SetUpAttack 
    : public IEnemyAttackState
{

public:


    static constexpr float CHARGE_DURATION = 1.5f;          // チャージ時間
    static constexpr float BUFF_APPLY_TIME = 1.0f;          // バフが適用される時間
    static constexpr float ATTACK_DURATION = 0.5f;          // 攻撃の時間
    static constexpr float BUFF_DURATION = 12.0f;           // バフが適用されている時間
    static constexpr float ATTACK_MULTIPLIER = 1.5f;        // バフの倍率

    SetUpAttack(EnemyBoss& boss);
    ~SetUpAttack(void);

    void Enter(EnemyBoss& boss)override;
    void Update(EnemyBoss& boss)override;
    void Exit(EnemyBoss& boss)override;
    bool IsFinished()const override;
    void StopEffect(void) override;

private:
    EnemyBoss& boss_;		        // ボス
    bool buffApplied_ = false;      //バフが適応されたかどうかのフラグ
    bool chargeFinished_ = false;   // チャージフェーズが狩猟したかどうかのフラグ

    // 音の初期化
    void InitSound(void)override;   
};

