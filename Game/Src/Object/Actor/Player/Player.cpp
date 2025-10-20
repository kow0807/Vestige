#include <string>
#include <EffekseerForDXLib.h>
#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Camera.h"
#include "../../Common/AnimationController.h"
#include "../../Common/Capsule.h"
#include "../../Common/Collider.h"
#include "../Enemy/EnemyBoss/EnemyBoss.h"
#include "../Enemy/EnemyBoss/EnemyBossDummy.h"
#include "../Enemy/EnemyScarecrow.h"
#include "../Enemy/EnemyScarecrowDummy.h"
#include "../Weapon/PlayerWeapon.h"
#include "../../Item/HpPotion.h"
#include "../../Item/UltPotion.h"
#include "../../Item/Book.h"
#include "../../Stage/Planet.h"
#include "../../Stage/Mist.h"
#include "Player.h"

Player::Player(void)
	:
	animationController_(nullptr),
	state_(STATE::NONE),
	itemType_(ITEM_TYPE::HP_POTION),
	speed_(0.0f),
	moveDir_(AsoUtility::VECTOR_ZERO),
	movePow_(AsoUtility::VECTOR_ZERO),
	movedPos_(AsoUtility::VECTOR_ZERO),
	isRun_(false),
	playerRotY_(Quaternion()),
	goalQuaRot_(Quaternion()),
	stepRotTime_(0.0f),
	jumpPow_(AsoUtility::VECTOR_ZERO),
	isJump_(false),
	stepJump_(0.0f),
	attack_(0),
	attackCnt_(0.0f),
	attackCnt2_(0.0f),
	isAttack2_(false),
	attackCnt3_(0.0f),
	isAttack3_(false),
	strongAttackCnt_(0.0f),
	stunCnt_(0.0f),
	rollingCnt_(0.0f),
	deadCnt_(0.0f),
	gravHitPosDown_(AsoUtility::VECTOR_ZERO),
	gravHitPosUp_(AsoUtility::VECTOR_ZERO),
	capsule_(nullptr),
	canAttack_(false),
	currentHp_(MAX_HP),
	displayHp_(currentHp_),
	damageHp_(currentHp_),
	healHp_(0),
	stamina_(MAX_STAMINA),
	ultFrameHandle_(resMng_.Load(ResourceManager::SRC::GAUGE_FRAME).handleId_),
	magicCircleHandle_(resMng_.Load(ResourceManager::SRC::MAGIC_CIRCLE).handleId_),
	runeHandle_(resMng_.Load(ResourceManager::SRC::RUNE).handleId_),
	ultValue_(0.0f),
	isUltActive_(false),
	wasUltMaxed_(false),
	flashTimer_(0),
	id_(0),
	lightningTimer_(0),
	isLightning_(false),
	attackSe_(resMng_.Load(ResourceManager::SRC::ATTACK_SE).handleId_),
	hitSe_(resMng_.Load(ResourceManager::SRC::HIT_SE).handleId_),
	rollingSe_(resMng_.Load(ResourceManager::SRC::ROLLING_SE).handleId_),
	ultSe_(resMng_.Load(ResourceManager::SRC::ULT_SE).handleId_),
	damageSe_(resMng_.Load(ResourceManager::SRC::DAMAGE_SE).handleId_),
	wristIndex_(-1),
	handIndex_(-1),
	weaponTopIndex_(-1),
	weaponDownIndex_(-1),
	weaponTopPos_(AsoUtility::VECTOR_ZERO),
	weaponDownPos_(AsoUtility::VECTOR_ZERO),
	wristPos_(AsoUtility::VECTOR_ZERO),
	handPos_(AsoUtility::VECTOR_ZERO),
	effectHitPlayId_(-1),
	effectHitResId_(-1),
	effectUltPlayId_(-1),
	effectUltResId_(-1)
{
}

Player::~Player(void)
{
	delete animationController_;
}

void Player::Init(void)
{
	ChangeVolumeSoundMem(ATTACK_SE_VOL, attackSe_);

	ChangeVolumeSoundMem(HIT_SE_VOL, hitSe_);

	ChangeVolumeSoundMem(ROLLING_SE_VOL, rollingSe_);

	ChangeVolumeSoundMem(ULT_SE_VOL, ultSe_);

	ChangeVolumeSoundMem(DAMAGE_SE_VOL, damageSe_);

	// モデルの基本設定
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.pos = INIT_PLAYER_POS;
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ INIT_QUA_ROT_LOCAL.x, AsoUtility::Deg2RadF(INIT_QUA_ROT_LOCAL.y), INIT_QUA_ROT_LOCAL.z });
	transform_.Update();

	// アニメーションの設定
	InitAnimation();

	// カプセルコライダ
	capsule_ = std::make_shared<Capsule>(transform_);
	capsule_->SetLocalPosTop(INIT_TOP_POS);
	capsule_->SetLocalPosDown(INIT_DOWN_POS);
	capsule_->SetRadius(PLAYER_RADIUS);

	weapon_ = std::make_shared<PlayerWeapon>();
	weapon_->Init();

	hpPotion_ = std::make_unique<HpPotion>();
	hpPotion_->Init();

	ultPotion_ = std::make_unique<UltPotion>();
	ultPotion_->Init();

	book_ = std::make_unique<Book>();
	book_->Init();

	wristIndex_ = MV1SearchFrame(transform_.modelId, "mixamorig:RightHand");
	handIndex_ = MV1SearchFrame(transform_.modelId, "mixamorig:RightHandMiddle1");
	weaponTopIndex_ = WEAPON_TOP_INDEX;
	weaponDownIndex_ = WEAPON_DOWN_INDEX;

	weaponTopPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponTopIndex_);
	weaponDownPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponDownIndex_);
	weaponTopPos_ = WeaponTopPos(INIT_WEAPON_TOP_POS);

	UpdateRightWeapon();
	UpdateWeaponCapsule();

	// 初期状態
	ChangeState(STATE::PLAY);
	SceneManager::GetInstance().GetCamera()->ChangeGameCamera(Camera::GAME_CAMERA::MOUSE);

	InitEffect();
}

