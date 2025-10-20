#include "ERecoverState.h"
#include "../../../../../../Manager/SceneManager.h"
#include "../../EnemyBoss.h"
#include "../../Animation/EnemyBossAnimation.h"
#include "../EnemyBossStateMachine.h"

ERecoverState::ERecoverState(void)
{
	time_ = 0.0f;
}

ERecoverState::~ERecoverState(void)
{
}

void ERecoverState::Enter(EnemyBoss& boss)
{
	time_ = 0.0f;
	boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::IDLE, true, EnemyBoss::ANIM_BLEND_RATE);
}

void ERecoverState::Update(EnemyBoss& boss)
{
	time_ += SceneManager::GetInstance().GetDeltaTime();

	if (time_ >= DURATION)
	{
		boss.ChangeState(EnemyBoss::STATE::IDLE);
	}
}

void ERecoverState::Exit(EnemyBoss& boss)
{
	time_ = 0.0f;
}
