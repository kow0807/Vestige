#pragma once
#include "../IEnemyState.h"
class EStunState 
	: public IEnemyState
{

public:

	EStunState(void);
	~EStunState(void);

	void Enter(EnemyBoss& boss)override;
	void Update(EnemyBoss& boss)override;
	void Exit(EnemyBoss& boss)override;

private:

};

