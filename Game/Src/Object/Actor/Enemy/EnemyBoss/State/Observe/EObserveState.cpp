#include "EObserveState.h"
#include "../../../../../../Manager/SceneManager.h"
#include "../../EnemyBoss.h"
#include "../../Animation/EnemyBossAnimation.h"
#include "../EnemyBossStateMachine.h"
#include "../../../../Player/Player.h"

EObserveState::EObserveState(void)
{
	time_ = 0.0f;
}

EObserveState::~EObserveState(void)
{
}

void EObserveState::Enter(EnemyBoss& boss)
{
	time_ = 0.0f;

	boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::OBSERVE, true, EnemyBoss::ANIM_BLEND_RATE);
}

void EObserveState::Update(EnemyBoss& boss)
{
	time_ += SceneManager::GetInstance().GetDeltaTime();

	// プレイヤーとの距離を確認(警戒中にプレイヤーが接近したら攻撃)
	float dist = VSize(VSub(boss.GetTransform().pos, boss.GetPlayer().GetTransform().pos));
	if (dist < DIST_LIMIT)
	{
		boss.ChangeState(EnemyBoss::STATE::ATTACK);
		return;
	}

	// 一定時間様子見したら追跡へ移行
	if (time_ >= OBSERVE_TIME)
	{
		boss.ChangeState(EnemyBoss::STATE::CHASE);
		return;
	}
}

void EObserveState::Exit(EnemyBoss& boss)
{
	time_ = 0.0f;
}
