#pragma once
#include <map>
#include <vector>
#include <unordered_set>
#include "../EnemyBoss.h"

class EnemyBoss;

// 攻撃状態処理管理
class EnemyBossCombatSystem
{

public:
	
	static constexpr float COMBO_RESET_TIME = 2.0f;		// 連続攻撃がリセットされる時間
	static constexpr float ATTACK_COOLDOWN = 1.0f;		// 攻撃のクールダウン
	static constexpr float ATTACK_DURATION = 0.5f;		// 攻撃の間隔
	static constexpr int MAX_COMBO = 2;					// 最大連続攻撃数

	static constexpr float EFFECT_SIZE = 25.0f;			// エフェクトの大きさ

	static constexpr float SPHERE_RADIUS = 128.0f;		// 当たり判定の半径の大きさ

	EnemyBossCombatSystem(EnemyBoss& boss);
	~EnemyBossCombatSystem();

	// 更新
	void Update(float deltaTime);

	// 攻撃処理
	void ExcuteAttack(EnemyBoss::ATK_STATE pattern);

	// プレイイヤーにあったか
	bool IsHit();

	// 攻撃をリセット
	void ClearHitRecord();

	// 攻撃可能か(クールダウン等)
	bool CanAttack() const;

private:
	EnemyBoss& boss_;

	float attackCooldown_;		// クールダウン時間
	float currentCooldown_;		// 現在のクールダウン時間

	bool isAttacking_;			// 攻撃中かどうか
	float attackDuration_;		// 攻撃間隔
	float attackTimer_;			// 攻撃時間

	int comboCount_;			// 連続攻撃回数
	int maxCombo_;				// 最大連続攻撃回数(３回)
	float comboResetTimer_;		// 現在の連続攻撃のリセット時間
	const float comboRestTime_;	// 連続攻撃のリセット時間

	// エフェクト
	int effectHitPlayId_;		// 再生用ハンドル
	int effectHitResId_;		// エフェクトハンドル

	std::unordered_set<int> hitPlayers_;	// 当たり判定用

	void InitEffect(void);
};

