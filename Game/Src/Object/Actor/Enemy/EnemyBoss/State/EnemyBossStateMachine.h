#pragma once
#include <map>
#include <functional>
#include "../EnemyBoss.h"
#include "IEnemyState.h"

// 状態遷移管理
class EnemyStateMachine
{

public:

	EnemyStateMachine(EnemyBoss& boss);
	~EnemyStateMachine();

	// 行動パターンの追加
	void Add(EnemyBoss::STATE stateId, std::shared_ptr<IEnemyState> stateInstance);
	
	// 行動パターン変更
	void ChangeState(EnemyBoss::STATE newState);
	
	// 更新処理
	void Update(EnemyBoss& boss);
	
	// 現在の行動ステートIDの取得
	EnemyBoss::STATE GetCurrentStateId() const;

	// デバック表示用
	void DrawDebugInfo(const VECTOR& screenPos);

private:
	EnemyBoss& boss_;
	std::map<EnemyBoss::STATE, std::shared_ptr<IEnemyState>> states_;	// 行動パターンとその行動ステートクラスを格納する
	std::shared_ptr<IEnemyState> curState_;								// 現在の行動パターン
	EnemyBoss::STATE curStateId_;										// 現在の行動パターンID

	static const std::map<EnemyBoss::STATE, const char*> stateNames_;	// デバック用
};



