#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "ExplosionAttack.h"
#include "../../../../../../../Utility/AsoUtility.h"
#include "../../../../../../../Manager/SceneManager.h"
#include "../../../../../../../Manager/ResourceManager.h"
#include "../../../EnemyBoss.h"
#include "../../../Animation/EnemyBossAnimation.h"
#include "../../../../../Player/Player.h"
#include "../../EnemyBossStateMachine.h"

ExplosionAttack::ExplosionAttack(EnemyBoss& boss) 
    :
    boss_(boss)
{
    selfStunDuration_ = SELF_STUN_DURATION;
    targetPosition_ = AsoUtility::VECTOR_ZERO;
    isAttack_ = false;
    targetPosionFixed_ = false;


    InitEffect();
	InitSound();
}

ExplosionAttack::~ExplosionAttack(void)
{
    DeleteEffekseerEffect(effectBuffResId_);
}

void ExplosionAttack::Enter(EnemyBoss& boss)
{
    isFinished_ = false;
    timer_ = 0.0f;
    isAttack_ = false;
    targetPosionFixed_ = false;
    reachedTargetPos_ = false;
    IsEffect_ = false;
    isSoundPlayed_ = false;
    hasSelfStunned_ = false;
}

void ExplosionAttack::Update(EnemyBoss& boss)
{
    timer_ += SceneManager::GetInstance().GetDeltaTime();

    // フェーズ1: ターゲット固定と予兆エフェクト開始
    if (!targetPosionFixed_)
    {
        // ターゲット位置を毎フレーム更新し続ける
        targetPosition_ = boss.GetPlayer().GetTransform().pos;

        if (timer_ >= TARGET_LOCK_TIME) // 予兆時間が経過したらターゲットを固定
        {
            targetPosionFixed_ = true;
        }
    }
    // フェーズ2: 固定されたターゲット位置への移動
    else if (!reachedTargetPos_)
    {
        VECTOR dirVec = VSub(targetPosition_, boss.GetTransform().pos);
        if (AsoUtility::MagnitudeF(dirVec) > DIRECTION_VEC_LIMIT)
        {
            boss.LookAt(targetPosition_);
        }
        boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::EXPLOSION, false, EnemyBoss::ANIM_BLEND_RATE);
    }

    if (!IsEffect_ && effectExploPlayId_ != -1) // 予兆エフェクトは最初に一度だけ開始
    {
        UpdateEffect();
        IsEffect_ = true;
    }

    if (!isSoundPlayed_ && timer_ >= SOUND_START_TIME)
    {
        if (noHitSound_ != -1)
        {
            PlaySoundMem(noHitSound_, DX_PLAYTYPE_BACK, true);
            isSoundPlayed_ = true;
        }
    }

    if (timer_ >= ATTACK_DIRECTION_TIME && !isAttack_)
    {
        // 攻撃判定
        float pDist = AsoUtility::MagnitudeF(VSub(boss.GetPlayer().GetTransform().pos, targetPosition_));
        if (pDist <= ATTACK_RADIUS)  // 爆発範囲
        {
            boss.GetPlayer().Damage(DAMEGE_VALUE);
            isAttack_ = true;
        }
    }

    // フェーズ4: 攻撃終了と自己スタン
    if (timer_ >= EXPLOSION_END_TIME) // 攻撃の総時間が経過したら終了
    {
        if (!hasSelfStunned_)
        {
            boss.SetStunned(selfStunDuration_); // ボス自身をスタン状態にする
            hasSelfStunned_ = true;
        }
        StopEffekseer3DEffect(effectExploPlayId_); // エフェクトを停止

        reachedTargetPos_ = true;
        isSoundPlayed_ = false;
        isFinished_ = true;
    }

}

void ExplosionAttack::Exit(EnemyBoss& boss)
{
}

bool ExplosionAttack::IsFinished() const
{
    return isFinished_;
}

void ExplosionAttack::StopEffect(void)
{
    StopEffekseer3DEffect(effectExploPlayId_);
}

void ExplosionAttack::InitEffect(void)
{
    // エフェクト
    effectBuffResId_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::EXPLOSION_EFFECT).handleId_;

    // 大きさ
    SetScalePlayingEffekseer3DEffect(effectExploPlayId_, EFFECT_SIZE_XZ, EFFECT_SIZE_Y, EFFECT_SIZE_XZ);

}

void ExplosionAttack::UpdateEffect(void)
{
    // エフェクト再生を開始し、インスタンスハンドルを取得
    effectExploPlayId_ = PlayEffekseer3DEffect(effectBuffResId_);

    SetScalePlayingEffekseer3DEffect(effectExploPlayId_, EFFECT_SIZE_XZ, EFFECT_SIZE_Y, EFFECT_SIZE_XZ);

    SetPosPlayingEffekseer3DEffect(effectExploPlayId_, targetPosition_.x, targetPosition_.y - EFFECT_POS_Y_ADJUSTMENT, targetPosition_.z);
}

void ExplosionAttack::InitSound(void)
{
    noHitSound_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::EXP_SE).handleId_;

    ChangeVolumeSoundMem(SOUND_VALUE, noHitSound_);
}
