#include "EAttackState.h"
#include "../../EnemyBoss.h"
#include "../EnemyBossStateMachine.h"
#include "../EnemyBossCombatController.h"
#include "EnemyBossAttackStateMachine.h"
#include "../Attack/Pattern/IEnemyAttackState.h"
#include "../Attack/Pattern/ComboAttack.h"
#include "../Attack/Pattern/ThrustAttack.h"
#include "../Attack/Pattern/RoarAttack.h"
#include "../Attack/Pattern/ExplosionAttack.h"
#include "../Attack/Pattern/SetUpAttack.h"

EAttackState::EAttackState(void)
{
	
}

EAttackState::~EAttackState(void)
{
}

void EAttackState::Enter(EnemyBoss& boss)
{
    if (!isContinuous_)
    {
        boss.GetAttackStateController()->GenerateNewAttackSeries(boss.GetPlayerDistanceXZ());
        isContinuous_ = true;
    }

    // 次の攻撃を EnemyBoss が記憶しているので
    EnemyBoss::ATK_STATE attackToExecute = boss.GetAttackStateController()->GetNextAttackInSeries();

    if (attackToExecute != EnemyBoss::ATK_STATE::NONE)
    {
        boss.GetAttackStateMachine()->Change(boss, attackToExecute);
    }
    else 
    {
        boss.ChangeState(EnemyBoss::STATE::CHASE);
    }
}

void EAttackState::Update(EnemyBoss& boss)
{
    if (boss.IsStunned())
    {
        return;
    }

    // サブステートマシンの更新
    if (boss.GetAttackStateMachine()) 
    {
        boss.GetAttackStateMachine()->Update(boss);
    }
    else
    {
        boss.ChangeState(EnemyBoss::STATE::CHASE);
        return;
    }

    // 現在の攻撃が終了したら、次の行動を決定
    if (boss.GetAttackStateMachine()->IsFinished())
    {
        // 攻撃が終了したのでクールダウンを設定
        boss.GetAttackStateController()->SetAttackCooldown(boss.GetAttackStateMachine()->GetCurrentStateType());

        // 連続攻撃のインデックスを進める
        boss.GetAttackStateController()->AdvanceContinuousAttackIndex();

        // 連続攻撃の総回数を確認
        if (boss.GetAttackStateController()->GetCurrentContinuousAttackCount() > boss.GetAttackStateController()->GetTotalContinuousAttackCount())
        {
            // 全ての連続攻撃が完了した場合
            // 連続攻撃終了
            isContinuous_ = false; 

            // 距離を保つステートへ
            boss.ChangeState(EnemyBoss::STATE::MOVE);
            return;
        }

        // 次の攻撃を CombatController に問い合わせる
        EnemyBoss::ATK_STATE nextAttackInSeries = boss.GetAttackStateController()->SelectAttackCandidate();

        if (nextAttackInSeries != EnemyBoss::ATK_STATE::NONE) 
        {
            // 次の攻撃があれば実行
            boss.GetAttackStateMachine()->Change(boss, nextAttackInSeries);
            boss.GetAttackStateController()->SetAttackCooldown(nextAttackInSeries);
        }
        else {
            // 連続攻撃シリーズの途中で、次の攻撃がクールダウン中などで選べなかった場合
            // 連続攻撃終了
            isContinuous_ = false; 
            // Chaseに戻る
            boss.ChangeState(EnemyBoss::STATE::CHASE);
        }
    }
}

void EAttackState::Exit(EnemyBoss& boss)
{
    // 攻撃終了時にサブステートマシンをリセット
    if (boss.GetAttackStateMachine()) 
    {
        boss.GetAttackStateMachine()->Reset(boss);
    }

    // 連続攻撃シリーズの状態をリセット
    if (boss.GetAttackStateController()) 
    {
        boss.GetAttackStateController()->ResetContinuousAttackSeries();
    }

    // 次の攻撃をリセット
    boss.SetNextAttackType(EnemyBoss::ATK_STATE::NONE);
    isContinuous_ = false;
 }