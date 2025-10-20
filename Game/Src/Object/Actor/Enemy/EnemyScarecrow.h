#pragma once
#include "../../../Utility/AsoUtility.h"
#include "../ActorBase.h"

class Player;

class EnemyScarecrow :
    public ActorBase
{

public:

    static constexpr float SIZE = 4.0f;											// モデルの拡大率
	static constexpr VECTOR INIT_POS = { 0.0f, 70.0f, 660.0f };					// 初期座標
	const VECTOR INIT_LOCAL_ROT = { 0.0f,AsoUtility::Deg2RadF(90.0f),0.0f };	// 初期ローカル回転

	
    static constexpr int MAX_HP = 1500;											// HP
	static constexpr float HP_RATE = 1.0f;										// HP比率

	
	static constexpr float FOLLOW_SPEED = 0.005f;								// 減速ゲージのスピード

	// HPバー関連
	static constexpr int WIDTH = 650;											// HPバーの幅
	static constexpr int HEIGHT = 20;											// HPバーの高さ
	static constexpr int X_ADJUSTMENT = 60;										// x座標の調整用
	static constexpr int Y_ADJUSTMENT = 80;										// y座標の調整用
	static constexpr int FONT_SIZE = 20;										// フォントのサイズ
	static constexpr int NAME_HEIGHT = 24;										// 名前テキストの高さ
	static constexpr int PADDING_LEFT = 8;										// テキストをx軸方向に調整する
	static constexpr int PADDING = 8;											// テキストの調整
	static constexpr int NAME_BLEND_PARAM = 160;								// 名前テキストのブレンド調整用
	static constexpr int MAX_COL = 255;											// 色の最大値
	static constexpr int TEXCOL_R = 255;										// テキストカラーのレッドの最大値
	static constexpr int TEXCOL_G = 255;										//テキストカラーのグリーンの最大値
	static constexpr int TEXCOL_B = 255;										// テキストカラーのブルーの最大値
	static constexpr int A_QUARTER_COL = 60;									// テキストカラーの最大の4分の一の値
	static constexpr int DELAYED_COL_LIGHT_R = 255;								// 遅れて変わる色のレッドの値
	static constexpr int DELAYED_COL_LIGHT = 200;								// 遅れて変わる色のグリーンとブルーの値
	static constexpr int DELAYED_COL_TOP_R = 255;								// 遅れて変わる色の最初のレッドの値
	static constexpr int DELAYED_COL_TOP_GB = 100;								// 遅れて変わる色の最初のグリーンとブルーの値
	static constexpr int DELAYED_COL_BOTTOM_R = 200;							// 遅れて変わる色の中間のレッドの値
	static constexpr int DELAYED_COL_BOTTOM_GB = 50;							// 遅れて変わる色の中間のグリーンとブルーの値
	static constexpr int HP_COL_TOP_R = 255;									// HPバー色の上端のレッドの値	
	static constexpr int HP_COL_TOP_GB = 80;									// HPバー色の上端のグリーンとブルーの値
	static constexpr int HP_COL_BOTTOM_R = 180;									// HPバー色の下端のレッドの値
	static constexpr int HP_COL_BOTTOM_GB = 40;									// HPバー色の下端のグリーンとブルーの値
	static constexpr int BORDER_DARK_COL = 40;									// 暗い境界線の色
	static constexpr int BORDER_LIGHT_COL = 200;								// 明るい境界線の色
	static constexpr int DAMAGE_BLEND_PARAM = 180;								// ダメージを受けた時のブレンドパラメータ
	static constexpr int BORDER_DARK_ADJUSTMENT = 2;							// 暗い境界線の調整用定数
	static constexpr int BORDER_LIGHT_ADJUSTMENT = 1;							// 明るい境界線の調整用定数

	EnemyScarecrow(Player& player);
	~EnemyScarecrow(void);

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;

	// HPバーの描画
	void DrawHpBar(void);

	void Damage(int damage);
	bool IsAlive(void);
private:
	// プレイヤー
	Player& player_;

	int hp_;
	float delayedHpRate_;

	bool isAlive_;
};

