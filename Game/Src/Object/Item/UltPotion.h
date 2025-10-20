#pragma once
#include "ItemBase.h"
class UltPotion :
	public ItemBase
{
public:
	static constexpr int UI_LEFT = 55;		// UIの左端X座標
	static constexpr int UI_TOP = 450;		// UIの上端Y座標
	static constexpr int UI_RIGHT = 145;	// UIの右端X座標
	static constexpr int UI_BOTTOM = 580;	// UIの下端Y座標

	UltPotion(void);	// コンストラクタ

	~UltPotion(void);	// デストラクタ

	void Init(void) override;	// 初期化処理
	void Update(void) override;	// 毎フレームの更新処理
	void Draw(void) override;	// 描画処理

	void Use(void);				// 使用時の処理
	int GetCount(void);			// 使用回数や残数を取得
private:
	int ultPotionSe_;			// 使用時の音
};

