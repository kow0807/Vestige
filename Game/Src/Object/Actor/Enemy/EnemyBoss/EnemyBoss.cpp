#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../../../../Application.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/Camera.h"
#include "../../../Common/AnimationController.h"
#include "../../../Common/Capsule.h"
#include "../../../Common/Collider.h"
#include "../../../Stage/Planet.h"
#include "../../Weapon/EnemyWeapon.h"
#include "../../Player/Player.h"
#include "../../Player/PlayerDummy.h"
#include "../../../Stage/Mist.h"

#include "State/EnemyBossStateMachine.h"
#include "Animation/EnemyBossAnimation.h"

#include "State/EnemyBossCombatSystem.h"
#include "State/EnemyBossCombatController.h"
#include "State/Attack/EnemyBossAttackStateMachine.h"

#include "../EnemyBoss/State/Idle/EIdleState.h"
#include "../EnemyBoss/State/Observe/EObserveState.h"
#include "../EnemyBoss/State/Move/EMoveState.h"
#include "../EnemyBoss/State/Chase/EChaseState.h"
#include "../EnemyBoss/State/Attack/EAttackState.h"
#include "../EnemyBoss/State/Step/EStepState.h"
#include "../EnemyBoss/State/Stun/EStunState.h"
#include "../EnemyBoss/State/Recover/ERecoverState.h"
#include "../EnemyBoss/State/Dead/EDeadState.h"

#include "State/Attack/Pattern/ComboAttack.h"
#include "State/Attack/Pattern/ThrustAttack.h"
#include "State/Attack/Pattern/RoarAttack.h"
#include "State/Attack/Pattern/ExplosionAttack.h"
#include "State/Attack/Pattern/SetUpAttack.h"

#include "EnemyBoss.h"

EnemyBoss::EnemyBoss(Player& player) 
	:
	player_(player),
	mist_(nullptr),
	weapon_(nullptr),
	stateMachine_(nullptr),
	animation_(nullptr),
	combat_(nullptr),
	combatController_(nullptr),
	attackFSM_(nullptr),
	hitPlayers_(0),
	hp_(0),
	maxHp_(0),
	nextAttackType_(ATK_STATE::NONE),
	moveDir_({}),
	movedPos_({}),
	movePow_({}),
	jumpPow_({}),
	isJump_(false),
	toPlayer_({}),
	gravHitPosDown_({}),
	gravHitPosUp_({}),
	damageHp_(0),
	colliders_({}),
	capsule_(nullptr),
	wristIndex_(0),
	wristPos({}),
	handIndex_(0),
	handPos({}),
	weaponTopIndex_(0),
	weaponTopPos_({}),
	weaponDownIndex_(0),
	weaponDownPos_({}),
	capsuleDir_({}),
	capsuleCenter_({}),
	radius_(0),
	isStunned_(false),
	stunTimer_(0.0f),
	effectStunPlayId_(-1),
	effectStunResId_(-1),
	isStunEffectActive_(false),
	effectBuffPlayId_(-1),
	effectBuffResId_(-1),
	isSetUpEffectActive_(false),
	currentAttackMultiplier_(0.0f),
	attackBuffTimer_(0.0f),
	isDead_(false),
	deadTimer_(0.0f),
	stepCooldown_(0.0f),
	hasUsedRoarBelow50Percent_(false),
	hasUsedExplosionBelow50Percent_(false),
	hasUsedSetupBelow40Percent_(false),
	sHandle_(0),
	isSound_(false)
{
	
}

EnemyBoss::~EnemyBoss(void)
{
	StopEffekseer3DEffect(effectBuffPlayId_);
	DeleteEffekseerEffect(effectBuffResId_);
}

void EnemyBoss::Init(void)
{
	// モデルの初期化
	InitModel();

	// 各種パラメータの初期値設定
	InitParam();
	
	// ステートマシンの初期化(行動管理)
	stateMachine_ = std::make_shared<EnemyStateMachine>(*this);
	InitStateMachine();

	// アニメーション関連の初期化
	animation_ = std::make_shared<EnemyBossAnimation>(transform_.modelId);
	InitAnimation();

	// 戦闘制御の初期化
	combat_ = std::make_shared<EnemyBossCombatSystem>(*this);
	combatController_ = std::make_shared<EnemyBossCombatController>(*this);

	// 攻撃パターンの状態遷移初期化
	attackFSM_ = std::make_shared<EnemyBossAttackStateMachine>();
	InitAttackPattern();
	
	// エフェクトとサウンドの初期化
	InitEffect();
	InitSound();

	// カプセルコライダ(当たり判定)設定
	capsule_ = std::make_shared<Capsule>(transform_);
	capsule_->SetLocalPosTop({ 0.0f, CAPSULE_TOP_LOCAL_POS_Y * SIZE, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, CAPSULE_DOWN_LOCAL_POS_Y, 0.0f });
	capsule_->SetRadius(CAPSULE_RADIUS * SIZE);

	// 武器初期化
	weapon_ = std::make_shared<EnemyWeapon>();
	weapon_->Init();

	// モデル内のフレームインデックス取得(武器と手の位置参照)
	wristIndex_ = MV1SearchFrame(transform_.modelId, "mixamorig:RightHand");
	handIndex_ = MV1SearchFrame(transform_.modelId, "mixamorig:RightHandMiddle1");
	weaponTopIndex_ = WEAPON_TOP_INDEX;
	weaponDownIndex_ = WEAPON_DOWN_INDEX;

	// 武器の先端・下端のワールド位置取得
	weaponTopPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponTopIndex_);
	weaponDownPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponDownIndex_);

	// 武器の当たり判定更新
	UpdateWeapon();
	UpdateWeaponCapsule();

	// 初期化のため行動はさせず、待機状態に設定
	stateMachine_->ChangeState(STATE::IDLE);
}

