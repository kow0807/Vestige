#pragma once
#include "../../ActorBase.h"
#include <map>
#include <unordered_set>
#include <functional>
#include <vector>
class Collider;
class Capsule;
class Player;
class PlayerDummy;
class EnemyWeapon;
class Mist;

class EnemyStateMachine;
class EnemyBossAnimation;
class EnemyBossCombatSystem;
class EnemyBossCombatController;
class EnemyBossAttackStateMachine;

class EnemyBoss : public ActorBase
{
public:


	static constexpr VECTOR INIT_POS = { 0.0f, -30.0f, 900.0f };			// 初期座標

	static constexpr float SIZE = 1.3f;										// モデルの大きさ

	static constexpr float INIT_QUAROT_LOCAL_X = 0.0f;						// 初期ローカル回転(x軸)
	static constexpr float INIT_QUAROT_LOCAL_Y = 180.0f;					// 初期ローカル回転(y軸)
	static constexpr float INIT_QUAROT_LOCAL_Z = 0.0f;						// 初期ローカル回転(z軸

	static constexpr int MAX_HP_VALUE = 2000;								// 最大HP

	// 初期化アニメーション
	static constexpr float IDLE_ANIM_SPEED = 20.0f;							// 待機アニメーション速度
	static constexpr float WALK_F_ANIM_SPEED = 10.0f;						// 前歩きアニメーション速度
	static constexpr float WALK_B_ANIM_SPEED = 20.0f;						// 後歩きアニメーション速度
	static constexpr float RUN_ANIM_SPEED = 20.0f;							// 走りアニメーション速度
	static constexpr float STEP_ANIM_SPEED = 40.0f;							// 回避アニメーション速度
	static constexpr float COMBO_ANIM_SPEED = 55.0f;						// 連続攻撃アニメーション速度
	static constexpr float THRUST_ANIM_SPEED = 40.0f;						// 突進攻撃アニメーション速度
	static constexpr float ROAR_ANIM_SPEED = 20.0f;							// 咆哮アニメーション速度
	static constexpr float EXPLOSION_ANIM_SPEED = 15.0f;					// 爆発攻撃アニメーション速度
	static constexpr float STUN_IDLE_ANIM_SPEED = 20.0f;					// スタン待機アニメーション速度
	static constexpr float SET_UP_ANIM_SPEED = 20.0f;						// 準備アニメーション速度
	static constexpr float DEATH_ANIM_SPEED = 30.0f;						// 死亡アニメーション速度
	static constexpr float ANIM_BLEND_RATE = 0.5f;							// アニメーションのブレンド率

	// 重力関連
	static constexpr float GRAVITY_CHECK_POW = 10.0f;						// 地面判定の強さ
	static constexpr float FALLINT_DOT_THRESHOLD = 0.9f;					// 落下判定の内積の閾値

	// カプセル関連
	static constexpr float CAPSULE_TOP_LOCAL_POS_Y = 150.0f;				// カプセルの上端のローカル座標
	static constexpr float CAPSULE_DOWN_LOCAL_POS_Y = 30.0f;				// カプセルの下端のローカル座標
	static constexpr float CAPSULE_RADIUS = 30.0f;							// カプセルの半径
	static constexpr int CAPSULE_LIMIT_COUNT = 10;							// カプセルの衝突判定の最大試行回数

	// ダメージ関連
	static constexpr float DECREASE_SPEED = 2.0f;							// ダメージゲージの減少速度

	// 武器関連
	static constexpr int WEAPON_TOP_INDEX = 3;								// 武器の先端のフレームインデックス
	static constexpr int WEAPON_DOWN_INDEX = 6;								// 武器の下端のフレームインデックス
	static constexpr float WEAPON_ROLL = 0.0f;								// 武器の回転
	static constexpr float WEAPON_FORWARD_OFFSET = -5.0f;					// 武器の前方向へのオフセット
	static constexpr float WEAPON_UP_OFFSET = -20.0f;						// 武器の上方向へのオフセット
	static constexpr float WEAPON_RIGHT_OFFSET = -5.0f;						// 武器の右方向へのオフセット
	static constexpr VECTOR WEAPON_UPVEC = { 0.0f,1.0f,0.0f };				// 武器の上方向ベクトル
	static constexpr float WEAPON_CAPSULE_FORWARD_OFFSET = 20.0f;			// 武器の当たり判定カプセルの前方向へのオフセット
	static constexpr float WEAPON_CAPSULE_UP_OFFSET = 0.0f;					// 武器の当たり判定カプセルの上方向へのオフセット
	static constexpr float WEAPON_CAPSULE_RIGHT_OFFSET = 5.0f;				// 武器の当たり判定カプセルの右方向へのオフセット
	static constexpr VECTOR WEAPON_CAPSULE_UPVEC = { 0.0f,1.0f,0.0f };		// 武器の当たり判定カプセルの上方向ベクトル

