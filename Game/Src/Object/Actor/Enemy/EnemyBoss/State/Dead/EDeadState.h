#pragma once
#include "../IEnemyState.h"
class EDeadState 
	: public IEnemyState
{

public:

	EDeadState(void);
	~EDeadState(void);

	void Enter(EnemyBoss& boss)override;
	void Update(EnemyBoss& boss)override;
	void Exit(EnemyBoss& boss)override;

private:

	bool deathAnimationStarted_;			// 死亡アニメーションが再生されたかどうか
	bool deathAnimationFinishedReported_;	// 死亡アニメーションが終了されたかどうか
	float deathAnimationDuration_;			// 死亡アニメーションの再生時間
};

