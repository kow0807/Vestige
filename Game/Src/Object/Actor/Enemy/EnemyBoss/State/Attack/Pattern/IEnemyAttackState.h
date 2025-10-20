#pragma once
class EnemyBoss;

class IEnemyAttackState
{

public:

	virtual ~IEnemyAttackState(void) = default;

	//	攻撃開始時の初期化
	virtual void Enter(EnemyBoss& boss) = 0;

	// 更新処理
	virtual void Update(EnemyBoss& boss) = 0;			
	
	// 攻撃の終了処理
	virtual void Exit(EnemyBoss& boss) = 0;

	// 攻撃終了フラグ
	virtual bool IsFinished(void) const = 0;

	// エフェクトの停止
	virtual void StopEffect(void) = 0;

protected:

	float timer_;			// 時間
	bool isFinished_;		// 攻撃が終了したかどうか
	int noHitSound_;		// サウンドハンドル
	bool isSoundPlayed_;	// 再生するかどうか

	// 音の初期化
	virtual void InitSound(void) = 0;
};