	// スタン関連
	static constexpr float STUN_TIME = 5.0f;								// スタン時間
	static constexpr int LIGHTNING_DAMAGE = 300;							// ライトニングのダメージ量

	// エフェクト関連
	// バフ
	static constexpr float BUFF_EFFECT_SIZE = 100.0f;						// バフエフェクトの大きさ
	static constexpr int BUFF_EFFECT_SOUND = 255;							// バフエフェクトの音量

	// スタン
	static constexpr float STUN_EFFECT_SIZE = 50.0f;						// スタンエフェクトの大きさ

	// 死亡関連
	static constexpr float DEAD_INTERVAL_TIME = 5.0f;						// 死亡してから消えるまでの時間

	// 回避のクールダウン間隔
	static constexpr float STEP_COOLDOWN_DURATON = 5.0f;					// 回避のクールダウン時間

	// HPバー関連
	static constexpr int WIDTH = 650;										// HPバーの幅
	static constexpr int HEIGHT = 20;										// HPバーの高さ
	static constexpr int X_ADJUSTMENT = 60;									// HPバーのX位置調整
	static constexpr int Y_ADJUSTMENT = 80;									// HPバーのY位置調整
	static constexpr int FONT_SIZE = 20;										// フォントサイズ
	static constexpr int NAME_HEIGHT = 24;									// ボスネームの高さ
	static constexpr int PADDING_LEFT = 8;									// ボスネームの左余白
	static constexpr int PADDING = 8;										// HPバーの内側余白
	static constexpr int NAME_BLEND_PARAM = 160;							// ボスネームのブレンド率
	static constexpr int MAX_COL = 255;										// 色の最大値
	static constexpr int TEXCOL_R = 255;									// テクスチャの色
	static constexpr int TEXCOL_G = 255;									// テクスチャの色
	static constexpr int TEXCOL_B = 255;									// テクスチャの色
	static constexpr int A_QUARTER_COL = 60;								// 透明度の1/4
	static constexpr int DAMEGE_COL_LIGHT_R = 255;							// ダメージ部分明るい色（白っぽい赤）
	static constexpr int DAMEGE_COL_LIGHT_GB = 200;							// ダメージ部分明るい色（白っぽい赤）
	static constexpr int DAMEGE_COL_TOP_R = 255;							// ダメージ上半分（明るい赤）
	static constexpr int DAMEGE_COL_TOP_GB = 100;							// ダメージ上半分（明るい赤）
	static constexpr int DAMEGE_COL_BOTTOM_R = 200;							// ダメージ下半分（暗い赤）
	static constexpr int DAMEGE_COL_BOTTOM_GB = 50;							// ダメージ下半分（暗い赤）
	static constexpr int HP_COL_TOP_R = 255;								// HPバー上半分（明るい赤）
	static constexpr int HP_COL_TOP_GB = 80;								// HPバー上半分（明るい赤）
	static constexpr int HP_COL_BOTTOM_R = 180;								// HPバー下半分（暗い赤）
	static constexpr int HP_COL_BOTTOM_GB = 40;								// HPバー下半分（暗い赤）
	static constexpr int BORDER_DARK_COL = 40;								// 外枠（暗め）
	static constexpr int BORDER_LIGHT_COL = 200;							// 外枠（明るめ）
	static constexpr int DAMAGE_BLEND_PARAM = 180;							// ダメージ部分のブレンド率
	static constexpr int BORDER_DARK_ADJUSTMENT = 2;						// 外枠の大きさ調整
	static constexpr int BORDER_LIGHT_ADJUSTMENT = 1;						// 外枠の大きさ調整

	// 長さの制限
	static constexpr float LENGTH_LIMIT = 0.001f;							// 長さの制限

	// 球面補間の速度
	static constexpr float SLERP_ROTIO = 0.05f;								// 球面補間の速度

	// 移動速度
	static constexpr float MOVE_SPEED = 5.0f;								// 移動速度
	
	// デバック用
	static constexpr float DEBUG_STATE_POS_Y = 100.0f;						// デバッグ表示のY位置
	static constexpr float DEBUG_ATTACK_POS_Y = 200.0f;						// デバッグ表示のY位置
	static constexpr int DEBUG_POS_X = 720;									// デバッグ表示のX位置
	static constexpr int DEBUG_DIST_POS_Y = 32;								// デバッグ表示のY位置
	static constexpr int DEBUG_DEADTIME_POSY = 64;							// デバッグ表示のY位置
	static constexpr int DEBUG_ATTACKBUFF_POSY = 96;						// デバッグ表示のY位置

