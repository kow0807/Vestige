#pragma once
#include "../ActorBase.h"
class ResourceManager;
class SceneManager;
class Player;

class PlayerDummy :public ActorBase
{
public:

	static constexpr float SIZE = 1.5f;		// サイズ

	PlayerDummy(Player& playerDummy);	// コンストラクタ

	~PlayerDummy(void);	// デストラクタ

	void Init(void) override;						// 初期化処理
	void Update(void) override;						// 毎フレーム更新処理
	void Draw(void) override;						// 描画処理

private:
	Player& player_;

};

