#include "AttackPatternStateMachine.h"
#include "../EnemyBoss.h"

AttackPatternStateMachine::AttackPatternStateMachine(EnemyBoss& boss)
    : 
    boss_(boss), 
    curAStateId_(EnemyBoss::ATK_STATE::NONE), 
    curAState_(nullptr)
{
}

void AttackPatternStateMachine::Update(EnemyBoss& boss)
{
    // 現在の攻撃パターンの更新を通す
    if (curAState_)
    {
        curAState_->Update(boss);
    }
}

void AttackPatternStateMachine::SetUpdateFunction(EnemyBoss::ATK_STATE aStateId, std::shared_ptr<IEnemyAttackState> stateInstance)
{
    states_[aStateId] = stateInstance;
}

void AttackPatternStateMachine::ChangeState(EnemyBoss::ATK_STATE newState)
{
    if (states_.count(newState) == 0)
    {
        return;
    }

    // 現在の攻撃パターンの初期化を通す
    if (curAState_)
    {
        curAState_->Exit(boss_);
    }

    curAState_ = states_[newState];
    curAStateId_ = newState;

    // 現在の攻撃パターンの終了を通す
    if (curAState_)
    {
        curAState_->Enter(boss_);
    }
}

EnemyBoss::ATK_STATE AttackPatternStateMachine::GetCurrentAStateId() const
{
    return curAStateId_;
}

void AttackPatternStateMachine::DrawDebugInfo(const VECTOR& screenPos)
{
    const char* stateName = "";
    switch (curAStateId_)
    {
    case EnemyBoss::ATK_STATE::NONE:          stateName = "NONE"; break;
    case EnemyBoss::ATK_STATE::COMBO:         stateName = "COMBO"; break;
    case EnemyBoss::ATK_STATE::THRUST:        stateName = "THRUST"; break;
    case EnemyBoss::ATK_STATE::ROAR:          stateName = "ROAR"; break;
    case EnemyBoss::ATK_STATE::EXPLOSION:     stateName = "EXPLOSION"; break;
    case EnemyBoss::ATK_STATE::SWEEP:         stateName = "SWEEP"; break;
    case EnemyBoss::ATK_STATE::FOLLOW_SLASH:  stateName = "FOLLOW_SLASH"; break;
    case EnemyBoss::ATK_STATE::SETUP:         stateName = "SETUP"; break;
    default: 
        stateName = "UNKNOWN";
        break;
    }

    DrawFormatString((int)screenPos.x, (int)screenPos.y, GetColor(255, 255, 255), "AState: %s", stateName);
}