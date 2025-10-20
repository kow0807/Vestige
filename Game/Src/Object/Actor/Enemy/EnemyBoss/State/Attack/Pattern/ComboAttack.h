#pragma once
#include "IEnemyAttackState.h"

class ComboAttack 
    : public IEnemyAttackState
{

public:

    // 攻撃判定時間
    static constexpr float ATTACK_DIRECTION_TIME = 0.73f;
    
    // ダメージ値
    static constexpr int DAMEGE_VALUE = 20;
    
    // 音量
    static constexpr int SOUND_VALUE = 255;

    ComboAttack(EnemyBoss& boss);
    ~ComboAttack(void);

    void Enter(EnemyBoss& boss)override;
    void Update(EnemyBoss& boss)override;
    void Exit(EnemyBoss& boss)override;
    bool IsFinished()const override;
    void StopEffect(void) override;

private:
    
    EnemyBoss& boss_;		// ボス

    // 音の初期化
    void InitSound(void)override;
};

