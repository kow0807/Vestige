#pragma once
#include <map>
#include <DxLib.h>
#include "../ActorBase.h"
class AnimationController;
class Collider;
class Capsule;
class EnemyBoss;
class EnemyBossDummy;
class EnemyScarecrow;
class EnemyScarecrowDummy;
class PlayerWeapon;
class HpPotion;
class UltPotion;
class Book;
class Mist;

class Player : public ActorBase
{

public:

	static constexpr int ATTACK_SE_VOL = 230;			// アタック音
	static constexpr int HIT_SE_VOL = 220;				// ヒット音
	static constexpr int ROLLING_SE_VOL = 200;			// ローリング音
	static constexpr int ULT_SE_VOL = 255;				// 必殺音
	static constexpr int DAMAGE_SE_VOL = 240;			// ダメージ音
	static constexpr int ATTACK_SE_TIMING = 50;			// 一連撃目アタック音を鳴らすタイミング
	static constexpr int ATTACK2_SE_TIMING = 40;		// 二連撃目アタック音を鳴らすタイミング
	static constexpr int ATTACK3_SE_TIMING = 80;		// 三連撃目アタック音を鳴らすタイミング
	static constexpr int STRONG_ATTACK_SE_TIMING = 70;	// 強攻撃アタック音を鳴らすタイミング
	static constexpr int ROLLING_SE_TIMING = 65;		// ローリング音を鳴らすタイミング

	static constexpr VECTOR INIT_PLAYER_POS = { 0.0f, -100.0f, -400.0f };	// プレイヤー初期位置
	static constexpr VECTOR INIT_TOP_POS = { 0.0f, 190.0f, 0.0f };			// プレイヤーの頭上位置
	static constexpr VECTOR INIT_DOWN_POS = { 0.0f, 20.0f, 0.0f };			// プレイヤーの足元位置
	static constexpr float PLAYER_RADIUS = 30.0f;							// プレイヤーカプセルの半径
	static constexpr VECTOR INIT_QUA_ROT_LOCAL = { 0.0f, 180.0f, 0.0f };	// プレイヤー初期角度
	static constexpr float PLAYER_DEAD_TIME = 150.0f;						// プレイヤーが死亡と見なされるまでの時間
	static constexpr float SPEED_MOVE = 5.0f;								// 歩きスピード
	static constexpr float SPEED_RUN = 10.0f;								// 走りスピード
	static constexpr float TIME_ROT = 0.3f;									// 回転完了までの時間

	static constexpr int MAX_HP = 100;						// 最大HP
	static constexpr int HP_POS_X = 150;					// HPバーの位置X
	static constexpr int HP_POS_Y = 60;						// HPバーの位置Y
	static constexpr int HP_SIZE_X = 450;					// HPバーのサイズX
	static constexpr int HP_SIZE_Y = 15;					// HPバーのサイズY
	static constexpr float HP_DECREASE_SPEED = 0.5f;		// HPバーの減少スピード
	static constexpr float HP_POTION_HEAL_AMOUNT = 15.0f;	// 回復薬の回復量


	// 色
	static constexpr int HP_COLOR_TOP_R = 60;				// HPバーの上部の色R
	static constexpr int HP_COLOR_TOP_G = 200;				// HPバーの上部の色G
	static constexpr int HP_COLOR_TOP_B = 60;				// HPバーの上部の色B
	static constexpr int HP_COLOR_BOTTOM_R = 30;			// HPバーの下部の色R
	static constexpr int HP_COLOR_BOTTOM_G = 150;			// HPバーの下部の色G
	static constexpr int HP_COLOR_BOTTOM_B = 30;			// HPバーの下部の色B
	static constexpr int DAMAGE_COLOR_R = 180;				// ダメージ時のHPバーの色R
	static constexpr int DAMAGE_COLOR_G = 80;				// ダメージ時のHPバーの色G
	static constexpr int DAMAGE_COLOR_B = 80;				// ダメージ時のHPバーの色B
	static constexpr int HEAL_COLOR_R = 100;				// 回復時のHPバーの色R
	static constexpr int HEAL_COLOR_G = 200;				// 回復時のHPバーの色G
	static constexpr int HEAL_COLOR_B = 200;				// 回復時のHPバーの色B
	static constexpr int HP_OUTER_BORDER_COLOR_R = 80;		// HPバーの外側の色R
	static constexpr int HP_OUTER_BORDER_COLOR_G = 80;		// HPバーの外側の色G
	static constexpr int HP_OUTER_BORDER_COLOR_B = 80;		// HPバーの外側の色B
	static constexpr int HP_INNER_BORDER_COLOR_R = 160;		// HPバーの内側の色R
	static constexpr int HP_INNER_BORDER_COLOR_G = 160;		// HPバーの内側の色G
	static constexpr int HP_INNER_BORDER_COLOR_B = 160;		// HPバーの内側の色B
	static constexpr int HP_BG_COLOR_R = 30;				// HPバーの背景の色R
	static constexpr int HP_BG_COLOR_G = 30;				// HPバーの背景の色G
	static constexpr int HP_BG_COLOR_B = 30;				// HPバーの背景の色B
	static constexpr int OUTER_BORDER_OFFSET = 2;			// HPバーの外側のオフセット
	static constexpr int INNER_BORDER_OFFSET = 1;			// HPバーの内側のオフセット

