#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
class SceneManager;
class SkyDome;
class AnimationController;

class TitleScene : public SceneBase
{

public:

	struct Particle {
		float x, y;       // 位置
		float vx, vy;     // 移動速度（左右ゆらぎ＆下方向）
		int size;         // 粒の大きさ（1～3）
		int alpha;        // 透明度（100～200くらい）
	};

	static constexpr int TITLE_BGM_VOL = 180;	// タイトル画面BGMの音量
	static constexpr int CLICK_SE_VOL = 255;	// クリック音の音量

	static constexpr int PARTICLE_COUNT = 70;				// パーティクルの総数
	static constexpr float PARTICLE_VX_CENTER = 0.5f;		// X方向基準速度
	static constexpr float PARTICLE_VX_RANGE = 0.2f;		// X方向速度のばらつき
	static constexpr float PARTICLE_VY_BASE = 0.2f;			// Y方向基準速度
	static constexpr int PARTICLE_VX_RAND_MAX = 100;		// X速度乱数上限
	static constexpr float PARTICLE_VX_DIVISOR = 100.0f;	// X速度乱数調整用割り算
	static constexpr int PARTICLE_VY_RAND_MAX = 10;			// Y速度乱数上限
	static constexpr float PARTICLE_VY_DIVISOR = 50.0f;		// Y速度乱数調整用割り算
	static constexpr int PARTICLE_SIZE_MIN = 1;				// パーティクル最小サイズ
	static constexpr int PARTICLE_SIZE_VARIATION = 3;		// パーティクルサイズのばらつき
	static constexpr int PARTICLE_ALPHA_MIN = 100;			// パーティクル最小透明度
	static constexpr int PARTICLE_ALPHA_VARIATION = 100;	// パーティクル透明度のばらつき

	static constexpr int INIT_MENU_HIGHLIGHT_Y = 460;		// メニュー初期選択位置のY座標

	static constexpr float STICK_THRESHOLD = 0.5f;		// スティック反応閾値
	static constexpr int POV_UP = 0;					// POV上方向
	static constexpr int POV_RIGHT = 9000;				// POV右方向
	static constexpr int POV_DOWN = 18000;				// POV下方向
	static constexpr int POV_LEFT = 27000;				// POV左方向
	static constexpr float STICK_SCALE = 1000.0f;		// スティック入力スケーリング係数

	static constexpr int CONFIRM_OPTION_COUNT = 2;		// 確認ダイアログの選択肢数
	static constexpr int MENU_ITEM_COUNT = 2;			// メニュー項目数
	static constexpr int OPTION_PADDING = 40;			// メニューオプション間の余白
	static constexpr int OPTION_Y_POS = 440;			// オプション開始Y座標
	static constexpr int OPTION_HEIGHT = 34;			// オプション1行の高さ

	static constexpr int FONT_SIZE_DEFAULT = 16;		// デフォルトフォントサイズ
	static constexpr int FONT_SIZE_CONFIRM = 28;		// 確認用フォントサイズ
	static constexpr int FONT_SIZE_MENU = 36;			// メニューフォントサイズ
	static constexpr int MENU_TEXT_HEIGHT = 36;			// メニューテキスト高さ
	static constexpr int MENU_BASE_Y = 460;				// メニュー基準Y座標
	static constexpr int MENU_ITEM_INTERVAL = 60;		// メニュー項目間隔
	static constexpr int MENU_PADDING_X = 20;			// メニュー横余白
	static constexpr int MENU_PADDING_Y = 10;			// メニュー縦余白

	static constexpr float CONFIRM_LERP_SPEED = 0.2f;	// 確認ダイアログ選択移動速度
	static constexpr float MENU_LERP_SPEED = 0.15f;		// メニュー選択移動速度

