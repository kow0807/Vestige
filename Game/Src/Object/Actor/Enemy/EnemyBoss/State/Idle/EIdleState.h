#pragma once
#include "../IEnemyState.h"
class EIdleState 
	: public IEnemyState
{

public:

	EIdleState(void);
	~EIdleState(void);

	void Enter(EnemyBoss& boss)override;
	void Update(EnemyBoss& boss)override;
	void Exit(EnemyBoss& boss)override;
};