	static constexpr int STAMINA_COLOR_TOP_R = 220;				// スタミナバー上部の色R
	static constexpr int STAMINA_COLOR_TOP_G = 220;				// スタミナバー上部の色G
	static constexpr int STAMINA_COLOR_TOP_B = 80;				// スタミナバー上部の色B
	static constexpr int STAMINA_COLOR_BOTTOM_R = 180;			// スタミナバー下部の色R
	static constexpr int STAMINA_COLOR_BOTTOM_G = 180;			// スタミナバー下部の色G
	static constexpr int STAMINA_COLOR_BOTTOM_B = 50;			// スタミナバー下部の色B
	static constexpr int STAMINA_OUTER_BORDER_COLOR_R = 60;		// スタミナバーの外枠の色R
	static constexpr int STAMINA_OUTER_BORDER_COLOR_G = 60;		// スタミナバーの外枠の色G
	static constexpr int STAMINA_OUTER_BORDER_COLOR_B = 60;		// スタミナバーの外枠の色B
	static constexpr int STAMINA_INNER_BORDER_COLOR_R = 180;	// スタミナバーの内枠の色R
	static constexpr int STAMINA_INNER_BORDER_COLOR_G = 180;	// スタミナバーの内枠の色G
	static constexpr int STAMINA_INNER_BORDER_COLOR_B = 180;	// スタミナバーの内枠の色B
	static constexpr int STAMINA_BG_COLOR_R = 30;				// スタミナバーの背景の色R
	static constexpr int STAMINA_BG_COLOR_G = 30;				// スタミナバーの背景の色G
	static constexpr int STAMINA_BG_COLOR_B = 30;				// スタミナバーの背景の色B

	static constexpr int STAMINA_POS_X = 150;			// スタミナバー位置X
	static constexpr int STAMINA_POS_Y = 80;			// スタミナバー位置Y
	static constexpr int STAMINA_SIZE_X = 350;			// スタミナバーサイズX
	static constexpr int STAMINA_SIZE_Y = 15;			// スタミナバーサイズY
	static constexpr int MAX_STAMINA = 255;				// 最大スタミナ
	static constexpr int ROLLING_STAMINA = 50;			// ローリングの際のスタミナ消費量
	static constexpr int STRONG_ATTACK_STAMINA = 30;	// 強攻撃の際のスタミナ消費量
	static constexpr int STAMINA_RECOVERY_RATE = 1;		// スタミナの回復量
	static constexpr int STAMINA_DRAIN_RATE = 1;		// スタミナの減少量

	static constexpr int NORMAL_DAMAGE = 100;			// 普通のダメージ
	static constexpr int STRONG_DAMAGE = 130;			// 強攻撃のダメージ
	static constexpr int SPECIAL_NORMAL_DAMAGE = 150;	// バフ時の普通のダメージ
	static constexpr int SPECIAL_STRONG_DAMAGE = 180;	// バフ時の強攻撃のダメージ

	static constexpr float MAX_ATTACK_CNT = 60.0f;			// 最大攻撃1カウント
	static constexpr float MAX_ATTACK_CNT_2 = 60.0f;		// 最大攻撃2カウント
	static constexpr float MAX_ATTACK_CNT_3 = 110.0f;		// 最大攻撃3カウント
	static constexpr float MAX_STRONG_ATTACK_CNT = 90.0f;	// 最大強攻撃カウント

