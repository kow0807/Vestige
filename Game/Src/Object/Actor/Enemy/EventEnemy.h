#pragma once
#include "../ActorBase.h"
#include <vector>
#include "../ActorBase.h"
class AnimationController;
class Collider;
class Capsule;
class EnemyWeapon;
class EventPlayer;

class EventEnemy 
	: public ActorBase
{

public:

	static constexpr VECTOR INIT_POS = { 0.0f, 200.0f, 900.0f };			// 初期座標

	// カプセル関連
	static constexpr VECTOR CAPSULE_POS_TOP = { 0.0f, 190.0f, 0.0f };		// カプセルの上端のローカル座標
	static constexpr VECTOR CAPSULE_POS_DOWN = { 0.0f, 20.0f, 0.0f };		// カプセルの下端のローカル座標
	static constexpr float CAPUSULE_RADIUS = 20.0f;							// カプセルの半径

	// 武器関連
	static constexpr int WEAPON_TOP_INDEX = 3;								// 武器の先端のフレームインデックス
	static constexpr int WEAPON_DOWN_INDEX = 6;								// 武器の下端のフレームインデックス
	static constexpr VECTOR WEAPON_TOP_POS = { 0.0f, 0.0f, -100.0f };		// 武器の先端のローカル座標
	static constexpr float WEAPON_ROLL = 0.0f;								// 武器の回転
	static constexpr float WEAPON_FORWARD_OFFSET = -5.0f;					// 武器の前方向へのオフセット
	static constexpr float WEAPON_UP_OFFSET = -20.0f;						// 武器の上方向へのオフセット
	static constexpr float WEAPON_RIGHT_OFFSET = -5.0f;						// 武器の右方向へのオフセット
	static constexpr VECTOR WEAPON_UPVEC = { 0.0f,1.0f,0.0f };				// 武器の上方向ベクトル
	static constexpr float WEAPON_CAPSULE_FORWARD_OFFSET = 20.0f;			// 武器の当たり判定カプセルの前方向へのオフセット
	static constexpr float WEAPON_CAPSULE_UP_OFFSET = 0.0f;					// 武器の当たり判定カプセルの上方向へのオフセット
	static constexpr float WEAPON_CAPSULE_RIGHT_OFFSET = 5.0f;				// 武器の当たり判定カプセルの右方向へのオフセット
	static constexpr VECTOR WEAPON_CAPSULE_UPVEC = { 0.0f,1.0f,0.0f };		// 武器の当たり判定カプセルの上方向ベクトル

	// 音関連
	static constexpr int SOUND_VALUE = 255;									// 音量

	// アニメーション関連
	static constexpr float IDLE_ANIM_SPEED = 20.0f;							// 待機アニメーション速度
	static constexpr float WALK_ANIM_SPEED = 10.0f;							// 歩きアニメーション速度
	static constexpr float LOOK_ANIM_SPEED = 20.0f;							// 見るアニメーション速度
	static constexpr float STANDBY_ANIM_SPEED = 20.0f;						// 待機アニメーション速度
	static constexpr float ANIM_BLEND_RATE = 5.0f;							// アニメーションのブレンド率

	// エフェクト関連
	static constexpr float EFFECT_SIZE = 25.0f;								// エフェクトの大きさ
	static constexpr float POS_Y_ADJUSTMENT = 100.0f;						// エフェクトのY位置調整
	static constexpr float TIME_LIMIT = 0.85f;								// エフェクトの時間制限
	
	// 回転の閾値
	static constexpr double THRESHOLD_ROT = 0.1;							// 回転の閾値

	// 回転完了までの時間
	static constexpr float TIME_ROT = 1.0f;									// 回転完了までの時間

	// 長さの制限
	static constexpr float LENGTH_LIMIT = 0.001f;							// 長さの制限

	// 球面回転比率
	static constexpr float SLEARP_RATIO = 0.05f;							// 球面補間の速度

	// 重力関連
	static constexpr float GRAVITY_CHECK_POW = 10.0f;						// 地面判定の強さ
	static constexpr float FALLINT_DOT_THRESHOLD = 0.9f;					// 落下判定の内積の閾値


	enum class STATE {
		NONE,
		IDLE,
		WALK,
		LOOK,
		STANDBY
	};

	enum class ANIM_TYPE {
		IDLE,
		WALK,
		LOOK,
		STANDBY
	};

	EventEnemy(EventPlayer& player);
	~EventEnemy();

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;

	void SetPosition(VECTOR pos);
	void SetGoalRotate(double rotRad);

	void ChangeState(STATE state);
	
	// 着地モーション終了
	bool IsEndLanding(ANIM_TYPE anim);

	// 衝突判定に用いられるコライダ制御
	void AddCollider(std::shared_ptr<Collider> collider);
	void ClearCollider(void);

	// 衝突用カプセルの取得
	const std::shared_ptr<Capsule> GetCapsule(void) const;

	int GetEffectRoarFPlayId(void);

private:
	// アニメーション
	std::unique_ptr<AnimationController> animationController_;

	// 武器
	std::shared_ptr<EnemyWeapon> weapon_;

	// イベント用のプレイヤー
	EventPlayer& player_;

	// 状態
	STATE state_;

	// 移動スピード
	float speed_;

	// 移動方向
	VECTOR moveDir_;

	// 移動量
	VECTOR movePow_;

	// 移動後の座標
	VECTOR movedPos_;

	// 回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;
	float stepRotTime_;

	// ジャンプ量
	VECTOR jumpPow_;

	// ジャンプ判定
	bool isJump_;

	// 衝突判定に用いられるコライダ
	std::vector<std::shared_ptr<Collider>> colliders_;
	std::shared_ptr<Capsule> capsule_;

	// 衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	//武器のボーンインデックス
	int weaponBoneIndex_;

	// 手首のフレーム
	int wristIndex_;
	VECTOR wristPos;

	// 手のフレーム
	int handIndex_;
	VECTOR handPos;

	// 武器のトップ
	int weaponTopIndex_;
	VECTOR weaponTopPos_;

	// 武器の下
	int weaponDownIndex_;
	VECTOR weaponDownPos_;

	int sHandle_;
	bool isSound_;

	float timer_;

	// アニメーションの初期化
	void InitAnimation(void);

	// 状態遷移
	void ChangeStateNone(void);
	void ChangeStateIdle(void);
	void ChangeStateWalk(void);
	void ChangeStateLook(void);
	void ChangeStateStandBy(void);

	// 更新ステップ
	void UpdateNone(void);
	void UpdateIdle(void);
	void UpdateWalk(void);
	void UpdateLook(void);
	void UpdateStandBy(void);

	// 回転
	void Rotate(void);

	// 常にプレイヤーの方向を向く
	void LookAt(VECTOR pos);				

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	// 移動量の計算
	void CalcGravityPow(void);

	VECTOR WeaponTopPos(VECTOR pos);

	// 武器の更新
	void UpdateRightWeapon(void);
	void UpdateWeapon();

	// エフェクト
	int effectRoarFPlayId_;
	int effectRoarFResId_;
	bool IsEffect_;

	void InitEffect(void);
};