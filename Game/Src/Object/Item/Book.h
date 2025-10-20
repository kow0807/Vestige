#pragma once
#include "ItemBase.h"

class Book : public ItemBase
{
public:

	static constexpr int UI_LEFT = 55;		// UIの左端X座標
	static constexpr int UI_TOP = 450;		// UIの上端Y座標
	static constexpr int UI_RIGHT = 145;	// UIの右端X座標
	static constexpr int UI_BOTTOM = 580;	// UIの下端Y座標

	Book(void);					// コンストラクタ
	~Book(void);				// デストラクタ

	void Init(void) override;	// 初期化処理
	void Update(void) override;	// 毎フレームの更新処理
	void Draw(void) override;	// 描画処理

	void Use(void);				// 使用時の処理

	int GetCount(void);			// 使用回数や残数を取得

	void LightningUpdate(VECTOR pos);		// 落雷エフェクトの更新処理

private:

	int bookSe_;		// 本を使うときの音

	int lightningSe_;	// 落雷発生時の音

	int effectLightningPlayId_;  // 再生中の落雷エフェクトID
	int effectLightningResId_;   // 落雷エフェクトのリソースID

};