	static constexpr int ATTACK_DISABLE_TIME = 20;			// 攻撃1無効時間
	static constexpr int ATTACK2_DISABLE_TIME = 20;			// 攻撃2無効時間
	static constexpr int ATTACK3_DISABLE_TIME = 30;			// 攻撃3無効時間
	static constexpr int STRONG_ATTACK_DISABLE_TIME = 30;	// 強攻撃無効時間

	static constexpr float MAX_ROLLING_CNT = 70.0f;			// 最大ローリングカウント
	static constexpr float ROLLING_MOVE = 10.0f;			// ローリング移動量

	static constexpr int ULT_CENTER_X = 80;						// ULTの中央位置X
	static constexpr int ULT_CENTER_Y = 80;						// ULTの中央位置Y
	static constexpr int ULT_RADIUS = 50;						// ULTの半径
	static constexpr int FLASH_BASE_RADIUS = 30;				// FLASHの基本半径
	static constexpr int FLASH_RING_SPACING = 10;				// FLASHのリング半径
	static constexpr int FLASH_RADIUS_GROWTH = 2;				// FLASHの半径成長
	static constexpr float ULT_MAX_VALUE = 1.0f;				// ULTの最大量
	static constexpr float ULT_CHARGE_SPEED = 0.1f;				// ULTのチャージスピード
	static constexpr float ULT_DEPLETION_SPEED = 0.2f;			// ULTの減少スピード
	static constexpr float MAGIC_CIRCLE_ROTATE_SPEED = 0.1f;	// マジックサークル回転速度
	static constexpr float FULL_CIRCLE_DEG = 360.0f;			// サークルの最大角度（360度）
	static constexpr float ULT_DRAW_SCALE = 0.15f;				// ULT描画時のスケール
	static constexpr int ULT_BRIGHTEN_ALPHA = 220;				// 明るいアルファ値
	static constexpr int ULT_SOFT_PULSE_ALPHA = 40;				// ソフトパルス用のアルファ値
	static constexpr int ULT_SEGMENT_COUNT = 100;				// ULTの円弧セグメント数
	static constexpr float ULT_START_ANGLE = 90.0f;				// ULT描画開始角度
	static constexpr int ULT_FLASH_DURATION = 40;				// FLASHエフェクトの持続フレーム数
	static constexpr int ULT_RAY_COUNT = 12;					// 放射する光線の本数
	static constexpr int ULT_RAY_THICKNESS = 3;					// 光線の太さ
	static constexpr int FLASH_ALPHA_DECAY = 5;					// FLASHのアルファ減衰量
	static constexpr int FLASH_ROTATE_SPEED = 10;				// FLASHの回転速度
	static constexpr int RAY_START_ALPHA = 200;					// 光線の初期アルファ値
	static constexpr int RAY_ALPHA_DECAY_PER_FRAME = 5;			// 光線のアルファ減衰量（フレームごと）
	static constexpr int FLASH_COLOR_R = 250;					// FLASHの色R
	static constexpr int FLASH_COLOR_G = 250;					// FLASHの色G
	static constexpr int FLASH_COLOR_B = 255;					// FLASHの色B
	static constexpr int RAY_COLOR_R = 180;						// 光線の色R
	static constexpr int RAY_COLOR_G = 240;						// 光線の色G
	static constexpr int RAY_COLOR_B = 255;						// 光線の色B
	static constexpr float ULT_EFFECT_SCALE = 100.0f;			// ULTエフェクト全体のスケール

	static constexpr int MAX_ALPHA = 255;						// 最大アルファ値


	static constexpr float WEAPON_RADIUS = 30.0f;								// 武器の当たり判定半径
	static constexpr int WEAPON_TOP_INDEX = 3;									// 武器の頂点インデックス（上方向）
	static constexpr int WEAPON_DOWN_INDEX = 6;									// 武器の頂点インデックス（下方向）
	static constexpr VECTOR INIT_WEAPON_TOP_POS = { 0.0f, 0.0f, -100.0f };		// 武器の初期先端位置（Zマイナス方向）
	static constexpr float FORWARD_OFFSET = 20.0f;								// 武器位置の前方向オフセット
	static constexpr float UP_OFFSET = 0.0f;									// 武器位置の上方向オフセット
	static constexpr float RIGHT_OFFSET = 5.0f;									// 武器位置の右方向オフセット
	static constexpr float EFFECT_HIT_SCALE = 25.0f;							// ヒットエフェクトのスケール

	static constexpr float ENEMY_HIT_SPHERE_RADIUS = 1.0f;		// 敵側当たり判定球の半径