void EnemyBoss::Update(void)
{
	// 時間の取得
	float dt = SceneManager::GetInstance().GetDeltaTime();

	// プレイヤーとの距離の計算
	toPlayer_ = VSub(player_.GetTransform().pos, transform_.pos);
	
	// 攻撃対象とのY軸の差を無視してXY平面の方向に制限する
	toPlayer_.y = 0.0f;

	// 武器の当たり判定更新
	UpdateWeapon();
	UpdateWeaponCapsule();

	// アニメーション再生
	animation_->Update();

	// ダメージ時の薄いゲージの更新
	Damage();

	// 死亡状態が最優先
	if (isDead_)
	{
		return;
	}
	
	// プレイヤーがボスに攻撃をヒットさせた場合、HPを減算
	if (player_.IsHitAttackEnemyBoss())
	{
		hp_ -= player_.GetAttack();
	}

	// プレイヤーがライトニングを発動させた場合、ボスをスタンさせ、HPを減算
	if (player_.IsLightning()) 
	{
		SetStunned(STUN_TIME);
		hp_ -= LIGHTNING_DAMAGE;
	}

	// HPが0以下かつ現在の状態が死亡状態でない場合、死亡処理を開始
	if (hp_ <= 0 && stateMachine_->GetCurrentStateId() != STATE::DEAD) 
	{
		// 状態を死亡に切り替え、アニメーションや処理を開始
		SetDead();
	}

	// 現在の状態が死亡状態の場合の処理
	if (stateMachine_->GetCurrentStateId() == STATE::DEAD) 
	{
		// 死亡後の猶予時間を減少させる
		if (deadTimer_ > 0.0f) 
		{
			deadTimer_ -= dt;
			// 猶予時間が0以下になったら完全に死亡状態に移行
			if (deadTimer_ <= 0.0f) 
			{
				deadTimer_ = 0.0f;
				// 完全に死亡したことを示すフラグをセット
				SetIsDead(true);
			}
		}

		// 死亡時はすべてのエフェクトを停止する
		attackFSM_->StopEffect();
		StopStunEffect();
		StopSetupEffect();
		
		// 死亡状態固有の処理を行う
		if (stateMachine_)
		{
			stateMachine_->Update(*this);
		}

		// 死亡中は他の更新処理を行わず、ここで処理を終了
		return;
	}

	// ステップのクールダウンタイマーを更新
	// クールダウン中であれば、経過時間分だけ減らす
	if (stepCooldown_ > 0.0f)
	{
		stepCooldown_ -= dt;
		// マイナス値にならないように0でクリップ
		if (stepCooldown_ < 0.0f)
		{
			stepCooldown_ = 0.0f;
		}
	}

	// ボスのスタン状態時の更新
	if (isStunned_) 
	{
		// スタン状態の残り時間を減少させ、0以下ならスタン解除、残り時間は0に固定
		stunTimer_ -= dt;
		if (stunTimer_ <= 0.0f) 
		{
			isStunned_ = false;
			stunTimer_ = 0.0f;
		}
		else 
		{
			// スタン状態中はスタン用エフェクトを開始(未開始であれば)
			if (!IsStunEffectActive())
			{
				StartStunEffect();
			}
		}
	}
	else 
	{
		// スタン状態なければスタン用エフェクトを停止させる
		if (IsStunEffectActive())
		{
			StopStunEffect();
		}
	}

	// 攻撃力アップバフの効果時間を更新
	if (attackBuffTimer_ > 0.0f) 
	{
		attackBuffTimer_ -= dt;

		// バフ時間が終了した場合の処理
		if (attackBuffTimer_ <= 0.0f) 
		{
			RemoveAttackBuff();		// 攻撃力アップの効果を解除
			StopSetupEffect();		// バフ終了時にエフェクト終了
			StopSoundMem(sHandle_);	// バフ効果音の停止
		}
		else 
		{
			// バフが有効な間は、エフェクトとサウンドを再生する
			if (!IsSetupEffectActive()) 
			{
				StartSetupEffect();	// エフェクトを開始
				if (!isSound_)
				{
					// ループ再生で効果音を流す
					PlaySoundMem(sHandle_, DX_PLAYTYPE_LOOP, true);
					isSound_ = true;
				}
			}
		}
	}
	else 
	{
		// バフ効果が既に切れている場合は、エフェクトとサウンドを完全に停止
		if (IsSetupEffectActive()) 
		{
			StopSetupEffect();
			StopSoundMem(sHandle_);
		}
	}

	// ボスがスタン中の場合、他の状態更新をスキップ
	if (IsStunned()) 
	{
		// 現在のステートがSTUN以外であれば、STUNステートに強制遷移させる
		if (stateMachine_->GetCurrentStateId() != STATE::STUN) 
		{
			stateMachine_->ChangeState(STATE::STUN);
		}

		// スタン中はステートマシンの更新のみ行う(他の処理は行わない)
		stateMachine_->Update(*this);
		return;
	}

	// CombatController の内部クールダウンタイマーや攻撃管理の更新
	// クールダウン中の攻撃を制限したり、再選択可能な状態に戻す役割
	if (combatController_) 
	{
		combatController_->Update(dt);
	}

	// メインステートマシンの更新と、攻撃の選択ロジック
	if (stateMachine_)
	{
		// 現在のステートが「CHASE」または「IDLE」状態である場合、
		// 次の攻撃を開始するチャンスとして扱う
		if (stateMachine_->GetCurrentStateId() == STATE::CHASE ||
			stateMachine_->GetCurrentStateId() == STATE::IDLE)
		{
			// CombatControllerに攻撃候補を選ばせる(距離や状況に応して選択)
			EnemyBoss::ATK_STATE nextAttack = combatController_->SelectAttackCandidate();
			
			// 攻撃が選ばれた場合、(NONE以外)に攻撃状態へ遷移
			if (nextAttack != EnemyBoss::ATK_STATE::NONE) {
				
				// メインのステートを「攻撃(ATTACK)」状態に変更
				ChangeState(STATE::ATTACK);

				// 選択された特定の攻撃パターンをAttackStateMachineに設定
				SetNextAttackType(nextAttack);

				// 選択された攻撃に対応するクールダウンを設定
				combatController_->SetAttackCooldown(nextAttack);

				// 攻撃状態へ遷移したので、これ以降の更新はスキップ
				return;
			}
		}

		// 攻撃選択がなかった場合や、他のステートの場合の更新
		stateMachine_->Update(*this);
	}

	// 攻撃力アップエフェクトの位置をボスの現在の座標に追従させる
	SetPosPlayingEffekseer3DEffect(
				effectBuffPlayId_, 
				transform_.pos.x, transform_.pos.y, transform_.pos.z
				);

	// 重力影響計算
	CalcGravityPow();

	// 衝突判定処理(地面やプレイヤーとの接触など)
	Collision();

	// モデル制御更新
	transform_.Update();
}

