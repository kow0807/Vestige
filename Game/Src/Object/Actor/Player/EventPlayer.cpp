#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Camera.h"
#include "../../Common/AnimationController.h"
#include "../../Common/Capsule.h"
#include "../../Common/Collider.h"
#include "../Weapon/PlayerWeapon.h"
#include "../../Stage/Planet.h"
#include "EventPlayer.h"

EventPlayer::EventPlayer()
	:
	animationController_(nullptr),
	state_(STATE::NONE),
	anim_(ANIM_TYPE::NONE),
	speed_(0.0f),
	moveDir_({}),
	movePow_({}),
	movedPos_({}),
	playerRotY_({}),
	goalQuaRot_({}),
	stepRotTime_(0.0f),
	jumpPow_({}),
	isJump_(false),
	capsule_(nullptr),
	gravHitPosDown_({}),
	gravHitPosUp_({}),
	weaponBoneIndex_(-1),
	wristIndex_(-1),
	wristPos({}),
	handIndex_(-1),
	handPos({}),
	weaponTopIndex_(-1),
	weaponTopPos_({}),
	weaponDownIndex_(-1),
	weaponDownPos_({}),
	sHandle_(-1),
	isSound_(false)
{
}

EventPlayer::~EventPlayer()
{
}

void EventPlayer::Init(void)
{
	// モデルの基本設定
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.pos = INIT_POS;
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ INIT_QUAROT_LOCAL_X, 
							AsoUtility::Deg2RadF(INIT_QUAROT_LOCAL_Y),
							INIT_QUAROT_LOCAL_Z });
	transform_.Update();

	// アニメーションの設定
	InitAnimation();

	sHandle_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::WALK_BGM).handleId_;
	ChangeVolumeSoundMem(SOUND_VALUE, sHandle_);

	isSound_ = false;

	// カプセルコライダ
	capsule_ = std::make_shared<Capsule>(transform_);
	capsule_->SetLocalPosTop(CAPSULE_POS_TOP);
	capsule_->SetLocalPosDown(CAPSULE_POS_DOWN);
	capsule_->SetRadius(CAPUSULE_RADIUS);

	weapon_ = std::make_shared<PlayerWeapon>();
	weapon_->Init();

	// フレーム取得
	wristIndex_ = MV1SearchFrame(transform_.modelId, "mixamorig:RightHand");
	handIndex_ = MV1SearchFrame(transform_.modelId, "mixamorig:RightHandMiddle1");

	weaponTopIndex_ = WEAPON_TOP_INDEX;
	weaponDownIndex_ = WEAPON_DOWN_INDEX;

	weaponTopPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponTopIndex_);
	weaponDownPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponDownIndex_);

	weaponTopPos_ = WeaponTopPos(WEAPON_TOP_POS);

	UpdateRightWeapon();
	UpdateWeapon();

	// 初期状態
	ChangeState(STATE::IDLE);
	SceneManager::GetInstance().GetCamera()->ChangeGameCamera(Camera::GAME_CAMERA::MOUSE);

}

void EventPlayer::Update(void)
{
	// 更新ステップ
	switch (state_)
	{
	case EventPlayer::STATE::NONE:
		UpdateNone();
		break;
	case EventPlayer::STATE::IDLE:
		UpdateIdle();
		break;
	case EventPlayer::STATE::WALK:
		UpdateWalk();
		break;
	case EventPlayer::STATE::LOOK:
		UpdateLook();
		break;
	case EventPlayer::STATE::STANDBY:
		UpdateStandBy();
		break;
	default:
		break;
	}

	// モデル制御更新
	transform_.Update();

	// アニメーション再生
	animationController_->Update();

	UpdateRightWeapon();
	UpdateWeapon();

}

void EventPlayer::Draw(void)
{
	weapon_->Draw();

	// モデルの描画
	MV1DrawModel(transform_.modelId);
}

void EventPlayer::SetPosition(VECTOR pos)
{
	transform_.pos = pos;
}

void EventPlayer::AddCollider(std::shared_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void EventPlayer::ClearCollider(void)
{
	colliders_.clear();
}

const std::shared_ptr<Capsule> EventPlayer::GetCapsule(void) const
{
	return std::shared_ptr<Capsule>();
}

EventPlayer::ANIM_TYPE EventPlayer::GetAnimType()
{
	return anim_;
}

void EventPlayer::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", IDLE_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::WALK, path + "Unarmed Walk Forward.mv1", WALK_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::LOOK, path + "Look_Around_Idle.mv1", LOOK_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::STANDBY, path + "Stable Sword Inward Slash.mv1", STANDBY_ANIM_SPEED);

	animationController_->Play((int)ANIM_TYPE::IDLE);
}

void EventPlayer::ChangeState(STATE state)
{
	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	switch (state_)
	{
	case EventPlayer::STATE::NONE:
		ChangeStateNone();
		break;
	case EventPlayer::STATE::IDLE:
		ChangeStateIdle();
		break;
	case EventPlayer::STATE::WALK:
		ChangeStateWalk();
		break;
	case EventPlayer::STATE::LOOK:
		ChangeStateLook();
		break;
	case EventPlayer::STATE::STANDBY:
		ChangeStateStandBy();
		break;
	default:
		break;
	}
}

void EventPlayer::ChangeStateNone(void)
{
}

