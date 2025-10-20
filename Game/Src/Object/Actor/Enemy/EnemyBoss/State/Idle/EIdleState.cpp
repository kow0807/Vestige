#include "EIdleState.h"
#include <DxLib.h>
#include "../../../../../../Manager/SceneManager.h"
#include"../../EnemyBoss.h"
#include "../../Animation/EnemyBossAnimation.h"
#include "../EnemyBossStateMachine.h"

EIdleState::EIdleState(void)
{
}

EIdleState::~EIdleState(void)
{
}

void EIdleState::Enter(EnemyBoss& boss)
{
	boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::IDLE, true, 0.5f);
}

void EIdleState::Update(EnemyBoss& boss)
{
	boss.GetStateMachine()->ChangeState(EnemyBoss::STATE::CHASE);
}

void EIdleState::Exit(EnemyBoss& boss)
{
}
