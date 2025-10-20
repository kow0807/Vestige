#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "RoarAttack.h"
#include "../../../../../../../Utility/AsoUtility.h"
#include "../../../../../../../Manager/SceneManager.h"
#include "../../../../../../../Manager/ResourceManager.h"
#include "../../../EnemyBoss.h"
#include "../../../Animation/EnemyBossAnimation.h"
#include "../../../../../Player/Player.h"

RoarAttack::RoarAttack(EnemyBoss& boss) 
    :
    boss_(boss)
{
    isFinished_ = false;
    waveTimer_ = 0.0f;
    effectRadius_ = 0.0f;
    waveAtMax_ = false;
    hasStunnedPlayer_ = false;

    InitEffect();
}

RoarAttack::~RoarAttack(void)
{
    DeleteEffekseerEffect(effectRoarFResId_);
    DeleteEffekseerEffect(effectRoarSResId_);
}

void RoarAttack::Enter(EnemyBoss& boss)
{
    isFinished_ = false;
    hasStunnedPlayer_ = false;
    timer_ = 0.0f;
    waveTimer_ = 0.0f;
    waveAtMax_ = false;
    IsEffect_ = false;
    isSoundPlayed_ = false;
    boss.GetAnimation()->Play(EnemyBossAnimation::ANIM_TYPE::ROAR, false, EnemyBoss::ANIM_BLEND_RATE);
}

void RoarAttack::Update(EnemyBoss& boss)
{
    timer_ += SceneManager::GetInstance().GetDeltaTime();
    waveTimer_ += SceneManager::GetInstance().GetDeltaTime();

    if (!IsEffect_)
    {
        // エフェクト01
        effectRoarFPlayId_ = PlayEffekseer3DEffect(effectRoarFResId_);

        // エフェクト再生を開始し、インスタンスハンドルを取得
        effectRoarSPlayId_ = PlayEffekseer3DEffect(effectRoarSResId_);

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

    // スタン発生タイミングになったらプレイヤーをスタンさせる
    if (!hasStunnedPlayer_ && timer_ >= STUN_TRIGGER_TIME)
    {
        if (!waveAtMax_)
        {
            float ratio = waveTimer_ / 1.0f;

            if (ratio >= 1.0f)
            {
                //最大に達したらリセット
                effectRadius_ = ATTACK_MAX_RANGE;
                waveAtMax_ = true;
                waveTimer_ = 0.0f;
            }
            else
            {
                effectRadius_ = ATTACK_MAX_RANGE * ratio;
            }
        }
        else
        {
            effectRadius_ = 0.0f;
            waveAtMax_ = false;
        }

        // 咆哮範囲か判定
        float dist = AsoUtility::MagnitudeF(VSub(boss.GetPlayer().GetTransform().pos, boss.GetTransform().pos));
        if (dist <= effectRadius_) {
            // プレイヤーをスタン状態にする
            boss.GetPlayer().StunUpdate(); 
        }
        hasStunnedPlayer_ = true; // 一度だけスタンさせる
    }

    UpdateEffect(boss);

    if (timer_ >= TIME_LIMIT)
    {
        StopEffekseer3DEffect(effectRoarFPlayId_);
        StopEffekseer3DEffect(effectRoarSPlayId_);
        timer_ = 0.0f;
        waveTimer_ = 0.0f;
        waveAtMax_ = false;
        isSoundPlayed_ = false;
        IsEffect_ = false;
        isFinished_ = true;
    }
}

void RoarAttack::Exit(EnemyBoss& boss)
{
    
}

bool RoarAttack::IsFinished() const
{
    return isFinished_;
}

void RoarAttack::StopEffect(void)
{
    StopEffekseer3DEffect(effectRoarFPlayId_);
    StopEffekseer3DEffect(effectRoarSPlayId_);
}

void RoarAttack::InitEffect(void)
{
    // エフェクト
    effectRoarFResId_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::ROAR_01).handleId_;

    // 大きさ
    SetScalePlayingEffekseer3DEffect(effectRoarFPlayId_, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);

    // エフェクト
    effectRoarSResId_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::ROAR_02).handleId_;

    SetScalePlayingEffekseer3DEffect(effectRoarSPlayId_, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);
}

void RoarAttack::UpdateEffect(EnemyBoss& boss)
{
    // 大きさ
    SetScalePlayingEffekseer3DEffect(effectRoarFPlayId_, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);

    SetPosPlayingEffekseer3DEffect(effectRoarFPlayId_, boss.GetTransform().pos.x, boss.GetTransform().pos.y + F_EFFECT_POS_Y, boss.GetTransform().pos.z);
    
    VECTOR deg = Quaternion::ToEuler(boss.GetTransform().quaRot);

    int index = MV1SearchFrame(boss.GetTransform().modelId, "mixamorig:Head");

    VECTOR pos = MV1GetFramePosition(boss.GetTransform().modelId, index);

    SetScalePlayingEffekseer3DEffect(effectRoarSPlayId_, EFFECT_SIZE, EFFECT_SIZE, S_EFFECT_SIZE_Z);

    SetPosPlayingEffekseer3DEffect(effectRoarSPlayId_, pos.x, pos.y, pos.z);

    SetRotationPlayingEffekseer3DEffect(effectRoarSPlayId_, deg.x, deg.y, deg.z);
}

void RoarAttack::InitSound(void)
{
    noHitSound_ = ResourceManager::GetInstance().Load(
        ResourceManager::SRC::ROAR_SE).handleId_;

    ChangeVolumeSoundMem(SOUND_VALUE, noHitSound_);
}
