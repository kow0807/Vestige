#include "EStunState.h"
#include "../../../../../../Manager/SceneManager.h"
#include "../../EnemyBoss.h"
#include "../../Animation/EnemyBossAnimation.h"
#include "../EnemyBossStateMachine.h"

EStunState::EStunState(void)
{
}

EStunState::~EStunState(void)
{
}

void EStunState::Enter(EnemyBoss& boss)
{
	boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::STUN_IDLE, true);
}

void EStunState::Update(EnemyBoss& boss)
{
    // スタンタイマーが0になるまで何もしない（ボス自身がタイマーを更新）
    if (!boss.IsStunned()) 
    {
        // スタンが解除されたらCHASEに戻る
        boss.ChangeState(EnemyBoss::STATE::MOVE);
    }
}

void EStunState::Exit(EnemyBoss& boss)
{
    boss.SetStunned(0.0f);
}
