#include "EMoveState.h"
#include "../../../../../../Manager/SceneManager.h"
#include "../../EnemyBoss.h"
#include "../../Animation/EnemyBossAnimation.h"
#include "../EnemyBossStateMachine.h"
#include "../EnemyBossCombatController.h"
#include "../../../../Player/Player.h"

EMoveState::EMoveState(void)
{
}

EMoveState::~EMoveState(void)
{
}

void EMoveState::Enter(EnemyBoss& boss)
{
    boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::WALK_F, true, EnemyBoss::ANIM_BLEND_RATE);
}

void EMoveState::Update(EnemyBoss& boss)
{
    // ボスがスタン中の場合は何もしない
    if (boss.IsStunned()) 
    {
        return;
    }

    float dist = boss.GetPlayerDistanceXZ();

    // 全ての攻撃のクールダウンが回復したか、または攻撃できるものが一つでもないかチェック
    if (!boss.GetAttackStateController()->AreAllAttacksOnCooldown()) 
    {
        boss.ChangeState(EnemyBoss::STATE::CHASE);
        return;
    }

    // プレイヤーとの距離を一定に保つための移動ロジック
    if (dist < TARGET_DISTANCE - DISTANCE_TOLERANCE) 
    {
        // 近すぎたらプレイヤーから離れる
        boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::WALK_B, true);

        // プレイヤーから離れる方向
        VECTOR dirAway = VNorm(VSub(boss.GetTransform().pos, boss.GetPlayer().GetTransform().pos));
        boss.SetPosition(VAdd(boss.GetTransform().pos, VScale(dirAway, MOVE_SPEED)));
    }
    else if (dist > TARGET_DISTANCE + DISTANCE_TOLERANCE) 
    {
        // 遠すぎたらプレイヤーに近づく
        boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::WALK_F, true);
        
        // プレイヤーに向かう方向
        VECTOR dirToward = VNorm(VSub(boss.GetPlayer().GetTransform().pos, boss.GetTransform().pos));
        boss.SetPosition(VAdd(boss.GetTransform().pos, VScale(dirToward, MOVE_SPEED)));
    }
    else 
    {
        // ちょうどよい距離なら停止
        boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::IDLE, true);
    }
}

void EMoveState::Exit(EnemyBoss& boss)
{
}
