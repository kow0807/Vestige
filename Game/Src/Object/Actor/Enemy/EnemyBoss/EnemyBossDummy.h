#pragma once
#include "../../../../Object/Common/Transform.h"
class ResourceManager;
class SceneManager;

class EnemyBoss;

class EnemyBossDummy
{
public:
	// サイズ
	static constexpr float SIZE = 1.5f;

	EnemyBossDummy(EnemyBoss& enemyBoss);

	~EnemyBossDummy(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	const Transform& GetTransform(void) const;

private:
	EnemyBoss& enemyBoss_;

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	// モデル制御の基本情報
	Transform transform_;

};

