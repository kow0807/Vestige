#include "ComboAttack.h"
#include <DxLib.h>
#include "../../../../../../../Manager/SceneManager.h"
#include "../../../../../../../Manager/ResourceManager.h"
#include "../../../EnemyBoss.h"
#include "../../../Animation/EnemyBossAnimation.h"
#include "../../EnemyBossCombatSystem.h"
#include "../../../../../Player/Player.h"

ComboAttack::ComboAttack(EnemyBoss& boss) 
    :
    boss_(boss)
{
    isFinished_ = false;
    timer_ = 0.0f;
    noHitSound_ = -1;
    isSoundPlayed_ = false;
}

ComboAttack::~ComboAttack(void)
{
}

void ComboAttack::Enter(EnemyBoss& boss)
{
    InitSound();
    timer_ = 0.0f;
    isFinished_ = false;
    isSoundPlayed_ = false;
    boss_.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::COMBO, false);
}

void ComboAttack::Update(EnemyBoss& boss)
{
    timer_ += SceneManager::GetInstance().GetDeltaTime();

    if (!boss_.GetAnimation()->IsEnd())
    {
        if (!isSoundPlayed_)
        {
            PlaySoundMem(noHitSound_, DX_PLAYTYPE_BACK, true);
            isSoundPlayed_ = true;
        }
        if (timer_ < ATTACK_DIRECTION_TIME)
        {
            if (boss.GetAttackStateSystem()->IsHit())
            {
                boss.GetPlayer().Damage(DAMEGE_VALUE);
            }
        }
    }
    else if (boss_.GetAnimation()->IsEnd())
    {
        boss.GetAttackStateSystem()->ClearHitRecord();
        isSoundPlayed_ = false;
        isFinished_ = true;
    }
}

void ComboAttack::Exit(EnemyBoss& boss)
{
}

bool ComboAttack::IsFinished() const
{
    return isFinished_;
}

void ComboAttack::StopEffect(void)
{
}

void ComboAttack::InitSound(void)
{
    noHitSound_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::ATTACK_SE).handleId_;

    ChangeVolumeSoundMem(SOUND_VALUE, noHitSound_);
}
