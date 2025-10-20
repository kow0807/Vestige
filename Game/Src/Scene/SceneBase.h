#pragma once
#include "../Manager/Camera.h"
class ResourceManager;

class SceneBase
{

public:

	// コンストラクタ
	SceneBase(void);

	// デストラクタ
	virtual ~SceneBase(void) = 0;

	// 初期化処理
	virtual void Init(void) = 0;

	// 更新ステップ
	virtual void Update(void) = 0;

	// 描画処理
	virtual void Draw(void) = 0;
	virtual void DrawUI(void) = 0;

	// カメラオブジェクトを引数にとり、
	//カメラの更新を委譲する
	virtual void UpdateCamera(VECTOR& pos,VECTOR& targetPos,VECTOR& cameraUp) = 0;

protected:

	// リソース管理
	ResourceManager& resMng_;

};
