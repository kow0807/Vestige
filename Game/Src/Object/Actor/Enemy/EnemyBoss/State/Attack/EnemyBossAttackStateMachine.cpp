#include "EnemyBossAttackStateMachine.h"
#include "../Attack/Pattern/IEnemyAttackState.h"
#include "../Attack/Pattern/ExplosionAttack.h"
#include "../Attack/Pattern/RoarAttack.h"

// static map の実体を定義(デバック表示のため)
const std::map<EnemyBoss::ATK_STATE, const char*> EnemyBossAttackStateMachine::attackStateNames_ = {
	{ EnemyBoss::ATK_STATE::NONE,			"NONE"        },
	{ EnemyBoss::ATK_STATE::COMBO,			"COMBO"       },
	{ EnemyBoss::ATK_STATE::THRUST,			"THRUST"      },
	{ EnemyBoss::ATK_STATE::ROAR,			"ROAR"		  },
	{ EnemyBoss::ATK_STATE::EXPLOSION,      "EXPLOSION"   },
	{ EnemyBoss::ATK_STATE::SWEEP,			"SWEEP"       },
	{ EnemyBoss::ATK_STATE::FOLLOW_SLASH,   "FOLLOW_SLASH"},
	{ EnemyBoss::ATK_STATE::SETUP,			"SETUP"       },
};

EnemyBossAttackStateMachine::EnemyBossAttackStateMachine(void): curStateId_(EnemyBoss::ATK_STATE::NONE)
{
}

EnemyBossAttackStateMachine::~EnemyBossAttackStateMachine(void)
{
}

void EnemyBossAttackStateMachine::Add(EnemyBoss::ATK_STATE type, std::shared_ptr<IEnemyAttackState> state)
{
	states_[type] = state;
}

void EnemyBossAttackStateMachine::Change(EnemyBoss& boss, EnemyBoss::ATK_STATE type)
{
	// 遷移先の状態が存在するかチェック
	if (states_.count(type) == 0) 
	{
		return;
	}

	// 同じ状態への遷移は無視
	if (curStateId_ == type) 
	{
		return;
	}

	if (curState_) 
	{
		curState_->Exit(boss);
	}

	curState_ = states_[type];
	curStateId_ = type; // 現在の状態IDを更新

	if (curState_) 
	{
		curState_->Enter(boss);
	}
}

void EnemyBossAttackStateMachine::Update(EnemyBoss& boss)
{
	if (curState_)
	{
		curState_->Update(boss);
	}
}

EnemyBoss::ATK_STATE EnemyBossAttackStateMachine::GetCurrentStateType() const
{
	return curStateId_;
}

void EnemyBossAttackStateMachine::Reset(EnemyBoss& boss)
{
	if (curState_)
	{
		curState_->Exit(boss);
	}

	curState_ = nullptr;
	if (!boss.IsDead())
	{
		curStateId_ = EnemyBoss::ATK_STATE::NONE;
	}
}

bool EnemyBossAttackStateMachine::IsFinished() const
{
	if (curState_)
	{
		return curState_->IsFinished();
	}
	else
	{
		return true;
	}

}

void EnemyBossAttackStateMachine::StopEffect(void)
{
	switch (curStateId_)
	{
	case EnemyBoss::ATK_STATE::NONE:
	{
		auto roarState = std::static_pointer_cast<RoarAttack>(states_[EnemyBoss::ATK_STATE::ROAR]);
		if (roarState)
		{
			roarState->StopEffect();
		}
		auto exploState = std::static_pointer_cast<ExplosionAttack>(states_[EnemyBoss::ATK_STATE::EXPLOSION]);
		if (exploState)
		{
			exploState->StopEffect();
		}
	}
		break;
	default:
		break;
	}
}

void EnemyBossAttackStateMachine::DrawDebugInfo(const VECTOR& screenPos)
{
	const char* stateName = "";

	auto it = attackStateNames_.find(curStateId_);
	if (it != attackStateNames_.end()) 
	{
		stateName = it->second;
	}

	DrawFormatString((int)screenPos.x, (int)screenPos.y, GetColor(255, 255, 255), "AState: %s", stateName);

}