void EnemyBoss::Draw(void)
{
	// 武器の描画
	weapon_->Draw();

	// モデルの描画
	MV1DrawModel(transform_.modelId);

	// デバックのみ
#ifdef _DEBUG
	//DrawDebug();
#endif // DEBUG

}

void EnemyBoss::SetPosition(VECTOR pos)
{
	transform_.pos = pos;
}

std::shared_ptr<EnemyStateMachine> EnemyBoss::GetStateMachine() const
{
	return stateMachine_;
}

std::shared_ptr<EnemyBossAnimation> EnemyBoss::GetAnimation() const
{
	return animation_;
}

std::shared_ptr<EnemyBossCombatSystem> EnemyBoss::GetAttackStateSystem() const
{
	return combat_;
}

std::shared_ptr<EnemyBossCombatController> EnemyBoss::GetAttackStateController() const
{
	return combatController_;
}

std::shared_ptr<EnemyBossAttackStateMachine> EnemyBoss::GetAttackStateMachine() const
{
	return attackFSM_;
}

void EnemyBoss::AddCollider(std::shared_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void EnemyBoss::ClearCollider(void)
{
	colliders_.clear();
}

const std::shared_ptr<Capsule> EnemyBoss::GetCapsule(void) const
{
	return capsule_;
}

void EnemyBoss::DrawDebug(void)
{
	// デバック表示(状態ステート)
	if (stateMachine_)
	{
		VECTOR screenPos = { transform_.pos.x,transform_.pos.y + DEBUG_STATE_POS_Y,transform_.pos.z };
		VECTOR screen2D = ConvWorldPosToScreenPos(screenPos);
		if (screen2D.z >= 0.0f)
		{
			stateMachine_->DrawDebugInfo(screen2D);
		}
	}

	// デバック表示(攻撃ステート)
	if (attackFSM_)
	{
		VECTOR screenPos = { transform_.pos.x,transform_.pos.y + DEBUG_ATTACK_POS_Y,transform_.pos.z };
		VECTOR screen2D = ConvWorldPosToScreenPos(screenPos);
		attackFSM_->DrawDebugInfo(screen2D);
	}

	DrawFormatString(DEBUG_POS_X, DEBUG_DIST_POS_Y, 0xffffff, "dist_=%f", GetPlayerDistanceXZ());
	DrawFormatString(DEBUG_POS_X, DEBUG_DEADTIME_POSY, 0xffffff, "deadt_=%f", deadTimer_);
	DrawFormatString(DEBUG_POS_X, DEBUG_ATTACKBUFF_POSY, 0xffffff, "bt_=%f", attackBuffTimer_);
}

VECTOR EnemyBoss::GetWeaponCenter(void)
{
	VECTOR top = weaponTopPos_;
	VECTOR down = weaponDownPos_;

	VECTOR diff = VSub(top, down);
	return VAdd(down, VScale(diff, 0.5f));
}

void EnemyBoss::UpdateWeapon(void)
{
	// 手首 → 手 の向きベクトル
	wristPos = MV1GetFramePosition(transform_.modelId, wristIndex_);
	handPos = MV1GetFramePosition(transform_.modelId, handIndex_);

	VECTOR dir = VSub(handPos, wristPos); // 向きベクトル
	dir = VNorm(dir); // 正規化して単位ベクトルにする

	// 向きから角度を算出
	float yaw = atan2f(dir.x, dir.z);
	float pitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
	float roll = WEAPON_ROLL;

	// 剣の回転を設定
	MV1SetRotationXYZ(weapon_->GetModelHandle(), VGet(pitch, yaw, roll));

	// 位置微調整
	float forwardOffset = WEAPON_FORWARD_OFFSET;
	float upOffset = WEAPON_UP_OFFSET;
	float rightOffset = WEAPON_RIGHT_OFFSET;

	// 右ベクトルと上ベクトルを簡易的に求める
	// 上方向
	VECTOR upVec = WEAPON_UPVEC;

	// 手の向きと上から右方向を作る
	VECTOR rightVec = VCross(upVec, dir);

	// 正規化
	rightVec = VNorm(rightVec); 

	// 上方向再調整
	upVec = VCross(dir, rightVec);
	upVec = VNorm(upVec);

	// オフセット合成
	VECTOR offsetPos = handPos;
	offsetPos = VAdd(offsetPos, VScale(dir, forwardOffset));   // 前方
	offsetPos = VAdd(offsetPos, VScale(upVec, upOffset));      // 上
	offsetPos = VAdd(offsetPos, VScale(rightVec, rightOffset));// 右

	// 剣の位置を設定
	MV1SetPosition(weapon_->GetModelHandle(), offsetPos);
}

void EnemyBoss::UpdateWeaponCapsule()
{
	// 手首から手の向きベクトルを求める
	weaponTopPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponTopIndex_);
	weaponDownPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponDownIndex_);

	// 武器方向ベクトル
	VECTOR dir = VSub(weaponTopPos_, weaponDownPos_);

	// 正規化して単位ベクトルにする
	dir = VNorm(dir); 

	// 位置微調整
	float forwardOffset = WEAPON_CAPSULE_FORWARD_OFFSET;
	float upOffset = WEAPON_CAPSULE_UP_OFFSET;
	float rightOffset = WEAPON_CAPSULE_RIGHT_OFFSET;

	// 右ベクトルと上ベクトルを簡易的に求める
	
	// 上方向
	VECTOR upVec = WEAPON_CAPSULE_UPVEC;
	
	// 手の向きと上から右方向を作る
	VECTOR rightVec = VCross(upVec, dir);
	
	// 正規化
	rightVec = VNorm(rightVec); 

	// 上方向再調整（手のひねりは考慮しない）
	upVec = VCross(dir, rightVec);
	upVec = VNorm(upVec);

	// オフセット合成
	VECTOR offsetPos = handPos;
	offsetPos = VAdd(offsetPos, VScale(dir, forwardOffset));   // 前方
	offsetPos = VAdd(offsetPos, VScale(upVec, upOffset));      // 上
	offsetPos = VAdd(offsetPos, VScale(rightVec, rightOffset));// 右

	// 剣の位置を設定
	MV1SetPosition(weapon_->GetModelHandle(), offsetPos);
}