	static constexpr float IDLE_ANIM_SPEED = 20.0f;				// 待機モーションの速度
	static constexpr float RUN_ANIM_SPEED = 20.0f;				// 走りモーションの速度
	static constexpr float FAST_RUN_ANIM_SPEED = 20.0f;			// ダッシュモーションの速度
	static constexpr float JUMP_ANIM_SPEED = 60.0f;				// ジャンプモーションの速度
	static constexpr float WARP_PAUSE_ANIM_SPEED = 60.0f;		// ワープ前の溜めモーション速度
	static constexpr float FLY_ANIM_SPEED = 60.0f;				// 飛行モーションの速度
	static constexpr float FALLING_ANIM_SPEED = 80.0f;			// 落下モーションの速度
	static constexpr float VICTORY_ANIM_SPEED = 60.0f;			// 勝利モーションの速度
	static constexpr float ATTACK_ANIM_SPEED = 40.0f;			// 通常攻撃1の速度
	static constexpr float ATTACK2_ANIM_SPEED = 40.0f;			// 通常攻撃2の速度
	static constexpr float ATTACK3_ANIM_SPEED = 40.0f;			// 通常攻撃3の速度
	static constexpr float STRONG_ATTACK_ANIM_SPEED = 40.0f;	// 強攻撃の速度
	static constexpr float STUN_ANIM_SPEED = 40.0f;				// スタン時のモーション速度
	static constexpr float ROLLING_ANIM_SPEED = 30.0f;			// 回避・ローリングモーションの速度
	static constexpr float DEAD_ANIM_SPEED = 30.0f;				// 死亡モーションの速度

	static constexpr float STUN_RECHARGE_THRESHOLD = -90.0f;	// スタン解除の角度しきい値
	static constexpr float STUN_DURATION = 180.0f;				// スタン継続時間（フレーム数）

	static constexpr int PAD_INPUT_THRESHOLD_XY = 500;			// XY入力のしきい値
	static constexpr int PAD_INPUT_THRESHOLD_Z = -800;			// Z入力のしきい値
	static constexpr int ATTACK_PAD_BUTTON_NUM = 5;				// 攻撃ボタン番号
	static constexpr int POV_RIGHT = 9000;						// POV入力：右方向
	static constexpr int POV_LEFT = 27000;						// POV入力：左方向
	static constexpr int INVALID_POV = -1;						// POV入力なし
	static constexpr int PAD_VIBRATION_POWER = 40000;			// パッド振動の強さ
	static constexpr int PAD_VIBRATION_DURATION_MS = 500;		// パッド振動の持続時間(ms)

	static constexpr int WHEEL_THRESHOLD = 3;					// ホイール入力のしきい値

	static constexpr int LIGHTNING_UPDATE_FRAME = 60;			// 雷エフェクト更新間隔
	static constexpr int LIGHTNING_START_FRAME = 40;			// 雷発生までのフレーム数
	static constexpr int LIGHTNING_DURATION_FRAME = 120;		// 雷エフェクトの持続フレーム数

	static constexpr double ROTATION_THRESHOLD_RAD = 0.1;		// 回転を発生させる角度（ラジアン）しきい値

	static constexpr float GROUND_CHECK_DISTANCE = 10.0f;								// 地面との判定距離
	static constexpr float GROUND_CHECK_UP_OFFSET = GROUND_CHECK_DISTANCE * 2.0f;		// 地面判定開始位置の上方向オフセット

	static constexpr float GRAVITY_COLLISION_DOT_THRESHOLD = 0.9f;		// 法線ベクトルと重力方向の内積しきい値（0.9以上で接地とみなす）

	static constexpr float LANDING_ADJUST_HEIGHT = 2.0f;				// 着地時の高さ補正量
	static constexpr float LANDING_ANIM_START_FRAME = 29.0f;			// 着地アニメーション開始フレーム
	static constexpr float LANDING_ANIM_END_FRAME = 45.0f;				// 着地アニメーション終了フレーム

	static constexpr int MAX_COLLISION_ADJUST_COUNT = 10;				// カプセルがポリゴンに埋まった際に、法線方向に押し戻しを試行する最大回数
	static constexpr float COLLISION_PUSHBACK_AMOUNT = 1.0f;			// 押し戻す距離
	static constexpr int MAX_COLLISION_RESOLVE_ATTEMPTS = 12;			// 何回まで衝突回避を試みるか