void Player::Update(void)
{
	// 更新ステップ
	switch (state_)
	{
	case Player::STATE::NONE:
		UpdateNone();
		break;
	case Player::STATE::PLAY:
		if (currentHp_ > 0)
		{
			UpdatePlay();
		}
		else
		{
			SceneManager::GetInstance().GetCamera()->ChangeGameCamera(Camera::GAME_CAMERA::NONE);
			deadCnt_++;
			animationController_->Play((int)ANIM_TYPE::DEAD, false);
		}

		// ダメージ時の薄いゲージをゆっくり減らす
		if (damageHp_ > currentHp_) {
			damageHp_ -= HP_DECREASE_SPEED;
			if (damageHp_ < currentHp_) damageHp_ = currentHp_;
		}
		// メインゲージは回復時にゆっくり増える
		if (currentHp_ < healHp_) {
			currentHp_ += HP_DECREASE_SPEED;
			if (currentHp_ >= healHp_)
			{
				currentHp_ = healHp_;
				healHp_ = currentHp_;
				damageHp_ = currentHp_;
			}
		}
		break;
	}

	// モデル制御更新
	transform_.Update();

	attackCnt_--;
	attackCnt2_--;
	attackCnt3_--;
	strongAttackCnt_--;
	stunCnt_--;
	rollingCnt_--;
	lightningTimer_--;

	// アニメーション再生
	animationController_->Update();

	UpdateRightWeapon();
	UpdateWeaponCapsule();
	UpdateUlt();
}

void Player::Draw(void)
{
	weapon_->Draw();

	// モデルの描画
	MV1DrawModel(transform_.modelId);

}

void Player::AddCollider(std::shared_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void Player::ClearCollider(void)
{
	colliders_.clear();
}

const std::shared_ptr<Capsule> Player::GetCapsule(void) const
{
	return capsule_;
}

void Player::SetEnemy(std::shared_ptr<EnemyBoss> enemy)
{
	enemy_ = enemy;
}

void Player::SetEnemyDummy(std::shared_ptr<EnemyBossDummy> enemyDummy)
{
	enemyDummy_ = enemyDummy;
}

void Player::SetEnemyScarecrow(std::shared_ptr<EnemyScarecrow> enemyScarecrow)
{
	enemyScarecrow_ = enemyScarecrow;
}

void Player::SetEnemyScarecrowDummy(std::shared_ptr<EnemyScarecrowDummy> enemyScarecrowDummy)
{
	enemyScarecrowDummy_ = enemyScarecrowDummy;
}

void Player::SetMist(std::shared_ptr<Mist> mist)
{
	mist_ = mist;
}

void Player::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = new AnimationController(transform_.modelId);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", IDLE_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", RUN_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", FAST_RUN_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", JUMP_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", WARP_PAUSE_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", FLY_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", FALLING_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", VICTORY_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::ATTACK, path + "Attack.mv1", ATTACK_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::ATTACK2, path + "Attack2.mv1", ATTACK2_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::ATTACK3, path + "Attack3.mv1", ATTACK3_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::STRONG_ATTACK, path + "StrongAttack.mv1", STRONG_ATTACK_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::STUN, path + "Stun.mv1", STUN_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::ROLLING, path + "Rolling.mv1", ROLLING_ANIM_SPEED);
	animationController_->Add((int)ANIM_TYPE::DEAD, path + "Dead.mv1", DEAD_ANIM_SPEED);

	animationController_->Play((int)ANIM_TYPE::IDLE);

}

void Player::ChangeState(STATE state)
{

	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	switch (state_)
	{
	case Player::STATE::NONE:
		ChangeStateNone();
		break;
	case Player::STATE::PLAY:
		ChangeStatePlay();
		break;
	}

}

void Player::ChangeStateNone(void)
{
}

void Player::ChangeStatePlay(void)
{
}

void Player::ChangeItem(ITEM_TYPE itemType)
{
	// 状態変更
	itemType_ = itemType;

	// 各状態遷移の初期処理
	switch (itemType_)
	{
	case Player::ITEM_TYPE::HP_POTION:
		break;
	case Player::ITEM_TYPE::ULT_POTION:
		break;
	}
}

void Player::UpdateNone(void)
{
}

void Player::UpdatePlay(void)
{
	isRun_ = false;
	// 移動処理
	ProcessMove();

	if (attackCnt_ > 0)
	{
		animationController_->Play((int)ANIM_TYPE::ATTACK);
		if (!isUltActive_)
		{
			attack_ = NORMAL_DAMAGE;
		}
		else
		{
			attack_ = SPECIAL_NORMAL_DAMAGE;
		}
		if (attackCnt_ == ATTACK_SE_TIMING)
		{
			PlaySoundMem(attackSe_, DX_PLAYTYPE_BACK, true);
		}
	}


	if (attackCnt_ <= 0 && attackCnt2_ > 0)
	{
		animationController_->Play((int)ANIM_TYPE::ATTACK2);
		if (!isUltActive_)
		{
			attack_ = NORMAL_DAMAGE;
		}
		else
		{
			attack_ = SPECIAL_NORMAL_DAMAGE;
		}
		if (attackCnt2_ == ATTACK2_SE_TIMING)
		{
			PlaySoundMem(attackSe_, DX_PLAYTYPE_BACK, true);
		}
	}


	if (attackCnt_ <= 0 && attackCnt2_ <= 0 && attackCnt3_ > 0)
	{
		animationController_->Play((int)ANIM_TYPE::ATTACK3);
		if (!isUltActive_)
		{
			attack_ = NORMAL_DAMAGE;
		}
		else
		{
			attack_ = SPECIAL_NORMAL_DAMAGE;
		}
		if (attackCnt3_ == ATTACK3_SE_TIMING)
		{
			PlaySoundMem(attackSe_, DX_PLAYTYPE_BACK, true);
		}
	}


	if (strongAttackCnt_ > 0)
	{
		animationController_->Play((int)ANIM_TYPE::STRONG_ATTACK);
		if (!isUltActive_)
		{
			attack_ = STRONG_DAMAGE;
		}
		else
		{
			attack_ = SPECIAL_STRONG_DAMAGE;
		}
		if (strongAttackCnt_ == STRONG_ATTACK_SE_TIMING)
		{
			PlaySoundMem(attackSe_, DX_PLAYTYPE_BACK, true);
		}
	}

	if ((attackCnt_ <= ATTACK_DISABLE_TIME && attackCnt2_ <= ATTACK2_DISABLE_TIME && attackCnt3_ <= ATTACK3_DISABLE_TIME && strongAttackCnt_ <= STRONG_ATTACK_DISABLE_TIME) || stunCnt_ > 0.0f)
	{
		canAttack_ = false;
	}

	if (stunCnt_ > 0.0f)
	{
		animationController_->Play((int)ANIM_TYPE::STUN);
	}
	if (rollingCnt_ > 0)
	{
		animationController_->Play((int)ANIM_TYPE::ROLLING);
		movePow_ = VScale(transform_.GetForward(), ROLLING_MOVE);
		if (rollingCnt_ == ROLLING_SE_TIMING)
		{
			PlaySoundMem(rollingSe_, DX_PLAYTYPE_BACK, true);
		}
	}


	// 移動方向に応じた回転
	Rotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	// 回転させる
	transform_.quaRot = playerRotY_;

	// アイテム
	UpdateItem();
}