void EventPlayer::ChangeStateIdle(void)
{
	animationController_->Play((int)ANIM_TYPE::IDLE, true, ANIM_BLEND_RATE);
	anim_ = ANIM_TYPE::IDLE;
}

void EventPlayer::ChangeStateWalk(void)
{
	animationController_->Play((int)ANIM_TYPE::WALK, true, ANIM_BLEND_RATE);
	anim_ = ANIM_TYPE::WALK;
}

void EventPlayer::ChangeStateLook(void)
{
	StopSoundMem(sHandle_);
	animationController_->Play((int)ANIM_TYPE::LOOK, true, ANIM_BLEND_RATE);
	anim_ = ANIM_TYPE::LOOK;
}

void EventPlayer::ChangeStateStandBy(void)
{
	animationController_->Play((int)ANIM_TYPE::STANDBY, false, ANIM_BLEND_RATE);
	anim_ = ANIM_TYPE::STANDBY;
}

void EventPlayer::UpdateNone(void)
{
	// 移動方向に応じた回転
	Rotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	// 回転させる
	transform_.quaRot = playerRotY_;
}

void EventPlayer::UpdateIdle(void)
{
	// 移動方向に応じた回転
	Rotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	// 回転させる
	transform_.quaRot = playerRotY_;
}

void EventPlayer::UpdateWalk(void)
{
	if (!isSound_)
	{
		PlaySoundMem(sHandle_, DX_PLAYTYPE_LOOP, true);
		isSound_ = true;
	}

	// ここに移動処理
	transform_.pos = VAdd(transform_.pos, VScale(AsoUtility::DIR_F, MOVE_SPEED));

	// 移動方向に応じた回転
	Rotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	// 回転させる
	transform_.quaRot = playerRotY_;
}

void EventPlayer::UpdateLook(void)
{
	// 移動方向に応じた回転
	Rotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	// 回転させる
	transform_.quaRot = playerRotY_;
}

void EventPlayer::UpdateStandBy(void)
{
	// 移動方向に応じた回転
	Rotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	// 回転させる
	transform_.quaRot = playerRotY_;
}

void EventPlayer::SetGoalRotate(double rotRad)
{
	VECTOR angles = SceneManager::GetInstance().GetCamera()->GetAngles();
	double cameraRadY = AsoUtility::Deg2RadD(angles.y);


	Quaternion axis = Quaternion::AngleAxis(cameraRadY + rotRad, AsoUtility::AXIS_Y);

	// 現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	// しきい値
	if (angleDiff > THRESHOLD_ROT)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;
}

void EventPlayer::Rotate(void)
{
	stepRotTime_ -= scnMng_.GetDeltaTime();

	// 回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);

}

void EventPlayer::Collision(void)
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

void EventPlayer::CollisionGravity(void)
{
	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	float checkPow = GRAVITY_CHECK_POW;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow * -1.0f));
	for (const auto c : colliders_)
	{

		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		// 最初は上の行のように実装して、木の上に登ってしまうことを確認する
		//if (hit.HitFlag > 0)
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

void EventPlayer::CollisionCapsule(void)
{
	// カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);

	// カプセルとの衝突判定
	for (const auto c : colliders_)
	{
		auto hits = MV1CollCheck_Capsule(
			c->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
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

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void EventPlayer::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 最初は実装しない。地面と突き抜けることを確認する。
	// 内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		jumpPow_ = gravity;
	}
}

bool EventPlayer::IsEndLanding(ANIM_TYPE anim)
{
	if (state_ == STATE::WALK)return false;
	if (animationController_->GetPlayType() != (int)anim)return false;
	return animationController_->IsEnd();
}

VECTOR EventPlayer::WeaponTopPos(VECTOR pos)
{
	return VAdd(weaponTopPos_, pos);
}

VECTOR EventPlayer::WeaponDownPos(VECTOR pos)
{
	return VAdd(weaponDownPos_, pos);
}

void EventPlayer::UpdateRightWeapon(void)
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

	// 右ベクトルと上ベクトルを簡易的に求める（完全な回転行列がない前提）
	VECTOR upVec = WEAPON_UPVEC; // 上方向
	VECTOR rightVec = VCross(upVec, dir);  // 手の向きと上から右方向を作る
	rightVec = VNorm(rightVec); // 正規化

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

void EventPlayer::UpdateWeapon()
{
	// 手首 → 手 の向きベクトル
	weaponTopPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponTopIndex_);
	weaponDownPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponDownIndex_);

	VECTOR dir = VSub(weaponTopPos_, weaponDownPos_); // 向きベクトル
	dir = VNorm(dir); // 正規化して単位ベクトルにする

	// 位置微調整
	float forwardOffset = WEAPON_CAPSULE_FORWARD_OFFSET;
	float upOffset = WEAPON_CAPSULE_UP_OFFSET;
	float rightOffset = WEAPON_CAPSULE_RIGHT_OFFSET;

	// 右ベクトルと上ベクトルを簡易的に求める（完全な回転行列がない前提）
	VECTOR upVec = WEAPON_CAPSULE_UPVEC; // 上方向
	VECTOR rightVec = VCross(upVec, dir);  // 手の向きと上から右方向を作る
	rightVec = VNorm(rightVec); // 正規化

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
