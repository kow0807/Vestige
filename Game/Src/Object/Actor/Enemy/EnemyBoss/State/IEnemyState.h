#pragma once
class EnemyBoss;

class IEnemyState
{

public:
	virtual ~IEnemyState(void) = default;

	// ‰Šú‰»ˆ—
	virtual void Enter(EnemyBoss& boss) = 0;

	// XVˆ—
	virtual void Update(EnemyBoss& boss) = 0;
	
	// I—¹ˆ—
	virtual void Exit(EnemyBoss& boss) = 0;

protected:

	// ŠÔ
	float time_;
};