void Player::ProcessMove(void)
{

	auto& ins = InputManager::GetInstance();

	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	// 移動量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotOutX();

	// 回転したい角度
	double rotRad = 0;

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (attackCnt_ <= 0 && attackCnt2_ <= 0 && attackCnt3_ <= 0 && strongAttackCnt_ <= 0 && stunCnt_ <= 0 && rollingCnt_ <= 0)
	{
		dir = AsoUtility::VECTOR_ZERO;

		if (ins.IsNew(KEY_INPUT_W) || input.Y < -PAD_INPUT_THRESHOLD_XY)
		{
			dir = VAdd(dir, cameraRot.GetForward());
		}
		if (ins.IsNew(KEY_INPUT_S) || input.Y > PAD_INPUT_THRESHOLD_XY)
		{
			dir = VAdd(dir, cameraRot.GetBack());
		}
		if (ins.IsNew(KEY_INPUT_D) || input.X > PAD_INPUT_THRESHOLD_XY)
		{
			dir = VAdd(dir, cameraRot.GetRight());
		}
		if (ins.IsNew(KEY_INPUT_A) || input.X < -PAD_INPUT_THRESHOLD_XY)
		{
			dir = VAdd(dir, cameraRot.GetLeft());
		}

		if (!AsoUtility::EqualsVZero(dir))
		{
			dir = VNorm(dir);
			rotRad = std::atan2(dir.x, dir.z);
		}
	}

	VECTOR angles = SceneManager::GetInstance().GetCamera()->GetAngles();

	Quaternion rots = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f), AsoUtility::Deg2RadF(angles.y), AsoUtility::Deg2RadF(0.0f));

	VECTOR direction = rots.PosAxis(moveDir_);

	if (!AsoUtility::EqualsVZero(dir) && (isJump_ || IsEndLanding())) {

		// 移動処理
		speed_ = SPEED_MOVE;
		if ((ins.IsNew(KEY_INPUT_LSHIFT) || ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) && stamina_ > 0)
		{
			speed_ = SPEED_RUN;
			isRun_ = true;
		}

		moveDir_ = dir;
		movePow_ = VScale(direction, speed_);

		// 回転処理
		SetGoalRotate(rotRad);


		if (!isJump_ && IsEndLanding())
		{
			if (attackCnt_ <= 0 && attackCnt2_ <= 0 && attackCnt3_ <= 0 && strongAttackCnt_ <= 0 && stunCnt_ <= 0 && rollingCnt_ <= 0)
			{
				if ((ins.IsTrgMouseRight() || input.Z < PAD_INPUT_THRESHOLD_Z) && stamina_ > STRONG_ATTACK_STAMINA)
				{
					strongAttackCnt_ = MAX_STRONG_ATTACK_CNT;
					canAttack_ = true;
					stamina_ -= STRONG_ATTACK_STAMINA;
				}

				if ((ins.IsTrgMouseLeft() || input.Buttons[ATTACK_PAD_BUTTON_NUM]) && strongAttackCnt_ < 0)
				{
					attackCnt_ = MAX_ATTACK_CNT;
					canAttack_ = true;
				}

				// アニメーション
				if (ins.IsNew(KEY_INPUT_LSHIFT) || ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
				{
					if (stamina_ > 0)
					{
						animationController_->Play((int)ANIM_TYPE::FAST_RUN);
						stamina_ -= STAMINA_DRAIN_RATE;
					}
					else
					{
						animationController_->Play((int)ANIM_TYPE::RUN);
					}
				}
				else
				{
					animationController_->Play((int)ANIM_TYPE::RUN);
					if (stamina_ < MAX_STAMINA)
					{

						stamina_ += STAMINA_RECOVERY_RATE;
					}
				}
				if ((ins.IsTrgDown(KEY_INPUT_SPACE) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT)) && stamina_ > ROLLING_STAMINA)
				{

					rollingCnt_ = MAX_ROLLING_CNT;
					stamina_ -= ROLLING_STAMINA;

				}
			}
		}

	}
	else
	{
		if (attackCnt_ <= 0 && attackCnt2_ <= 0 && attackCnt3_ <= 0 && strongAttackCnt_ <= 0 && stunCnt_ <= 0 && rollingCnt_ <= 0)
		{
			if ((ins.IsTrgMouseRight() || input.Z < PAD_INPUT_THRESHOLD_Z) && stamina_ > STRONG_ATTACK_STAMINA)
			{
				strongAttackCnt_ = MAX_STRONG_ATTACK_CNT;
				canAttack_ = true;
				stamina_ -= STRONG_ATTACK_STAMINA;
			}

			if ((ins.IsTrgMouseLeft() || input.Buttons[ATTACK_PAD_BUTTON_NUM]) && strongAttackCnt_ < 0)
			{
				attackCnt_ = MAX_ATTACK_CNT;
				canAttack_ = true;
			}

			if (!isJump_ && IsEndLanding())
			{
				animationController_->Play((int)ANIM_TYPE::IDLE);
			}

			if ((ins.IsTrgDown(KEY_INPUT_SPACE) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT)) && stamina_ > ROLLING_STAMINA)
			{
				rollingCnt_ = MAX_ROLLING_CNT;
				stamina_ -= ROLLING_STAMINA;
			}

			if (stamina_ < MAX_STAMINA)
			{
				stamina_ += STAMINA_RECOVERY_RATE;
			}
		}
	}
	if (attackCnt_ < ATTACK_DISABLE_TIME && attackCnt_ > 0.0f)
	{
		if (ins.IsTrgMouseLeft() || input.Buttons[ATTACK_PAD_BUTTON_NUM])
		{
			attackCnt2_ = MAX_ATTACK_CNT_2;
		}
	}
	if (attackCnt_ == 0.0f && attackCnt2_ > 0.0f)
	{
		isAttack2_ = true;
	}
	if (isAttack2_)
	{
		attackCnt2_ = MAX_ATTACK_CNT_2;
		isAttack2_ = false;
		canAttack_ = true;
	}


	if (attackCnt2_ < ATTACK2_DISABLE_TIME && attackCnt2_ > 0.0f)
	{
		if (ins.IsTrgMouseLeft() || input.Buttons[ATTACK_PAD_BUTTON_NUM])
		{
			attackCnt3_ = MAX_ATTACK_CNT_3;
		}

	}
	if (attackCnt2_ == 0.0f && attackCnt3_ > 0.0f)
	{
		isAttack3_ = true;
	}
	if (isAttack3_)
	{
		attackCnt3_ = MAX_ATTACK_CNT_3;
		isAttack3_ = false;
		canAttack_ = true;
	}
}


void Player::SetGoalRotate(double rotRad)
{

	VECTOR angles = SceneManager::GetInstance().GetCamera()->GetAngles();
	double cameraRadY = AsoUtility::Deg2RadD(angles.y);


	Quaternion axis = Quaternion::AngleAxis(cameraRadY + rotRad, AsoUtility::AXIS_Y);

	// 現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	// しきい値
	if (angleDiff > ROTATION_THRESHOLD_RAD)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;
}