void EnemyBoss::InitEffect(void)
{
	// バフエフェクト
	effectBuffResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::BUFF_EFFECT).handleId_;

	SetScalePlayingEffekseer3DEffect(effectBuffPlayId_, BUFF_EFFECT_SIZE, BUFF_EFFECT_SIZE, BUFF_EFFECT_SIZE);

	// スタンエフェクト
	effectStunResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::STUN_EFFECT).handleId_;

	SetScalePlayingEffekseer3DEffect(effectStunPlayId_, BUFF_EFFECT_SIZE, BUFF_EFFECT_SIZE, BUFF_EFFECT_SIZE);

}

void EnemyBoss::InitSound(void)
{
	sHandle_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::ULT_SE).handleId_;
	ChangeVolumeSoundMem(BUFF_EFFECT_SOUND, sHandle_);

	isSound_ = false;
}

int EnemyBoss::GetHp(void) const
{
	return hp_;
}

void EnemyBoss::ChangeState(STATE newState)
{
	if (stateMachine_)
	{
		stateMachine_->ChangeState(newState);
	}
}

void EnemyBoss::ChangeAttack(ATK_STATE newAttack)
{
	if (attackFSM_)
	{
		attackFSM_->Change(*this, newAttack);
	}
}

void EnemyBoss::SetNextAttackType(ATK_STATE type)
{
	nextAttackType_ = type;
}