	// 状態
	enum class STATE
	{
		IDLE,
		OBSERVE,
		MOVE,
		CHASE,
		ATTACK,
		STEP,
		STUN,
		RECOVER,
		DEAD
	};

	enum class ATK_STATE
	{
		NONE,
		COMBO,
		THRUST,
		ROAR,
		EXPLOSION,
		SWEEP,
		FOLLOW_SLASH,
		SETUP
	};

	EnemyBoss(Player& player);
	~EnemyBoss(void);

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;


	// 各クラスのゲッター
	//------------------
	std::shared_ptr<EnemyStateMachine> GetStateMachine() const;
	std::shared_ptr<EnemyBossAnimation> GetAnimation() const;
	std::shared_ptr<EnemyBossCombatSystem> GetAttackStateSystem() const;
	std::shared_ptr<EnemyBossCombatController> GetAttackStateController() const;
	std::shared_ptr<EnemyBossAttackStateMachine> GetAttackStateMachine() const;

	// 衝突判定に用いられるコライダ制御
	void AddCollider(std::shared_ptr<Collider> collider);
	void ClearCollider(void);

	// 霧との衝突判定用情報を取得する
	void SetMist(std::shared_ptr<Mist> mist);

	// 衝突用カプセルの取得
	const std::shared_ptr<Capsule> GetCapsule(void) const;

	// HPの取得
	int GetHp(void) const;

	// 現在のステート状態を取得
	STATE GetCurrentStateId() const;

	// メインステート変更インターフェース
	void ChangeState(STATE newState);
	
	// 攻撃ステート変更インターフェース
	void ChangeAttack(ATK_STATE newAttack);

	// EChaseStateからEAttackStateへ渡すための「次に実行する攻撃」
	void SetNextAttackType(ATK_STATE type);

	// EChaseStateからEAttackStateへ渡すための「次に実行する攻撃」の取得
	ATK_STATE GetNextAttackType() const;

	// プレイヤー関連
	Player& GetPlayer();

	// プレイヤーとの距離を取得
	float GetPlayerDistanceXZ() const;

	// プレイヤーへの方向ベクトルを取得
	VECTOR GetToPlayer();

	// モデルのTransformを取得
	void SetPosition(VECTOR pos);
	
	// モデルのTransformを取得(書き換え可能)
	Transform& GetTransformMutable();

	// ボスをスタン時間を設定する
	void SetStunned(float duration);

	// スタンしているかどうか
	bool IsStunned(void) const;

	// スタン時間の取得
	float  GetStunTimer(void) const;

	// 攻撃力アップ状態を設定
	void SetAttackBuff(float duration, float muktiplier);
	
	// 攻撃力アップ状態を解除
	void RemoveAttackBuff(void);
	
	// 現在の攻撃力倍率を取得
	float GetCurrentAttackMultiplier() const;

	// 死亡しているかどうか
	bool IsDead() const;
	
	// 死亡状態を設定
	void SetIsDead(bool flag);
	
	// 死亡処理
	void SetDead();
	
	// 死亡してからの経過時間を取得
	float GetDeadTimer() const;
	
	// 死亡アニメーション終了時のコールバック
	void OnDeathAnimationFinished(void);

	// スタンエフェクト開始
	void StartStunEffect(void);
	
	// スタンエフェクト停止
	void StopStunEffect(void);
	
	// スタンエフェクトがアクティブかどうか
	bool IsStunEffectActive(void);

	// SetUpエフェクト開始
	void StartSetupEffect(void);
	
	// SetUpエフェクト停止
	void StopSetupEffect(void);
	
	// SetUpエフェクトがアクティブかどうか
	bool IsSetupEffectActive(void);
	
	// 武器の先端の座標を取得
	VECTOR GetTopPos();
	
	// 武器の下端の座標を取得
	VECTOR GetDownPos();

	// 最大HPの取得
	int GetMaxHp() const;
	
	// HPの割合を取得
	float GetHpRatio() const;

	// HPバーの描画
	void DrawHpBar(void);

	// 引数の方向を向かせる
	void LookAt(VECTOR pos);

	// ターゲットの方に歩く
	void MoveToward(const VECTOR& target);

	// 攻撃範囲描画用
	void DrawCircleOnGround(VECTOR center, float radius, int segment, int color);

