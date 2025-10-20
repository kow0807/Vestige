#include "EChaseState.h"
#include "../../../../../../Manager/SceneManager.h"
#include "../../EnemyBoss.h"
#include "../../Animation/EnemyBossAnimation.h"
#include "../EnemyBossCombatController.h"
#include "../EnemyBossStateMachine.h"
#include "../../../../Player/Player.h"

EChaseState::EChaseState(void)
{
	dist_ = 0.0f;
}

EChaseState::~EChaseState(void)
{
}

void EChaseState::Enter(EnemyBoss& boss)
{
	boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::RUN, true, EnemyBoss::ANIM_BLEND_RATE);
}

void EChaseState::Update(EnemyBoss& boss)
{
	// プレイヤーへの方向（水平面のみ）
	dist_ = boss.GetPlayerDistanceXZ();
	if (dist_ < ATTACK_RANGE)
	{
		if (boss.GetStepCooldown() <= 0.0f)
		{
			boss.SetStepCooldown(EnemyBoss::STEP_COOLDOWN_DURATON);
			boss.ChangeState(EnemyBoss::STATE::STEP);
			return;
		}
	}

	// プレイヤーが攻撃開始範囲内に入ったら、攻撃可能かチェック
	if (dist_ < ATTACK_INITIATE_RANGE)
	{
		// 次の攻撃候補を CombatController から取得
		EnemyBoss::ATK_STATE nextAttack = boss.GetAttackStateController()->SelectAttackCandidate();
		if (nextAttack != EnemyBoss::ATK_STATE::NONE)
		{
			// EAttackStateで使うため,次の攻撃を EnemyBoss が記憶する
			boss.SetNextAttackType(nextAttack);
			boss.ChangeState(EnemyBoss::STATE::ATTACK);
			return;
		}
	}

	// 正規化方向ベクトル
	VECTOR dir = VNorm(boss.GetToPlayer());

	// 向きをクォータニオンで補間
	Quaternion targetRot = Quaternion::LookRotation(dir);
	boss.GetTransformMutable().quaRot = Quaternion::Slerp(boss.GetTransform().quaRot, targetRot, 0.1f);

	// 前進移動
	boss.GetTransformMutable().pos = VAdd(boss.GetTransform().pos, VScale(dir, CHASE_SPEED));
}

void EChaseState::Exit(EnemyBoss& boss)
{
}