EnemyBoss::ATK_STATE EnemyBoss::GetNextAttackType() const
{
	return nextAttackType_;
}

Player& EnemyBoss::GetPlayer()
{
	return player_;
}

Transform& EnemyBoss::GetTransformMutable()
{
	return transform_;
}

void EnemyBoss::SetStunned(float duration)
{
	isStunned_ = true;
	stunTimer_ = duration;
}

bool EnemyBoss::IsStunned(void) const
{
	return isStunned_;
}

float EnemyBoss::GetStunTimer(void) const
{
	return stunTimer_;
}

void EnemyBoss::SetAttackBuff(float duration, float muktiplier)
{
	attackBuffTimer_ = duration;
	currentAttackMultiplier_ = muktiplier;
}

void EnemyBoss::RemoveAttackBuff(void)
{
	attackBuffTimer_ = 0.0f;
	currentAttackMultiplier_ = 1.0f;
}

float EnemyBoss::GetCurrentAttackMultiplier() const
{
	return attackBuffTimer_;
}

bool EnemyBoss::IsDead() const
{
	return isDead_;
}

void EnemyBoss::SetIsDead(bool flag)
{
	isDead_ = flag;
}

void EnemyBoss::SetDead()
{
	ChangeState(STATE::DEAD);
	isStunned_ = false;
	stunTimer_ = 0.0f;
	SceneManager::GetInstance().GetCamera()->ChangeGameCamera(Camera::GAME_CAMERA::MOUSE);
}

float EnemyBoss::GetDeadTimer() const
{
	return deadTimer_;
}

void EnemyBoss::OnDeathAnimationFinished(void)
{
	deadTimer_ = DEAD_INTERVAL_TIME;
}

void EnemyBoss::StartStunEffect(void)
{
	if (!isStunEffectActive_)
	{
		isStunEffectActive_ = true;
	}

	effectStunPlayId_ = PlayEffekseer3DEffect(effectStunResId_);

	SetScalePlayingEffekseer3DEffect(effectStunPlayId_, STUN_EFFECT_SIZE, STUN_EFFECT_SIZE, STUN_EFFECT_SIZE);

	SetPosPlayingEffekseer3DEffect(effectStunPlayId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
}

void EnemyBoss::StopStunEffect(void)
{
	if (isStunEffectActive_) 
	{
		isStunEffectActive_ = false;
		StopEffekseer3DEffect(effectStunPlayId_);
	}
}

bool EnemyBoss::IsStunEffectActive(void)
{
	return isStunEffectActive_;
}

void EnemyBoss::StartSetupEffect()
{
	if (!isSetUpEffectActive_)
	{
		isSetUpEffectActive_ = true;
	}

	effectBuffPlayId_ = PlayEffekseer3DEffect(effectBuffResId_);

	SetScalePlayingEffekseer3DEffect(effectBuffPlayId_, BUFF_EFFECT_SIZE, BUFF_EFFECT_SIZE, BUFF_EFFECT_SIZE);

	SetPosPlayingEffekseer3DEffect(effectBuffPlayId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
}

void EnemyBoss::StopSetupEffect()
{
	if (isSetUpEffectActive_) 
	{
		isSetUpEffectActive_ = false;
		StopEffekseer3DEffect(effectBuffPlayId_);
	}
}

bool EnemyBoss::IsSetupEffectActive()
{
	return isSetUpEffectActive_;
}

VECTOR EnemyBoss::GetTopPos()
{
	return weaponTopPos_;
}

VECTOR EnemyBoss::GetDownPos()
{
	return weaponDownPos_;
}

int EnemyBoss::GetMaxHp() const
{
	return maxHp_;
}

float EnemyBoss::GetHpRatio() const
{
	if (maxHp_ <= 0)
	{
		return 0.0f;
	}
	else
	{
		return static_cast<float>(hp_) / maxHp_;
	}
}

EnemyBoss::STATE EnemyBoss::GetCurrentStateId() const
{
	return stateMachine_->GetCurrentStateId();
}

float EnemyBoss::GetPlayerDistanceXZ() const
{
	return VSize(toPlayer_);
}

VECTOR EnemyBoss::GetToPlayer()
{
	return toPlayer_;
}

void EnemyBoss::InitModel(void)
{
	// モデルの基本設定
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_BOSS));
	transform_.scl = { SIZE, SIZE, SIZE };
	transform_.pos = INIT_POS;
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ INIT_QUAROT_LOCAL_X, AsoUtility::Deg2RadF(INIT_QUAROT_LOCAL_Y), INIT_QUAROT_LOCAL_Z });
	transform_.Update();
}

void EnemyBoss::InitParam(void)
{
	// パラメータの基本設定
	hp_ = MAX_HP_VALUE;
	maxHp_ = hp_;
	damageHp_ = hp_;
	isDead_ = false;
	nextAttackType_ = ATK_STATE::NONE;
	hasUsedSetupBelow40Percent_ = false;
}