	// プレイヤーがボスの正面にいるかを判定する
	bool IsPlayerInFront(float viewAngleDeg, float maxDistance);

	// 咆哮をせず５割を切った状態を取得
	bool HasUsedRoarBelow50Percent(void);

	// 咆哮をせず５割を切った状態を変更
	void SetHasUsedRoarBelow50Percent(bool flag);

	// 爆発攻撃をせず５割を切った状態を取得
	bool HasUsedExplosionBelow50Percent(void);

	// 爆発攻撃をせず５割を切った状態を変更
	void SetHasUsedExplosionBelow50Percent(bool flag);
	
	//SETUP攻撃がHP4割以下で一度使用されたかを取得
	bool HasUsedSetupBelow40Percent(void);

	// //SETUP攻撃がHP4割以下で一度使用された状態かを取得
	void SetHasUsedSetupBelow40Percent(bool flag);

	// 霧との当たり判定
	void CollisionMist(void);

	// 回避のクールダウン時間を取得
	float GetStepCooldown(void);

	// 回避のクールダウン時間を設定
	void SetStepCooldown(float duration);

private:

	// プレイヤー
	Player& player_;

	std::shared_ptr<Mist> mist_;

	// アニメーション
	std::shared_ptr<EnemyWeapon> weapon_;

	// 状態遷移
	std::shared_ptr<EnemyStateMachine> stateMachine_;

	// 見た目と動作
	std::shared_ptr<EnemyBossAnimation> animation_;

	// 戦闘処理
	std::shared_ptr<EnemyBossCombatSystem> combat_;
	std::shared_ptr<EnemyBossCombatController> combatController_;
	std::shared_ptr<EnemyBossAttackStateMachine> attackFSM_;

	// 攻撃中に一回でもあったプレイヤーIDを記録する
	std::unordered_set<int> hitPlayers_;	

	int hp_;				// ボスのHP
	int maxHp_;				//　HPの最大値
	
	// EChaseStateからEAttackStateへ渡す次の攻撃タイプ
	ATK_STATE nextAttackType_;

	// 移動用
	VECTOR moveDir_;
	VECTOR movedPos_;
	VECTOR movePow_;

	VECTOR jumpPow_;
	bool isJump_;

	// プレイヤーとの距離
	VECTOR toPlayer_;

	// 衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	// 受けたダメージの描画用
	int damageHp_;

	// 衝突判定に用いられるコライダ
	std::vector<std::shared_ptr<Collider>> colliders_;
	std::shared_ptr<Capsule> capsule_;

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

	// 手のカプセル
	VECTOR capsuleDir_;
	VECTOR capsuleCenter_;

	// 武器用当たり半径
	float radius_;

	// ボスの自己スタン管理
	bool isStunned_;
	float stunTimer_;

	// スタンエフェクト関連
	int effectStunPlayId_;
	int effectStunResId_;
	bool isStunEffectActive_;

	// SetUpエフェクト関連
	int effectBuffPlayId_;
	int effectBuffResId_;
	bool isSetUpEffectActive_; // SetUpエフェクトがアクティブかどうか

	// 攻撃力アップ管理(SETUP用)
	float currentAttackMultiplier_;	// 基本攻撃力
	float attackBuffTimer_;			// 攻撃バフ残り時間

	bool isDead_;			// 死亡時：trueを返す
	float deadTimer_;		// 死亡時のタイマー

	float stepCooldown_;	// Stepクールダウン

	// 攻撃制限フラグ
	bool hasUsedRoarBelow50Percent_;		// 咆哮をせず５割を切ったとき
	bool hasUsedExplosionBelow50Percent_;	// 爆発攻撃をせず５割を切ったとき
	bool hasUsedSetupBelow40Percent_;	// SETUP攻撃がHP4割以下で一度使用されたかどうかのフラグ


	// モデル情報の初期化
	void InitModel(void);

	//パラメータの初期化
	void InitParam(void);

	// 状態遷移の初期化
	void InitStateMachine(void);

	// アニメーションの初期化
	void InitAnimation(void);

	// 攻撃パターンの初期化
	void InitAttackPattern(void);

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	// 移動量の計算
	void CalcGravityPow(void);

	// ダメージ処理
	void Damage(void);

	// デバック用描画
	void DrawDebug(void);

	// 武器用
	VECTOR GetWeaponCenter(void);
	void UpdateWeapon(void);	// 武器の更新
	void UpdateWeaponCapsule();	// 武器カプセルの更新

	// 音関連の初期化
	void InitEffect(void);

	// サウンド
	int sHandle_;
	bool isSound_;
	void InitSound(void);
};