	// 状態
	enum class STATE
	{
		NONE,
		PLAY,
		WARP_RESERVE,
		WARP_MOVE,
		DEAD,
		VICTORY,
		END
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		JUMP,
		WARP_PAUSE,
		FLY,
		FALLING,
		VICTORY,
		ATTACK,
		ATTACK2,
		ATTACK3,
		STRONG_ATTACK,
		STUN,
		ROLLING,
		DEAD,
	};

	// アイテム
	enum class ITEM_TYPE
	{
		HP_POTION,
		ULT_POTION,
		BOOK,
		NUM
	};


	Player(void);	// コンストラクタ

	~Player(void);	// デストラクタ

	void Init(void) override;						// 初期化処理
	void Update(void) override;						// 毎フレーム更新処理
	void Draw(void) override;						// 描画処理

	void AddCollider(std::shared_ptr<Collider> collider);	// コライダ追加
	void ClearCollider(void);								// コライダ全削除

	const std::shared_ptr<Capsule> GetCapsule(void) const;	// 衝突用カプセルの取得

	void SetEnemy(std::shared_ptr<EnemyBoss> enemy);										// 対象の敵ボス設定
	void SetEnemyDummy(std::shared_ptr<EnemyBossDummy> enemyDummy);							// ボスダミー設定
	void SetEnemyScarecrow(std::shared_ptr<EnemyScarecrow> enemyScarecrow);					// かかし敵設定
	void SetEnemyScarecrowDummy(std::shared_ptr<EnemyScarecrowDummy> enemyScarecrowDummy);	// かかしダミー設定
	void SetMist(std::shared_ptr<Mist> mist);												// ミスト（霧エフェクト）設定

	bool IsHitAttackEnemyBoss(void);					// ボスに攻撃がヒットしたか
	bool IsHitAttackEnemyScarecrow(void);				// かかしに攻撃がヒットしたか
	float GetHp(void);									// 現在HP取得
	int GetStamina(void);								// 現在スタミナ取得

	void Damage(int damage);							// ダメージ処理

	bool IsDead(void);									// 死亡状態か確認

	VECTOR GetMovePow(void);							// 移動量ベクトル取得

	int GetID() const;									// プレイヤーID取得

	void StunUpdate(void);								// スタン状態の更新処理

	void CollisionEnemyBoss(void);						// ボスとの衝突判定
	void CollisionEnemyScarecrow(void);					// かかしとの衝突判定
	void CollisionMist(void);							// ミストとの衝突判定

	bool IsRun(void);									// 走行中か判定
	float GetAttackCnt(void);							// 攻撃1のカウント値取得
	float GetAttackCnt2(void);							// 攻撃2のカウント値取得
	float GetAttackCnt3(void);							// 攻撃3のカウント値取得
	float GetStrongAttackCnt(void);						// 強攻撃のカウント値取得
	float GetRollingCnt(void);							// 回避ローリングのカウント値取得

	int GetAttack(void);								// 攻撃力を取得

	void SetUltValue(float ultValue);					// ULTゲージ値を設定

	bool IsLightning(void);								// 雷状態か判定
	void StopEffect(void);								// エフェクト停止

	void DrawHp(float hp, int maxHp);					// HPゲージ描画
	void DrawStamina(float stamina, int staminaMax);	// スタミナゲージ描画
	void DrawItem(void);								// 所持アイテム描画
	void DrawUlt();										// ULTゲージ描画

	void TutorialUlt(void);								// チュートリアル用ULT演出

private:

	// 音
	int attackSe_;
	int hitSe_;
	int rollingSe_;
	int ultSe_;
	int damageSe_;

	AnimationController* animationController_;					// アニメーション制御用

	std::shared_ptr<EnemyBoss> enemy_;							// 対象の敵ボス
	std::shared_ptr<EnemyBossDummy> enemyDummy_;				// ダミーボス
	std::shared_ptr<EnemyScarecrow> enemyScarecrow_;			// かかし敵
	std::shared_ptr<EnemyScarecrowDummy> enemyScarecrowDummy_;	// かかしダミー
	std::shared_ptr<PlayerWeapon> weapon_;						// 武器
	std::unique_ptr<HpPotion> hpPotion_;						// HPポーション
	std::unique_ptr<UltPotion> ultPotion_;						// ULTポーション
	std::unique_ptr<Book> book_;								// 魔法書
	std::shared_ptr<Mist> mist_;								// ミスト（霧エフェクト）

	// 状態管理
	STATE state_;

	// アイテム状態
	ITEM_TYPE itemType_;

	// 移動スピード
	float speed_;