void EnemyBoss::InitStateMachine(void)
{
	stateMachine_->Add(STATE::IDLE, std::make_shared<EIdleState>());
	stateMachine_->Add(STATE::OBSERVE, std::make_shared<EObserveState>());
	stateMachine_->Add(STATE::MOVE, std::make_shared<EMoveState>());
	stateMachine_->Add(STATE::CHASE, std::make_shared<EChaseState>());
	stateMachine_->Add(STATE::ATTACK, std::make_shared<EAttackState>());
	stateMachine_->Add(STATE::STEP, std::make_shared<EStepState>());
	stateMachine_->Add(STATE::STUN, std::make_shared<EStunState>());
	stateMachine_->Add(STATE::RECOVER, std::make_shared<ERecoverState>());
	stateMachine_->Add(STATE::DEAD, std::make_shared<EDeadState>());

}

void EnemyBoss::InitAnimation(void)
{
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::IDLE, "idle.mv1", IDLE_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::WALK_F, "Walk Forward.mv1", WALK_F_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::WALK_B, "Standing Walk Back.mv1", WALK_B_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::RUN, "Unarmed Run Forward.mv1", RUN_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::STEP, "Jump.mv1", STEP_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::COMBO, "Standing Melee Attack Downward.mv1", COMBO_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::THRUST, "Upward Thrust.mv1", THRUST_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::ROAR, "Standing Taunt Battlecry.mv1", ROAR_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::EXPLOSION, "Sword And Shield Casting.mv1", EXPLOSION_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::STUN_IDLE, "Standing React Large From Left.mv1", STUN_IDLE_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::SETUP, "Standing React Large From Left.mv1", SET_UP_ANIM_SPEED);
	animation_->AddAnimation(EnemyBossAnimation::ANIM_TYPE::DEATH, "Two Handed Sword Death.mv1", DEATH_ANIM_SPEED);

	animation_->Play(EnemyBossAnimation::ANIM_TYPE::IDLE, true, ANIM_BLEND_RATE);
}

void EnemyBoss::InitAttackPattern(void)
{
	attackFSM_->Add(EnemyBoss::ATK_STATE::COMBO, std::make_shared<ComboAttack>(*this));
	attackFSM_->Add(EnemyBoss::ATK_STATE::THRUST, std::make_shared<ThrustAttack>(*this));
	attackFSM_->Add(EnemyBoss::ATK_STATE::ROAR, std::make_shared<RoarAttack>(*this));
	attackFSM_->Add(EnemyBoss::ATK_STATE::EXPLOSION, std::make_shared<ExplosionAttack>(*this));
	attackFSM_->Add(EnemyBoss::ATK_STATE::SETUP, std::make_shared<SetUpAttack>(*this));
}

void EnemyBoss::Collision(void)
{
	// 現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	// 衝突(カプセル)
	CollisionCapsule();

	// 衝突(重力)
	CollisionGravity();

	// 移動
	transform_.pos = movedPos_;
}

void EnemyBoss::CollisionGravity(void)
{
	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	// 頭上判定時の押出力
	float checkPow = GRAVITY_CHECK_POW;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));

	//天井衝突チェックのために広めに2.0fをかけて判定をとる
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));
	for (const auto& c : colliders_)
	{
		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > FALLINT_DOT_THRESHOLD)
		{
			// 衝突地点から、少し上に移動
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));

			// ジャンプリセット
			jumpPow_ = AsoUtility::VECTOR_ZERO;
			isJump_ = false;
		}
	}
}

void EnemyBoss::CollisionCapsule(void)
{
	// カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);

	// カプセルとの衝突判定
	for (const auto& c : colliders_)
	{
		auto hits = MV1CollCheck_Capsule(
			c->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];
			for (int tryCnt = 0; tryCnt < CAPSULE_LIMIT_COUNT; tryCnt++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));

					// カプセルを移動させる
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}
				break;
			}
		}
		MV1CollResultPolyDimTerminate(hits);
	}
}

void EnemyBoss::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		jumpPow_ = gravity;
	}
}

void EnemyBoss::Damage(void)
{
	float speed = DECREASE_SPEED; // 減少速度調整

	// ダメージ時の薄いゲージをゆっくり減らす
	if (damageHp_ > hp_)
	{
		damageHp_ -= static_cast<int>(speed);
		if (damageHp_ < hp_) damageHp_ = hp_;
	}
}