void Player::Rotate(void)
{

	stepRotTime_ -= scnMng_.GetDeltaTime();

	// 回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);

}

void Player::Collision(void)
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

void Player::CollisionGravity(void)
{

	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, GROUND_CHECK_UP_OFFSET));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, -GROUND_CHECK_DISTANCE));
	for (const auto c : colliders_)
	{

		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		// 最初は上の行のように実装して、木の上に登ってしまうことを確認する
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > GRAVITY_COLLISION_DOT_THRESHOLD)
		{

			// 衝突地点から、少し上に移動
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, LANDING_ADJUST_HEIGHT));

			// ジャンプリセット
			jumpPow_ = AsoUtility::VECTOR_ZERO;
			stepJump_ = 0.0f;

			if (isJump_)
			{
				// 着地モーション
				animationController_->Play(
					(int)ANIM_TYPE::JUMP, false, LANDING_ANIM_START_FRAME, LANDING_ANIM_END_FRAME, false, true);
			}

			isJump_ = false;

		}

	}

}

void Player::CollisionCapsule(void)
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

			for (int tryCnt = 0; tryCnt < MAX_COLLISION_ADJUST_COUNT; tryCnt++)
			{

				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);

				if (pHit)
				{
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, COLLISION_PUSHBACK_AMOUNT));
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

void Player::CollisionEnemyBoss(void)
{
	MV1RefreshCollInfo(enemyDummy_->GetTransform().modelId);

	// カプセルとの衝突判定
	auto hits = MV1CollCheck_Capsule(
		enemyDummy_->GetTransform().modelId, -1,
		capsule_->GetPosTop(), capsule_->GetPosDown(), PLAYER_RADIUS);

	// 衝突した複数のポリゴンと衝突回避するまで、
	// プレイヤーの位置を移動させる
	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hit = hits.Dim[i];
		// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
		// この時、移動させる方向は、移動前座標に向いた方向であったり、
		// 衝突したポリゴンの法線方向だったりする
		for (int tryCnt = 0; tryCnt < MAX_COLLISION_RESOLVE_ATTEMPTS; tryCnt++)
		{
			// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
			// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
			int pHit = HitCheck_Capsule_Triangle(
				capsule_->GetPosTop(), capsule_->GetPosDown(), PLAYER_RADIUS,
				hit.Position[0], hit.Position[1], hit.Position[2]);
			if (pHit)
			{
				// 法線の方向にちょっとだけ移動させる
				movedPos_ = VAdd(movedPos_, VScale(hit.Normal, COLLISION_PUSHBACK_AMOUNT));
				//// カプセルも一緒に移動させる
				//transform_.topPos.x = movedPos_.x;
				//transform_.topPos.z = movedPos_.z;
				//transform_.downPos.x = movedPos_.x;
				//transform_.downPos.z = movedPos_.z;
				transform_.pos = movedPos_;
				transform_.Update();
				continue;
			}
			break;
		}
	}
	// 検出したポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
}

void Player::CalcGravityPow(void)
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

bool Player::IsEndLanding(void)
{

	bool ret = true;

	// アニメーションがジャンプではない
	if (animationController_->GetPlayType() != (int)ANIM_TYPE::JUMP)
	{
		return ret;
	}

	// アニメーションが終了しているか
	if (animationController_->IsEnd())
	{
		return ret;
	}

	return false;

}

void Player::UpdateRightWeapon(void)
{

	// 手首 → 手 の向きベクトル
	wristPos_ = MV1GetFramePosition(transform_.modelId, wristIndex_);
	handPos_ = MV1GetFramePosition(transform_.modelId, handIndex_);

	VECTOR dir = VSub(handPos_, wristPos_); // 向きベクトル
	dir = VNorm(dir); // 正規化して単位ベクトルにする

	// 向きから角度を算出
	float yaw = atan2f(dir.x, dir.z);
	float pitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
	float roll = 0.0f;

	// 剣の回転を設定
	MV1SetRotationXYZ(weapon_->GetModelHandle(), VGet(pitch, yaw, roll));

	// 位置微調整
	float forwardOffset = FORWARD_OFFSET;
	float upOffset = UP_OFFSET;
	float rightOffset = RIGHT_OFFSET;

	// 右ベクトルと上ベクトルを簡易的に求める（完全な回転行列がない前提）
	VECTOR upVec = AsoUtility::DIR_U; // 上方向
	VECTOR rightVec = VCross(upVec, dir);  // 手の向きと上から右方向を作る
	rightVec = VNorm(rightVec); // 正規化

	// 上方向再調整（手のひねりは考慮しない）
	upVec = VCross(dir, rightVec);
	upVec = VNorm(upVec);

	// オフセット合成
	VECTOR offsetPos = handPos_;
	offsetPos = VAdd(offsetPos, VScale(dir, forwardOffset));   // 前方
	offsetPos = VAdd(offsetPos, VScale(upVec, upOffset));      // 上
	offsetPos = VAdd(offsetPos, VScale(rightVec, rightOffset));// 右

	// 剣の位置を設定
	MV1SetPosition(weapon_->GetModelHandle(), offsetPos);
}

