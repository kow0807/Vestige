#include "SetUpAttack.h"
#include <DxLib.h>
#include "../../../../../../../Manager/SceneManager.h"
#include "../../../../../../../Manager/ResourceManager.h"
#include "../../../EnemyBoss.h"
#include "../../../Animation/EnemyBossAnimation.h"


SetUpAttack::SetUpAttack(EnemyBoss& boss) 
    :
    boss_(boss)
{
}

SetUpAttack::~SetUpAttack(void)
{
}

void SetUpAttack::Enter(EnemyBoss& boss)
{
    isFinished_ = false;
    timer_ = 0.0f;
    buffApplied_ = false;
    chargeFinished_ = false;
    boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::SETUP, false);
}

void SetUpAttack::Update(EnemyBoss& boss)
{
    timer_ += SceneManager::GetInstance().GetDeltaTime();

    // チャージフェーズ
    if (timer_ < CHARGE_DURATION)
    {
        // バフ適用タイミングのチェック
        if (!buffApplied_ && timer_ >= BUFF_APPLY_TIME)
        {
            // ボス自身のバフを適用
            boss.SetAttackBuff(BUFF_DURATION, ATTACK_MULTIPLIER);
            buffApplied_ = true; // バフは一度だけ適用
        }
    }
    else 
    {
        // チャージ完了後、残りの攻撃アニメーションなど
        if (!chargeFinished_) 
        {
            chargeFinished_ = true;
        }
    }

    if (timer_ >= CHARGE_DURATION + ATTACK_DURATION) // 全てのフェーズが終了したら攻撃完了
    {
        if (!IsFinished())
        {
            isFinished_ = true;
        }
    }
}

void SetUpAttack::Exit(EnemyBoss& boss)
{
}

bool SetUpAttack::IsFinished() const
{
    return isFinished_;
}

void SetUpAttack::StopEffect(void)
{
}

void SetUpAttack::InitSound(void)
{
}
