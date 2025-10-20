#include "ThrustAttack.h"
#include <DxLib.h>
#include "../../../../../../../Utility/AsoUtility.h"
#include "../../../../../../../Manager/SceneManager.h"
#include "../../../../../../../Manager/ResourceManager.h"
#include "../../../EnemyBoss.h"
#include "../../../Animation/EnemyBossAnimation.h"
#include "../../EnemyBossCombatSystem.h"
#include "../../../../../Player/Player.h"


ThrustAttack::ThrustAttack(EnemyBoss& boss) 
    :
    boss_(boss),
    initialPos_({ 0.0f,0.0f,0.0f }),
    chargeDirection_({ 0.0f,0.0f ,0.0f })
{
    isFinished_ = false;
    timer_ = 0.0f;
    initialPos_ = AsoUtility::VECTOR_ZERO;
    chargeDirection_ = AsoUtility::VECTOR_ZERO;
}

ThrustAttack::~ThrustAttack(void)
{
}

void ThrustAttack::Enter(EnemyBoss& boss)
{
    InitSound();
    isFinished_ = false;
    timer_ = 0.0f;
    isSoundPlayed_ = false;

    // 突進開始位置を保存
    initialPos_ = boss.GetPlayer().GetTransform().pos;

    // プレイヤー方向を突進方向とする
    chargeDirection_ = VNorm(boss.GetToPlayer());

    // 仮のアニメーション再生
    boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::THRUST, false); 
}

void ThrustAttack::Update(EnemyBoss& boss)
{
    float dt = SceneManager::GetInstance().GetDeltaTime();
    timer_ += dt;

    float t = timer_ / DURATION; // 0.0から1.0に正規化された時間

    // 時間が経過したら攻撃終了
    if (t > 1.0f) 
    {
        t = 1.0f; // クランプ
    }

    float currentSpeed = GetCurrentSpeed(t);

    VECTOR diff = VSub(initialPos_, boss.GetTransform().pos);
    float dist = std::sqrt(diff.x * diff.x + diff.z * diff.z);

    if (dist < DIST_LIMIT)
    {
        float targetPos = VSize(VSub(boss.GetPlayer().GetTransform().pos, initialPos_));
        if (targetPos > TARGETPOS_LIMIT)
        {
            VECTOR pos = boss.GetTransformMutable().pos;
            pos.x += chargeDirection_.x * currentSpeed;
            pos.z += chargeDirection_.z * currentSpeed;
        }
    }

    if (!boss.GetAnimation()->IsEnd())
    {
        if (!isSoundPlayed_)
        {
            PlaySoundMem(noHitSound_, DX_PLAYTYPE_BACK, true);
            isSoundPlayed_ = true;
        }
        if (timer_ < ATTACK_TIME_LIMIT)
        {
            if (boss.GetAttackStateSystem()->IsHit())
            {
                boss.GetPlayer().Damage(DAMAGE_VALUE);
            }
        }
        else 
        {
            if (!isSoundPlayed_)
            {
                PlaySoundMem(noHitSound_, DX_PLAYTYPE_BACK, true);
                isSoundPlayed_ = true;
            }
        }
    }

    if (boss.GetAnimation()->IsEnd())
    {
        boss.GetAttackStateSystem()->ClearHitRecord();
        isFinished_ = true;
    }
}

void ThrustAttack::Exit(EnemyBoss& boss)
{
}

bool ThrustAttack::IsFinished() const
{
    return isFinished_;
}

void ThrustAttack::StopEffect(void)
{
}

float ThrustAttack::GetCurrentSpeed(float t)
{
    if (t < DECELERATION_RATE)
    {
        // 加速フェーズ (最初は遅く、ピークへ)
        // ease-in-out のようなカーブを想定
        float progress = t / DECELERATION_RATE; // 0.0から1.0
        return INITIAL_SPEED + (PEAK_SPEED - INITIAL_SPEED) * sinf(progress * DX_PI_F / 2.0f); // 曲線的な加速
    }
    else 
    {
        // 減速フェーズ (ピークから0へ)
        float progress = (t - DECELERATION_RATE) / (1.0f - DECELERATION_RATE); // 0.0から1.0
        return PEAK_SPEED * (1.0f - progress); // 線形減衰 (必要に応じて曲線に)
    }
}

void ThrustAttack::InitSound(void)
{
    noHitSound_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::ATTACK_SE).handleId_;
    ChangeVolumeSoundMem(SOUND_VALUE, noHitSound_);
}