void Player::DrawHp(float hp, int maxHp)
{
	// 色設定：原色ではなく中間色を使用して落ち着いた印象に
	int hpColorTop = GetColor(HP_COLOR_TOP_R, HP_COLOR_TOP_G, HP_COLOR_TOP_B);  // 現在HPの上側（少し明るめ）
	int hpColorBottom = GetColor(HP_COLOR_BOTTOM_R, HP_COLOR_BOTTOM_G, HP_COLOR_BOTTOM_B);  // 現在HPの下側（落ち着いた緑）
	int damageColor = GetColor(DAMAGE_COLOR_R, DAMAGE_COLOR_G, DAMAGE_COLOR_B);  // ダメージ分（やや濃い赤）
	int healColor = GetColor(HEAL_COLOR_R, HEAL_COLOR_G, HEAL_COLOR_B); // 回復分（中間緑）
	int borderColor = GetColor(HP_OUTER_BORDER_COLOR_R, HP_OUTER_BORDER_COLOR_G, HP_OUTER_BORDER_COLOR_B);   // 外枠（ダークグレー）

	// 背景と枠（2重に描いてやや立体的に見せる）
	DrawBox(HP_POS_X - OUTER_BORDER_OFFSET, HP_POS_Y - OUTER_BORDER_OFFSET, HP_POS_X + HP_SIZE_X + OUTER_BORDER_OFFSET, HP_POS_Y + HP_SIZE_Y + OUTER_BORDER_OFFSET, borderColor, TRUE);     // 外枠（暗）
	DrawBox(HP_POS_X - INNER_BORDER_OFFSET, HP_POS_Y - INNER_BORDER_OFFSET, HP_POS_X + HP_SIZE_X + INNER_BORDER_OFFSET, HP_POS_Y + HP_SIZE_Y + INNER_BORDER_OFFSET, GetColor(HP_INNER_BORDER_COLOR_R, HP_INNER_BORDER_COLOR_G, HP_INNER_BORDER_COLOR_B), TRUE); // 内枠（明）

	// 背景（HPがゼロのときのベース）
	DrawBox(HP_POS_X, HP_POS_Y, HP_POS_X + HP_SIZE_X, HP_POS_Y + HP_SIZE_Y, GetColor(HP_BG_COLOR_R, HP_BG_COLOR_G, HP_BG_COLOR_B), TRUE);

	// ダメージ分
	int damageW = static_cast<int>(HP_SIZE_X * damageHp_ / maxHp);
	if (damageW > 0)
		DrawBox(HP_POS_X, HP_POS_Y, HP_POS_X + damageW, HP_POS_Y + HP_SIZE_Y, damageColor, TRUE);

	// 回復分
	int healW = static_cast<int>(HP_SIZE_X * healHp_ / maxHp);
	if (healW > 0)
		DrawBox(HP_POS_X, HP_POS_Y, HP_POS_X + healW, HP_POS_Y + HP_SIZE_Y, healColor, TRUE);

	// 現在HP（グラデーション風：上と下で色を変える）
	int hpW = static_cast<int>(HP_SIZE_X * currentHp_ / maxHp);
	if (hpW > 0)
	{
		int halfH = HP_SIZE_Y / 2;
		DrawBox(HP_POS_X, HP_POS_Y, HP_POS_X + hpW, HP_POS_Y + halfH, hpColorTop, TRUE);      // 上半分
		DrawBox(HP_POS_X, HP_POS_Y + halfH, HP_POS_X + hpW, HP_POS_Y + HP_SIZE_Y, hpColorBottom, TRUE);  // 下半分
	}
}

void Player::DrawStamina(float stamina, int staminaMax)
{
	// 落ち着いた黄色系の色を用意（上下で微妙に違う色でグラデ風）
	int colorTop = GetColor(STAMINA_COLOR_TOP_R, STAMINA_COLOR_TOP_G, STAMINA_COLOR_TOP_B);    // 明るめの黄色
	int colorBottom = GetColor(STAMINA_COLOR_BOTTOM_R, STAMINA_COLOR_BOTTOM_G, STAMINA_COLOR_BOTTOM_B); // 少し暗めの黄色

	int borderDark = GetColor(STAMINA_OUTER_BORDER_COLOR_R, STAMINA_OUTER_BORDER_COLOR_G, STAMINA_OUTER_BORDER_COLOR_B);    // 枠の暗い色
	int borderLight = GetColor(STAMINA_INNER_BORDER_COLOR_R, STAMINA_INNER_BORDER_COLOR_G, STAMINA_INNER_BORDER_COLOR_B); // 枠の明るい色

	int barWidth = STAMINA_SIZE_X;
	int barHeight = STAMINA_SIZE_Y;

	// 外枠（濃いめ）
	DrawBox(STAMINA_POS_X - OUTER_BORDER_OFFSET, STAMINA_POS_Y - OUTER_BORDER_OFFSET, STAMINA_POS_X + barWidth + OUTER_BORDER_OFFSET, STAMINA_POS_Y + barHeight + OUTER_BORDER_OFFSET, borderDark, TRUE);

	// 内枠（少し明るい）
	DrawBox(STAMINA_POS_X - INNER_BORDER_OFFSET, STAMINA_POS_Y - INNER_BORDER_OFFSET, STAMINA_POS_X + barWidth + INNER_BORDER_OFFSET, STAMINA_POS_Y + barHeight + INNER_BORDER_OFFSET, borderLight, TRUE);

	// 背景（暗め）
	DrawBox(STAMINA_POS_X, STAMINA_POS_Y, STAMINA_POS_X + barWidth, STAMINA_POS_Y + barHeight, GetColor(STAMINA_BG_COLOR_R, STAMINA_BG_COLOR_G, STAMINA_BG_COLOR_B), TRUE);

	// スタミナゲージの幅を計算
	int staminaW = static_cast<int>(barWidth * stamina / staminaMax);
	if (staminaW > 0)
	{
		int halfH = barHeight / 2;
		// 上半分：明るめ黄色
		DrawBox(STAMINA_POS_X, STAMINA_POS_Y, STAMINA_POS_X + staminaW, STAMINA_POS_Y + halfH, colorTop, TRUE);
		// 下半分：暗め黄色
		DrawBox(STAMINA_POS_X, STAMINA_POS_Y + halfH, STAMINA_POS_X + staminaW, STAMINA_POS_Y + barHeight, colorBottom, TRUE);
	}
}

void Player::UpdateItem(void)
{
	auto& ins = InputManager::GetInstance();
	DINPUT_JOYSTATE input;
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	static int prevPOV = INVALID_POV;
	int nowPOV = input.POV[0];

	bool isPOVRightTriggered = (nowPOV == POV_RIGHT && prevPOV != POV_RIGHT);
	bool isPOVLeftTriggered = (nowPOV == POV_LEFT && prevPOV != POV_LEFT);
	prevPOV = nowPOV;

	static int wheelCounter = 0;
	int wheelDelta = ins.GetWheelDelta();
	wheelCounter += wheelDelta;

	// 次のアイテムへ
	if (wheelCounter >= WHEEL_THRESHOLD || isPOVRightTriggered)
	{
		itemType_ = static_cast<ITEM_TYPE>((static_cast<int>(itemType_) + 1) % static_cast<int>(ITEM_TYPE::NUM));
		wheelCounter = 0;
	}
	// 前のアイテムへ
	else if (wheelCounter <= -WHEEL_THRESHOLD || isPOVLeftTriggered)
	{
		itemType_ = static_cast<ITEM_TYPE>((static_cast<int>(itemType_) - 1 + static_cast<int>(ITEM_TYPE::NUM)) % static_cast<int>(ITEM_TYPE::NUM));
		wheelCounter = 0;
	}

	// アイテム使用
	switch (itemType_)
	{
	case ITEM_TYPE::HP_POTION:
		UseHpPosion();
		break;
	case ITEM_TYPE::ULT_POTION:
		UseUltPosion();
		break;
	case ITEM_TYPE::BOOK:
		UseBook();
		break;
	}

	isLightning_ = false;
	if (lightningTimer_ == LIGHTNING_UPDATE_FRAME)
	{
		if (SceneManager::GetInstance().GetSceneID() == SceneManager::SCENE_ID::TUTORIAL)
		{
			book_->LightningUpdate(enemyScarecrow_->GetTransform().pos);
		}
		else if (SceneManager::GetInstance().GetSceneID() == SceneManager::SCENE_ID::GAME)
		{
			book_->LightningUpdate(enemy_->GetTransform().pos);
		}
	}
	if (lightningTimer_ == LIGHTNING_START_FRAME)
	{
		isLightning_ = true;
	}
}