	// 移動方向
	VECTOR moveDir_;

	// 移動量
	VECTOR movePow_;

	// 移動後の座標
	VECTOR movedPos_;

	// RUN状態かどうか
	bool isRun_;

	// 回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;
	float stepRotTime_;

	// ジャンプ量
	VECTOR jumpPow_;

	// ジャンプ判定
	bool isJump_;

	// ジャンプの入力受付時間
	float stepJump_;

	// 攻撃力
	int attack_;

	// 攻撃のアニメーションカウント
	float attackCnt_;					// 攻撃1カウント
	float attackCnt2_;					// 攻撃2カウント
	bool isAttack2_;					// 攻撃2中か
	float attackCnt3_;					// 攻撃3カウント
	bool isAttack3_;					// 攻撃3中か

	float strongAttackCnt_;				// 強攻撃カウント

	float stunCnt_;						// スタン時間カウント

	float deadCnt_;						// 死亡時カウント

	float rollingCnt_;					// ローリングカウント

	std::vector<std::shared_ptr<Collider>> colliders_;		// 衝突判定に用いられるコライダ
	std::shared_ptr<Capsule> capsule_;						// プレイヤー当たり判定カプセル

	VECTOR gravHitPosDown_;				// 下方向の接地判定座標
	VECTOR gravHitPosUp_;				// 上方向の接地判定座標

	bool canAttack_;					// 攻撃可能かどうか

	int wristIndex_;					// 手首のフレーム
	VECTOR wristPos_;					// 手首座標

	int handIndex_;						// 手のフレーム
	VECTOR handPos_;						// 手座標


	int weaponTopIndex_;				// 武器のトップ
	VECTOR weaponTopPos_;				// 武器先端座標


	int weaponDownIndex_;				// 武器の下
	VECTOR weaponDownPos_;				// 武器下端座標

	float currentHp_;					// 現在HP
	float displayHp_;					// 表示用HP
	float damageHp_;					// ダメージ演出用HP
	float healHp_;						// 回復演出用HP

	int stamina_;						// スタミナ

	int id_;							// プレイヤーID

	int ultFrameHandle_;				// ULTゲージ用フレーム画像
	int magicCircleHandle_;				// 魔法陣画像
	int runeHandle_;					// ルーン画像
	float ultValue_;					// ULTゲージ値
	bool isUltActive_;					// ULT発動中か
	bool wasUltMaxed_;					// 一度でもULTが最大になったか
	int flashTimer_;					// ULT発動演出用タイマー

	int lightningTimer_;				// 雷エフェクト用タイマー
	bool isLightning_;					// 雷エフェクト有効か

	void InitAnimation(void);			// アニメーション初期化

	void ChangeState(STATE state);		// 状態を変更
	void ChangeStateNone(void);			// 状態をNoneに変更
	void ChangeStatePlay(void);			// 状態をPlayに変更

	void ChangeItem(ITEM_TYPE itemType);	// アイテム変更

	void UpdateNone(void);				// None状態更新
	void UpdatePlay(void);				// Play状態更新

	void ProcessMove(void);				// 移動処理

	void SetGoalRotate(double rotRad);	// 回転目標角度を設定
	void Rotate(void);					// 回転補間処理

	void Collision(void);				// 衝突判定処理
	void CollisionGravity(void);		// 重力方向の衝突判定
	void CollisionCapsule(void);		// カプセル衝突判定

	void CalcGravityPow(void);			// 重力を考慮した移動量計算

	bool IsEndLanding(void);			// 着地モーションが終了したか

	void UpdateRightWeapon(void);		// 右手武器更新

	void UpdateItem(void);				// アイテム更新処理
	void UseHpPosion(void);				// HPポーション使用
	void UseUltPosion(void);			// ULTポーション使用
	void UseBook(void);					// 魔法書使用

	VECTOR WeaponTopPos(VECTOR pos);	// 武器先端座標を返す
	VECTOR WeaponDownPos(VECTOR pos);	// 武器下端座標を返す

	void UpdateWeaponCapsule();			// 武器の当たり判定更新

	void UpdateUlt(void);				// ULTゲージ更新

	int effectHitPlayId_;				// ヒットエフェクト再生ID
	int effectHitResId_;				// ヒットエフェクトリソースID

	int effectUltPlayId_;				// ULTエフェクト再生ID
	int effectUltResId_;				// ULTエフェクトリソースID

	void InitEffect(void);				// エフェクト初期化

};
