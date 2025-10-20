#pragma once
#include "../IEnemyState.h"
#include <DxLib.h>
class EStepState 
	: public IEnemyState
{

public:

	// バックステップの移動時間
	static constexpr float DURATION = 0.7f;

	// 一度目の移動距離
	static constexpr float FIRST_DIS = 10.0f;

	// 二度目の移動距離
	static constexpr float SECOND_DIS = 13.0f;
	
	// ステップの回数
	static constexpr int STEP_MAX_COUNT = 2;
	
	// 円周率
	static constexpr float PI = 3.14159f;

	EStepState(void);
	~EStepState(void);

	void Enter(EnemyBoss& boss)override;
	void Update(EnemyBoss& boss)override;
	void Exit(EnemyBoss& boss)override;

private:

	// ステップの回数
	int stepCount_;

	// ステップステート中の時間
	float backStepTimer_;

	// 高さ
	float height_;

	// 移動計算
	VECTOR CalculateBackstepPosition(const VECTOR& startPos, const VECTOR& dir, float elapsedTime, float duration, float distance, float height);
};