void Player::DrawItem(void)
{
	switch (itemType_)
	{
	case ITEM_TYPE::HP_POTION:
		hpPotion_->Draw();
		break;
	case ITEM_TYPE::ULT_POTION:
		ultPotion_->Draw();
		break;
	case ITEM_TYPE::BOOK:
		book_->Draw();
		break;
	}
}

void Player::UseHpPosion(void)
{
	auto& ins = InputManager::GetInstance();

	if ((ins.IsTrgDown(KEY_INPUT_R)
		|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT))
		&& hpPotion_->GetCount() > 0 && healHp_ < MAX_HP
		&& currentHp_ < MAX_HP
		&& stunCnt_ < 0.0f)
	{
		hpPotion_->Use();
		healHp_ = healHp_ + HP_POTION_HEAL_AMOUNT;
	}
	if (currentHp_ > MAX_HP)
	{
		currentHp_ = MAX_HP;
	}
	if (healHp_ > MAX_HP)
	{
		healHp_ = MAX_HP;
	}
}

void Player::UseUltPosion(void)
{
	auto& ins = InputManager::GetInstance();

	if ((ins.IsTrgDown(KEY_INPUT_R)
		|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT))
		&& ultPotion_->GetCount() > 0
		&& ultValue_ < ULT_MAX_VALUE
		&& isUltActive_ == false
		&& stunCnt_ < 0.0f)
	{
		ultPotion_->Use();
		ultValue_ += ULT_MAX_VALUE;
	}
}

void Player::UseBook(void)
{
	auto& ins = InputManager::GetInstance();

	if ((ins.IsTrgDown(KEY_INPUT_R)
		|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT))
		&& book_->GetCount() > 0
		&& stunCnt_ < 0.0f)
	{
		book_->Use();
		lightningTimer_ = LIGHTNING_DURATION_FRAME;
	}
}

VECTOR Player::WeaponTopPos(VECTOR pos)
{
	return VAdd(weaponTopPos_, pos);
}

VECTOR Player::WeaponDownPos(VECTOR pos)
{
	return VAdd(weaponDownPos_, pos);
}


void Player::UpdateWeaponCapsule()
{
	// 手首 → 手 の向きベクトル
	weaponTopPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponTopIndex_);
	weaponDownPos_ = MV1GetFramePosition(weapon_->GetModelHandle(), weaponDownIndex_);

	VECTOR dir = VSub(weaponTopPos_, weaponDownPos_); // 向きベクトル
	dir = VNorm(dir); // 正規化して単位ベクトルにする

	// 右ベクトルと上ベクトルを簡易的に求める（完全な回転行列がない前提）
	VECTOR upVec = AsoUtility::DIR_U; // 上方向
	VECTOR rightVec = VCross(upVec, dir);  // 手の向きと上から右方向を作る
	rightVec = VNorm(rightVec); // 正規化

	// 上方向再調整（手のひねりは考慮しない）
	upVec = VCross(dir, rightVec);
	upVec = VNorm(upVec);

	// オフセット合成
	VECTOR offsetPos = handPos_;
	offsetPos = VAdd(offsetPos, VScale(dir, FORWARD_OFFSET));   // 前方
	offsetPos = VAdd(offsetPos, VScale(upVec, UP_OFFSET));      // 上
	offsetPos = VAdd(offsetPos, VScale(rightVec, RIGHT_OFFSET));// 右

	// 剣の位置を設定
	MV1SetPosition(weapon_->GetModelHandle(), offsetPos);
}

void Player::InitEffect(void)
{
	// ヒットエフェクト
	effectHitResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::HIT_EFFECT).handleId_;

	// ULTエフェクト
	effectUltResId_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::ULT_EFFECT).handleId_;
}

void Player::UpdateUlt(void)
{
	auto& ins = InputManager::GetInstance();
	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	if (!isUltActive_) {
		// 通常時はゲージ増加
		ultValue_ += SceneManager::GetInstance().GetDeltaTime() * ULT_CHARGE_SPEED;

		if (ultValue_ > ULT_MAX_VALUE) ultValue_ = ULT_MAX_VALUE;

		// ゲージが満タンになったらフラグを立てる
		if (ultValue_ >= ULT_MAX_VALUE) {
			wasUltMaxed_ = true;
		}

		// 発動開始
		if (ultValue_ >= ULT_MAX_VALUE && currentHp_ > 0 && (ins.IsTrgDown(KEY_INPUT_Q) || input.Buttons[4])) {
			if (input.Buttons[4]) {
				// パッド振動（例：強めに0.5秒振動）
				StartJoypadVibration(DX_INPUT_PAD1, PAD_VIBRATION_POWER, PAD_VIBRATION_DURATION_MS);
			}
			isUltActive_ = true;
			flashTimer_ = 0;
			PlaySoundMem(ultSe_, DX_PLAYTYPE_LOOP, true);
			// エフェクト
			effectUltPlayId_ = PlayEffekseer3DEffect(effectUltResId_);
			// 大きさ
			SetScalePlayingEffekseer3DEffect(effectUltPlayId_, ULT_EFFECT_SCALE, ULT_EFFECT_SCALE, ULT_EFFECT_SCALE);
			SetPosPlayingEffekseer3DEffect(effectUltPlayId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);

		}
	}
	else {
		// 発動中はゲージを減らす
		ultValue_ -= SceneManager::GetInstance().GetDeltaTime() * ULT_DEPLETION_SPEED;
		SetPosPlayingEffekseer3DEffect(effectUltPlayId_, transform_.pos.x, transform_.pos.y, transform_.pos.z);
		if (ultValue_ < 0.0f) {
			ultValue_ = 0.0f;
			isUltActive_ = false;
			wasUltMaxed_ = false;  // ゲージが0になったのでリセット
			StopSoundMem(ultSe_);
		}
	}
}

