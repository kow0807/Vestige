#pragma once
#include<map>
#include<functional>
#include "../EnemyBoss.h"
#include "../State/Attack/Pattern/IEnemyAttackState.h"

class AttackPatternStateMachine
{

public:

    explicit AttackPatternStateMachine(EnemyBoss& boss);

    void Update(EnemyBoss& boss);
    void SetUpdateFunction(EnemyBoss::ATK_STATE aStateId, std::shared_ptr<IEnemyAttackState> stateInstance);
    void ChangeState(EnemyBoss::ATK_STATE newState);

    EnemyBoss::ATK_STATE GetCurrentAStateId() const;

    // デバック用表示
    void DrawDebugInfo(const VECTOR& screenPos);

private:
    EnemyBoss& boss_;       // ボス
    std::map<EnemyBoss::ATK_STATE, std::shared_ptr<IEnemyAttackState>> states_;     // 攻撃ステートと攻撃ステートクラスを格納する
    std::shared_ptr<IEnemyAttackState> curAState_;                                  // 現在の攻撃ステート
    EnemyBoss::ATK_STATE curAStateId_;                                              // 現在の攻撃ステートID
};