	static constexpr int TITLE_IMAGE_POS_Y = 250;		// タイトル画像Y座標
	static constexpr int CONFIRM_QUESTION_Y = 380;		// 確認ダイアログ質問文Y座標
	static constexpr int HIGHLIGHT_BOX_MARGIN_X = 10;	// 選択ハイライトボックスX余白
	static constexpr int HIGHLIGHT_BOX_MARGIN_Y = 6;	// 選択ハイライトボックスY余白
	static constexpr int SHADOW_OFFSET = 2;				// 文字影のずらし量
	static constexpr float GLOW_OFFSET_Y = 2.0f;		// メニューグロー効果Yずれ

	static constexpr int ALPHA_MENU_HIGHLIGHT = 120;	// 選択中メニューの透明度
	static constexpr int ALPHA_MENU_NORMAL = 100;		// 非選択メニューの透明度

	static constexpr int COLOR_MENU_HIGHLIGHT_R = 255;	// 選択中メニュー文字R
	static constexpr int COLOR_MENU_HIGHLIGHT_G = 230;	// 選択中メニュー文字G
	static constexpr int COLOR_MENU_HIGHLIGHT_B = 100;	// 選択中メニュー文字B
	static constexpr int COLOR_MENU_NORMAL_R = 160;		// 非選択メニュー文字R
	static constexpr int COLOR_MENU_NORMAL_G = 160;		// 非選択メニュー文字G
	static constexpr int COLOR_MENU_NORMAL_B = 160;		// 非選択メニュー文字B
	static constexpr int COLOR_HIGHLIGHT_BG_R = 255;	// 選択ハイライト背景R
	static constexpr int COLOR_HIGHLIGHT_BG_G = 255;	// 選択ハイライト背景G
	static constexpr int COLOR_HIGHLIGHT_BG_B = 180;	// 選択ハイライト背景B
	static constexpr int COLOR_TEXT_SHADOW_R = 0;		// テキスト影色R
	static constexpr int COLOR_TEXT_SHADOW_G = 0;		// テキスト影色G
	static constexpr int COLOR_TEXT_SHADOW_B = 0;		// テキスト影色B
	static constexpr int COLOR_TEXT_MAIN_R = 255;		// テキスト主色R
	static constexpr int COLOR_TEXT_MAIN_G = 255;		// テキスト主色G
	static constexpr int COLOR_TEXT_MAIN_B = 255;		// テキスト主色B
	static constexpr int COLOR_SELECTED_BG_R = 255;		// 選択中背景色R
	static constexpr int COLOR_SELECTED_BG_G = 230;		// 選択中背景色G
	static constexpr int COLOR_SELECTED_BG_B = 160;		// 選択中背景色B

	static constexpr int COLOR_PARTICLE_R = 220;		// パーティクル色R
	static constexpr int COLOR_PARTICLE_G = 220;		// パーティクル色G
	static constexpr int COLOR_PARTICLE_B = 220;		// パーティクル色B

	TitleScene(void);	// コンストラクタ

	~TitleScene(void);	// デストラクタ

	void Init(void) override;	// 初期化処理
	void Update(void) override;	// 毎フレームの更新処理
	void Draw(void) override;	// 描画処理
	void DrawUI(void) override;	// 描画処理(UI)
	void UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)override;	// カメラの更新処理


private:

	int imgTitle_;		// タイトル画像
	int titleBgm_;		// タイトルBGM
	int clickSe_;		// クリック音

	Particle particles_[PARTICLE_COUNT];	// パーティクルの配列

	int menuSelected_;			// 現在選択中のメニューインデックス
	const char* menuItems_[2];	// メニュー項目の文字列配列
	float menuEffectPhase_;		// メニュー選択時のエフェクト位相

	bool isConfirmingTutorial_;  // チュートリアル確認中かどうか
	int confirmSelected_;        // 確認選択肢インデックス

	int prevPOVDirection_;       // 前フレームのPOV方向

	float highlightPosY_;          // メニュー選択ハイライトの現在Y座標
	float confirmHighlightPosX_;   // 確認ダイアログハイライトのX座標
	float confirmHighlightPosY_;   // 確認ダイアログハイライトのY座標

	void DrawParticles(void);	// パーティクルを描画する関数

};
