#include "EStepState.h"
#include "../../../../../../Manager/SceneManager.h"
#include "../../EnemyBoss.h"
#include "../../Animation/EnemyBossAnimation.h"
#include "../EnemyBossStateMachine.h"
#include "../../../../Player/Player.h"

EStepState::EStepState(void)
{
    stepCount_ = 0;
    backStepTimer_ = 0.0f;
    height_ = 0.0f;
}

EStepState::~EStepState(void)
{
}

void EStepState::Enter(EnemyBoss& boss)
{
    stepCount_ = 0;
    backStepTimer_ = 0.0f;
    boss.LookAt(boss.GetPlayer().GetTransform().pos);
    boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::STEP, true, EnemyBoss::ANIM_BLEND_RATE);
}

void EStepState::Update(EnemyBoss& boss)
{
    boss.LookAt(boss.GetPlayer().GetTransform().pos);
    
    // 2回目は遠くに下がる
    float distance = (stepCount_ == 0) ? FIRST_DIS : SECOND_DIS;  
    VECTOR pos = boss.GetTransform().pos;
    VECTOR toPlayer = VSub(pos, boss.GetPlayer().GetTransform().pos);
    
    // プレイヤーの逆方向（後退方向）
    VECTOR dir = VNorm(VScale(toPlayer, 1.0f));

    if (backStepTimer_ < DURATION)
    {
        backStepTimer_ += SceneManager::GetInstance().GetDeltaTime();

        boss.GetTransformMutable().pos = CalculateBackstepPosition(pos, dir, backStepTimer_, DURATION, distance, height_);
    }
    else
    {
        backStepTimer_ = 0.0f;
        stepCount_++;
        if (stepCount_ < STEP_MAX_COUNT)
        {
            // 2回目のステップ開始。
            boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::STEP, true, EnemyBoss::ANIM_BLEND_RATE);
        }
        else
        {
            // 2回ステップ完了したらRecoverステートへ遷移
            boss.ChangeState(EnemyBoss::STATE::RECOVER);
        }
    }
}

void EStepState::Exit(EnemyBoss& boss)
{
}

VECTOR EStepState::CalculateBackstepPosition(const VECTOR& startPos, const VECTOR& dir, float elapsedTime, float duration, float distance, float height)
{
    float t = elapsedTime / duration;
    if (t > 1.0f) t = 1.0f;

    // より柔らかい加速と減速
    float easedT = 1.0f - (1.0f - t) * (1.0f - t);

    // 高さの補間を若干長く「浮かせる」ように調整
    float yOffset = height * sinf(t * PI); // サイン波でふわっと上下

    // ステップ方向に距離を補間
    VECTOR moveOffset = VScale(dir, distance * easedT);

    VECTOR result = 
    {
        startPos.x + moveOffset.x,
        startPos.y + yOffset,
        startPos.z + moveOffset.z
    };

    return result;
}
