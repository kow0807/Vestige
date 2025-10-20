#pragma once
#include "../Common/Transform.h"
class ResourceManager;
class SceneManager;

class ItemBase
{
public:

	ItemBase(void);					// コンストラクタ

	virtual ~ItemBase(void);		// デストラクタ

	virtual void Init(void) = 0;	// 初期化処理
	virtual void Update(void) = 0;	// 毎フレームの更新処理
	virtual void Draw(void) = 0;	// 描画処理

	const Transform& GetTransform(void) const;

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	// モデル制御の基本情報
	Transform transform_;

	int itemImg_;
	int count_;

};