void Player::DrawUlt()
{
	auto& ins = InputManager::GetInstance();

	// 0. 魔法円（常にゆっくり回転）
	float angle = fmod(GetNowCount() * MAGIC_CIRCLE_ROTATE_SPEED, FULL_CIRCLE_DEG);
	float rotationRad = angle * AsoUtility::DEG2RAD;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, MAX_ALPHA);
	DrawRotaGraph(ULT_CENTER_X, ULT_CENTER_Y, ULT_DRAW_SCALE, rotationRad, magicCircleHandle_, TRUE);

	// 1. 魔法円の明るさ調整（点滅をやめて少し明るく）
	if (wasUltMaxed_) {
		SetDrawBlendMode(DX_BLENDMODE_ADD, ULT_BRIGHTEN_ALPHA);
		DrawRotaGraph(ULT_CENTER_X, ULT_CENTER_Y, ULT_DRAW_SCALE, rotationRad, magicCircleHandle_, TRUE);
	}
	else if (ultValue_ > 0.0f) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ULT_SOFT_PULSE_ALPHA);
		DrawRotaGraph(ULT_CENTER_X, ULT_CENTER_Y, ULT_DRAW_SCALE, rotationRad, magicCircleHandle_, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 2. 黒いマスク（ultValue_が増えるにつれて反時計回りに消える、開始位置は上(90度)に変更）
	float angleMax = FULL_CIRCLE_DEG * (ULT_MAX_VALUE - ultValue_);

	int maskColor = GetColor(0, 0, 0);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, MAX_ALPHA);
	for (int i = 0; i < ULT_SEGMENT_COUNT; ++i) {
		float t1 = (float)i / ULT_SEGMENT_COUNT;
		float t2 = (float)(i + 1) / ULT_SEGMENT_COUNT;

		float angle1 = fmod(FULL_CIRCLE_DEG - angleMax * t1 + ULT_START_ANGLE, FULL_CIRCLE_DEG);
		float angle2 = fmod(FULL_CIRCLE_DEG - angleMax * t2 + ULT_START_ANGLE, FULL_CIRCLE_DEG);

		// 描画すべき角度範囲をチェック

		float radius1 = angle1 * AsoUtility::DEG2RAD;
		float radius2 = angle2 * AsoUtility::DEG2RAD;

		int x1 = ULT_CENTER_X + (int)(cos(radius1) * ULT_RADIUS);
		int y1 = ULT_CENTER_Y - (int)(sin(radius1) * ULT_RADIUS);
		int x2 = ULT_CENTER_X + (int)(cos(radius2) * ULT_RADIUS);
		int y2 = ULT_CENTER_Y - (int)(sin(radius2) * ULT_RADIUS);

		DrawTriangle(ULT_CENTER_X, ULT_CENTER_Y, x1, y1, x2, y2, maskColor, TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 3. Qキーを押したときの演出（40フレーム継続）
	if (isUltActive_ && flashTimer_ < ULT_FLASH_DURATION) {
		for (int i = 0; i < FLASH_ALPHA_DECAY; ++i) {
			int alpha = (int)(MAX_ALPHA * (ULT_MAX_VALUE - flashTimer_ / static_cast<float>(ULT_FLASH_DURATION))) - i * ULT_FLASH_DURATION;
			if (alpha < 0) alpha = 0;
			int radius = ULT_RADIUS + FLASH_BASE_RADIUS + i * FLASH_RING_SPACING + flashTimer_ * FLASH_RADIUS_GROWTH;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawCircle(ULT_CENTER_X, ULT_CENTER_Y, radius, GetColor(FLASH_COLOR_R, FLASH_COLOR_G, FLASH_COLOR_B), TRUE);
		}

		for (int i = 0; i < ULT_RAY_COUNT; ++i) {
			float rayAngle = (flashTimer_ * FLASH_ROTATE_SPEED + i * (FULL_CIRCLE_DEG / ULT_RAY_COUNT)) * AsoUtility::DEG2RAD;
			int rayLength = ULT_RADIUS + ULT_FLASH_DURATION + (flashTimer_ % FLASH_ROTATE_SPEED) * FLASH_ALPHA_DECAY;
			int x = ULT_CENTER_X + (int)(cos(rayAngle) * rayLength);
			int y = ULT_CENTER_Y - (int)(sin(rayAngle) * rayLength);
			int alpha = RAY_START_ALPHA - (flashTimer_ * RAY_ALPHA_DECAY_PER_FRAME);
			if (alpha < 0) alpha = 0;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawLine(ULT_CENTER_X, ULT_CENTER_Y, x, y, GetColor(RAY_COLOR_R, RAY_COLOR_G, RAY_COLOR_B), ULT_RAY_THICKNESS);
		}

		flashTimer_++;
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}


	// 4. 枠
	DrawRotaGraph(ULT_CENTER_X, ULT_CENTER_Y, ULT_DRAW_SCALE, 0.0f, ultFrameHandle_, TRUE);
}

void Player::TutorialUlt(void)
{
	ultValue_ = 0.0f;
	isUltActive_ = false;
	wasUltMaxed_ = false;  // ゲージが0になったのでリセット
	StopSoundMem(ultSe_);
	StopEffekseer3DEffect(effectUltPlayId_);
}

bool Player::IsHitAttackEnemyBoss(void)
{
	MV1RefreshCollInfo(enemy_->GetTransform().modelId);

	// カプセルとの衝突判定
	auto hits = MV1CollCheck_Capsule(
		enemy_->GetTransform().modelId, -1,
		weaponTopPos_, weaponDownPos_, WEAPON_RADIUS);

	bool isUseEffect = false;

	if (hits.HitNum > 0 && canAttack_)
	{
		canAttack_ = false;
		PlaySoundMem(hitSe_, DX_PLAYTYPE_BACK, true);

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto ePos = hits.Dim[i].HitPosition;
			VECTOR wCenter = VScale(VAdd(weaponTopPos_, weaponDownPos_), 0.5f);
			if (AsoUtility::IsHitSpheres(wCenter, WEAPON_RADIUS * 2, ePos, ENEMY_HIT_SPHERE_RADIUS))
			{

				// エフェクト
				effectHitPlayId_ = PlayEffekseer3DEffect(effectHitResId_);
				// 大きさ
				SetScalePlayingEffekseer3DEffect(effectHitPlayId_, EFFECT_HIT_SCALE, EFFECT_HIT_SCALE, EFFECT_HIT_SCALE);
				SetPosPlayingEffekseer3DEffect(effectHitPlayId_, hits.Dim->HitPosition.x, hits.Dim->HitPosition.y, hits.Dim->HitPosition.z);
				isUseEffect = true;
			}
		}
		if (isUseEffect == false)
		{
			// エフェクト
			effectHitPlayId_ = PlayEffekseer3DEffect(effectHitResId_);
			// 大きさ
			SetScalePlayingEffekseer3DEffect(effectHitPlayId_, EFFECT_HIT_SCALE, EFFECT_HIT_SCALE, EFFECT_HIT_SCALE);
			SetPosPlayingEffekseer3DEffect(effectHitPlayId_, weaponTopPos_.x, weaponTopPos_.y, weaponTopPos_.z);
		}
		// 検出したポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
		return true;
	}
	else
	{
		// 検出したポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
		return false;
	}
}

