#pragma once
#include "../IEnemyState.h"
class EAttackState 
	: public IEnemyState
{

public:

	EAttackState(void);
	~EAttackState(void);

	void Enter(EnemyBoss& boss) override;
	void Update(EnemyBoss& boss) override;
	void Exit(EnemyBoss& boss)override;

private:

	// ˜A‘±UŒ‚‚ğŠJn‚·‚é‚©‚Ç‚¤‚©
	bool isContinuous_;
};