void EnemyBoss::DrawHpBar(void)
{
	int width = WIDTH;
	int height = HEIGHT;

	int x = (Application::SCREEN_SIZE_X - width) / 2 + X_ADJUSTMENT;
	int y = Application::SCREEN_SIZE_Y - height - Y_ADJUSTMENT;

	float hpRate = static_cast<float>(hp_) / maxHp_;
	int filledWidth = width;

	// 敵の名前表示準備
	const char* enemyName = "門の兵士";
	SetFontSize(FONT_SIZE);
	int nameWidth = GetDrawStringWidth(enemyName, static_cast<int>(strlen(enemyName)));
	int nameHeight = NAME_HEIGHT;

	int paddingLeft = PADDING_LEFT;
	int nameX = x + paddingLeft;
	int nameY = y - nameHeight - PADDING;

	int padding = PADDING;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, NAME_BLEND_PARAM);
	DrawBox(nameX - padding / 2, nameY - padding / 2,
	nameX + nameWidth + padding, nameY + nameHeight + padding / 2,
	GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	int outlineColor = GetColor(0, 0, 0);
	int textColor = GetColor(TEXCOL_R, TEXCOL_G, TEXCOL_B);
	DrawString(nameX - 1, nameY, enemyName, outlineColor);
	DrawString(nameX + 1, nameY, enemyName, outlineColor);
	DrawString(nameX, nameY - 1, enemyName, outlineColor);
	DrawString(nameX, nameY + 1, enemyName, outlineColor);
	DrawString(nameX, nameY, enemyName, textColor);

	// 色設定
	int bgColor = GetColor(A_QUARTER_COL, A_QUARTER_COL, A_QUARTER_COL);          // バー背景（暗い灰色）
	
	// ダメージ色は白をベースに赤のグラデーションで透過的に表現
	int damageColorLight = GetColor(DAMEGE_COL_LIGHT_R, DAMEGE_COL_LIGHT_GB, DAMEGE_COL_LIGHT_GB);		// ダメージ部分明るい色（白っぽい赤）
	int damageColorTop = GetColor(DAMEGE_COL_TOP_R, DAMEGE_COL_TOP_GB, DAMEGE_COL_TOP_GB);				// ダメージ上半分（明るい赤）
	int damageColorBottom = GetColor(DAMEGE_COL_BOTTOM_R, DAMEGE_COL_BOTTOM_GB, DAMEGE_COL_BOTTOM_GB);  // ダメージ下半分（暗い赤）

	int hpColorTop = GetColor(HP_COL_TOP_R, HP_COL_TOP_GB, HP_COL_TOP_GB);				// HPバー上半分（明るい赤）
	int hpColorBottom = GetColor(HP_COL_BOTTOM_R, HP_COL_BOTTOM_GB, HP_COL_BOTTOM_GB);  // HPバー下半分（暗い赤）
	int borderDark = GetColor(BORDER_DARK_COL, BORDER_DARK_COL, BORDER_DARK_COL);       // 外枠（暗め）
	int borderLight = GetColor(BORDER_LIGHT_COL, BORDER_LIGHT_COL, BORDER_LIGHT_COL);   // 内枠（明るめ）

	// 外枠（2重で立体感）
	DrawBox(x - BORDER_DARK_ADJUSTMENT, y - BORDER_DARK_ADJUSTMENT, x + width + BORDER_DARK_ADJUSTMENT, y + height + BORDER_DARK_ADJUSTMENT, borderDark, TRUE);
	DrawBox(x - BORDER_LIGHT_ADJUSTMENT, y - BORDER_LIGHT_ADJUSTMENT, x + width + BORDER_LIGHT_ADJUSTMENT, y + height + BORDER_LIGHT_ADJUSTMENT, borderLight, TRUE);

	// 背景バー
	DrawBox(x, y, x + width, y + height, bgColor, TRUE);

	if (hp_ > 0)
	{
		// ダメージ部分の描画（半透明＆グラデーション）
		int damageW = static_cast<int>(filledWidth * damageHp_ / maxHp_);
		if (damageW > 0)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, DAMAGE_BLEND_PARAM);

			// 白ベースの透過的な赤部分
			DrawBox(x, y, x + damageW, y + height, damageColorLight, TRUE);

			// 赤のグラデーション上下に描画
			int halfH = height / 2;
			DrawBox(x, y, x + damageW, y + halfH, damageColorTop, TRUE);
			DrawBox(x, y + halfH, x + damageW, y + height, damageColorBottom, TRUE);

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		// 現在HPバー（上下で色分け）
		int hpW = static_cast<int>(filledWidth * hpRate);
		if (hpW > 0)
		{
			int halfH = height / 2;
			DrawBox(x, y, x + hpW, y + halfH, hpColorTop, TRUE);
			DrawBox(x, y + halfH, x + hpW, y + height, hpColorBottom, TRUE);
		}
	}

	// 枠線（念のため）
	DrawBox(x, y, x + width, y + height, textColor, FALSE);
}

void EnemyBoss::LookAt(VECTOR pos)
{
	VECTOR BossPos = transform_.pos;
	VECTOR targetPosXZ = pos;
	targetPosXZ.y = BossPos.y;//y軸無視(水平回転のみ)

	VECTOR dir = VSub(targetPosXZ, BossPos);
	float length = AsoUtility::MagnitudeF(dir);
	if (length < LENGTH_LIMIT)return;

	dir = VNorm(dir);
	Quaternion lookRot = Quaternion::LookRotation(dir);

	const float slerpRotio = SLERP_ROTIO;
	// スムーズに回転(球面線形補間)
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, lookRot, slerpRotio);
}

void EnemyBoss::MoveToward(const VECTOR& target)
{
	VECTOR dir = AsoUtility::VNormalize(VSub(target, transform_.pos));
	transform_.pos.x += dir.x * MOVE_SPEED;
	transform_.pos.z += dir.z * MOVE_SPEED;
}