bool Player::IsHitAttackEnemyScarecrow(void)
{
	MV1RefreshCollInfo(enemyScarecrow_->GetTransform().modelId);

	// カプセルとの衝突判定
	auto hits = MV1CollCheck_Capsule(
		enemyScarecrow_->GetTransform().modelId, -1,
		weaponTopPos_, weaponDownPos_, WEAPON_RADIUS);

	bool isUseEffect = false;

	if (hits.HitNum > 0 && canAttack_)
	{
		canAttack_ = false;
		PlaySoundMem(hitSe_, DX_PLAYTYPE_BACK, true);

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto ePos = hits.Dim[i].HitPosition;
			VECTOR wCenter = VScale(VAdd(weaponTopPos_, weaponDownPos_), 0.5f);
			if (AsoUtility::IsHitSpheres(wCenter, WEAPON_RADIUS * 2, ePos, ENEMY_HIT_SPHERE_RADIUS))
			{
				// エフェクト
				effectHitPlayId_ = PlayEffekseer3DEffect(effectHitResId_);
				// 大きさ
				SetScalePlayingEffekseer3DEffect(effectHitPlayId_, EFFECT_HIT_SCALE, EFFECT_HIT_SCALE, EFFECT_HIT_SCALE);
				SetPosPlayingEffekseer3DEffect(effectHitPlayId_, hits.Dim->HitPosition.x, hits.Dim->HitPosition.y, hits.Dim->HitPosition.z);
				isUseEffect = true;
			}
		}
		if (isUseEffect == false)
		{
			// エフェクト
			effectHitPlayId_ = PlayEffekseer3DEffect(effectHitResId_);
			// 大きさ
			SetScalePlayingEffekseer3DEffect(effectHitPlayId_, EFFECT_HIT_SCALE, EFFECT_HIT_SCALE, EFFECT_HIT_SCALE);
			SetPosPlayingEffekseer3DEffect(effectHitPlayId_, weaponTopPos_.x, weaponTopPos_.y, weaponTopPos_.z);
		}

		// 検出したポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
		return true;
	}
	else
	{
		// 検出したポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
		return false;
	}
}

float Player::GetHp(void)
{
	return currentHp_;
}

int Player::GetStamina(void)
{
	return stamina_;
}

void Player::Damage(int damage)
{
	if (currentHp_ > 0)
	{
		// ローリングの処理
		if (rollingCnt_ <= 0)
		{
			currentHp_ -= damage;
			healHp_ = currentHp_;
			PlaySoundMem(damageSe_, DX_PLAYTYPE_BACK, true);
		}
		else
		{
			if (!isUltActive_)
			{
				ultValue_ += ULT_CHARGE_SPEED;
			}
		}

		if (currentHp_ < 0)
		{
			currentHp_ = 0;
		}
		if (healHp_ < 0)
		{
			healHp_ = 0;
		}

		if (ultValue_ > ULT_MAX_VALUE)
		{
			ultValue_ = ULT_MAX_VALUE;
		}
	}
}

bool Player::IsDead(void)
{
	if (deadCnt_ > PLAYER_DEAD_TIME)
	{
		return true;
	}
	else
	{
		return false;
	}
}

VECTOR Player::GetMovePow(void)
{
	return movePow_;
}

int Player::GetID() const
{
	return id_;
}

void Player::StunUpdate(void)
{
	if (stunCnt_ <= STUN_RECHARGE_THRESHOLD && rollingCnt_ <= 0)
	{
		stunCnt_ = STUN_DURATION;
	}
}

void Player::CollisionEnemyScarecrow(void)
{
	MV1RefreshCollInfo(enemyScarecrowDummy_->GetTransform().modelId);

	// カプセルとの衝突判定
	auto hits = MV1CollCheck_Capsule(
		enemyScarecrowDummy_->GetTransform().modelId, -1,
		capsule_->GetPosTop(), capsule_->GetPosDown(), WEAPON_RADIUS);

	// 衝突した複数のポリゴンと衝突回避するまで、プレイヤーの位置を移動させる
	for (int i = 0; i < hits.HitNum; i++)
	{
		auto hit = hits.Dim[i];
		// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
		// この時、移動させる方向は、移動前座標に向いた方向であったり、
		// 衝突したポリゴンの法線方向だったりする
		for (int tryCnt = 0; tryCnt < MAX_COLLISION_RESOLVE_ATTEMPTS; tryCnt++)
		{
			// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
			// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
			int pHit = HitCheck_Capsule_Triangle(
				capsule_->GetPosTop(), capsule_->GetPosDown(), WEAPON_RADIUS,
				hit.Position[0], hit.Position[1], hit.Position[2]);
			if (pHit)
			{
				// 法線の方向にちょっとだけ移動させる
				movedPos_ = VAdd(movedPos_, VScale(hit.Normal, COLLISION_PUSHBACK_AMOUNT));
				transform_.pos = movedPos_;
				transform_.Update();
				continue;
			}
			break;
		}
	}
	// 検出したポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
}

void Player::CollisionMist(void)
{
	for (int h = 0; h < mist_->GetMistTransforms().size(); h++) {
		MV1RefreshCollInfo(mist_->GetTransform().modelId);

		// カプセルとの衝突判定
		auto hits = MV1CollCheck_Capsule(
			mist_->GetMistTransforms()[h].modelId, -1,
			capsule_->GetPosTop(), capsule_->GetPosDown(), WEAPON_RADIUS);

		// 衝突した複数のポリゴンと衝突回避するまで、
		// プレイヤーの位置を移動させる
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];
			// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
			// この時、移動させる方向は、移動前座標に向いた方向であったり、
			// 衝突したポリゴンの法線方向だったりする
			for (int tryCnt = 0; tryCnt < MAX_COLLISION_RESOLVE_ATTEMPTS; tryCnt++)
			{
				// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
				// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
				int pHit = HitCheck_Capsule_Triangle(
					capsule_->GetPosTop(), capsule_->GetPosDown(), WEAPON_RADIUS,
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					// 法線の方向にちょっとだけ移動させる
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, COLLISION_PUSHBACK_AMOUNT));
					//// カプセルも一緒に移動させる
					transform_.pos = movedPos_;
					transform_.Update();
					continue;
				}
				break;
			}
		}
		// 検出したポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

bool Player::IsRun(void)
{
	return isRun_;
}

float Player::GetAttackCnt(void)
{
	return attackCnt_;
}

float Player::GetAttackCnt2(void)
{
	return attackCnt2_;
}

float Player::GetAttackCnt3(void)
{
	return attackCnt3_;
}

float Player::GetStrongAttackCnt(void)
{
	return strongAttackCnt_;
}

float Player::GetRollingCnt(void)
{
	return rollingCnt_;
}

int Player::GetAttack(void)
{
	return attack_;
}

void Player::SetUltValue(float ultValue)
{
	ultValue_ = ultValue;
}

bool Player::IsLightning(void)
{
	return isLightning_;
}

void Player::StopEffect(void)
{
	StopEffekseer3DEffect(effectHitPlayId_);
	StopEffekseer3DEffect(effectUltPlayId_);
}
