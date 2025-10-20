#pragma once
#include "../IEnemyState.h"
class EMoveState 
	: public IEnemyState
{

public:

	
	static constexpr float TARGET_DISTANCE = 350.0f;	// 目標とするプレイヤーからの距離
	static constexpr float MOVE_SPEED = 2.0f;			// 移動速度
	static constexpr float DISTANCE_TOLERANCE = 50.0f;	// 目標距離からの許容誤差

	EMoveState(void);
	~EMoveState(void);

	void Enter(EnemyBoss& boss)override;
	void Update(EnemyBoss& boss)override;
	void Exit(EnemyBoss& boss)override;

private:

};