void EnemyBoss::DrawCircleOnGround(VECTOR center, float radius, int segment, int color)
{
	float angleStep = static_cast<float>(DX_TWO_PI) / segment;
	for (int i = 0; i < segment; ++i) 
	{
		float angle1 = i * angleStep;
		float angle2 = (i + 1) * angleStep;

		VECTOR p1 = VAdd(center, VGet(cosf(angle1) * radius, 0.0f, sinf(angle1) * radius));
		VECTOR p2 = VAdd(center, VGet(cosf(angle2) * radius, 0.0f, sinf(angle2) * radius));

		DrawLine3D(p1, p2, color);
	}
}

bool EnemyBoss::IsPlayerInFront(float viewAngleDeg, float maxDistance)
{
	VECTOR playerPos = GetPlayer().GetTransform().pos;
	VECTOR bossPos = transform_.pos;
	VECTOR toPlayer = VSub(playerPos, bossPos);

	// 距離判定
	float distToPlayer = AsoUtility::MagnitudeF(toPlayer);
	if (distToPlayer > maxDistance) 
	{
		return false; // プレイヤーが視野範囲の最大距離外にいる
	}

	// 敵の正面方向ベクトルを計算
	// transform_.quaRot から Yaw (Y軸回転) 角度を取得
	VECTOR rad = Quaternion::ToEuler(transform_.quaRot);

	// Quaternion::ToEuler は XYZ の順でオイラー角を返すので、Y軸回転が rad.y に対応
	float bossYaw = rad.y; // ボスのY軸回転（左右の向き）

	// Y軸回転から正面ベクトルを計算
	// お示しいただいた計算式と同じ方法
	float forwardX = sinf(bossYaw);
	float forwardZ = cosf(bossYaw);
	VECTOR bossForward = VGet(forwardX, 0.0f, forwardZ); // Y軸は無視 (地面に平行な方向)

	// 敵からプレイヤーへの方向ベクトルと、敵の正面方向ベクトルのなす角度を計算
	toPlayer.y = 0.0f; // プレイヤーへのベクトルもY軸は無視し、水平方向の判定にする
	toPlayer = VNorm(toPlayer); // 正規化

	// 内積を使って角度のコサイン値を取得
	float dotProduct = VDot(bossForward, toPlayer);

	// 浮動小数点誤差による問題を避けるためクランプ
	dotProduct = fmaxf(-1.0f, fminf(1.0f, dotProduct));

	// なす角度（ラジアン）を計算
	float angleToPlayerRadians = acosf(dotProduct);

	// 視野角の閾値をラジアンに変換 (viewAngleDeg は片側の角度)
	float viewAngleRadians = AsoUtility::Deg2RadF(viewAngleDeg);

	// なす角度が視野角の範囲内か判定
	return angleToPlayerRadians <= viewAngleRadians;
}

bool EnemyBoss::HasUsedRoarBelow50Percent(void)
{
	return hasUsedRoarBelow50Percent_;
}

void EnemyBoss::SetHasUsedRoarBelow50Percent(bool flag)
{
	hasUsedRoarBelow50Percent_ = flag;
}

bool EnemyBoss::HasUsedExplosionBelow50Percent(void)
{
	return hasUsedExplosionBelow50Percent_;
}

void EnemyBoss::SetHasUsedExplosionBelow50Percent(bool flag)
{
	hasUsedExplosionBelow50Percent_ = flag;
}

bool EnemyBoss::HasUsedSetupBelow40Percent(void)
{
	return hasUsedSetupBelow40Percent_;
}

void EnemyBoss::SetHasUsedSetupBelow40Percent(bool flag)
{
	hasUsedSetupBelow40Percent_ = flag;
}

void EnemyBoss::CollisionMist(void)
{
	for (int h = 0; h < mist_->GetMistTransforms().size(); h++)
	{
		MV1RefreshCollInfo(mist_->GetTransform().modelId);

		// カプセルとの衝突判定
		auto hits = MV1CollCheck_Capsule(
			mist_->GetMistTransforms()[h].modelId, -1,
			capsule_->GetPosTop(), capsule_->GetPosDown(), CAPSULE_RADIUS);

		// 衝突した複数のポリゴンと衝突回避するまで、
		// プレイヤーの位置を移動させる
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];
			// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
			// この時、移動させる方向は、移動前座標に向いた方向であったり、
			// 衝突したポリゴンの法線方向だったりする
			for (int tryCnt = 0; tryCnt < 12; tryCnt++)
			{
				// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
				// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
				int pHit = HitCheck_Capsule_Triangle(
					capsule_->GetPosTop(), capsule_->GetPosDown(), CAPSULE_RADIUS,
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					// 法線の方向にちょっとだけ移動させる
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					// カプセルも一緒に移動させる
					transform_.pos = movedPos_;
					transform_.Update();
					continue;
				}
				break;
			}
		}
		MV1CollResultPolyDimTerminate(hits);
	}
}

float EnemyBoss::GetStepCooldown(void)
{
	return stepCooldown_;
}

void EnemyBoss::SetStepCooldown(float duration)
{
	stepCooldown_ = duration;
}

void EnemyBoss::SetMist(std::shared_ptr<Mist> mist)
{
	mist_ = mist;
}
