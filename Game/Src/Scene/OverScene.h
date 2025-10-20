#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class OverScene : public SceneBase
{

public:
	static constexpr int OVER_BGM_VOL = 180;	// ゲームオーバー時のBGM音量
	static constexpr int ENTER_SE_VOL = 255;	// 決定音の音量

	static constexpr int PARTICLE_COUNT = 70;				// パーティクルの総数
	static constexpr float PARTICLE_VX_CENTER = 0.5f;		// パーティクルのX方向の基準速度
	static constexpr float PARTICLE_VX_RANGE = 0.2f;		// パーティクルのX速度のばらつき範囲
	static constexpr float PARTICLE_VY_BASE = 0.2f;			// パーティクルのY方向の基準速度
	static constexpr int PARTICLE_VX_RAND_MAX = 100;		// X速度乱数の上限
	static constexpr float PARTICLE_VX_DIVISOR = 100.0f;	// X速度乱数の割り算係数
	static constexpr int PARTICLE_VY_RAND_MAX = 10;			// Y速度乱数の上限
	static constexpr float PARTICLE_VY_DIVISOR = 50.0f;		// Y速度乱数の割り算係数
	static constexpr int PARTICLE_SIZE_MIN = 1;				// パーティクルの最小サイズ
	static constexpr int PARTICLE_SIZE_VARIATION = 3;		// パーティクルサイズのばらつき幅
	static constexpr int PARTICLE_ALPHA_MIN = 100;			// パーティクルの最小透明度
	static constexpr int PARTICLE_ALPHA_VARIATION = 100;	// パーティクル透明度のばらつき幅

	static constexpr float OVER_IMAGE_SCALE = 0.7f;			// 「GAME OVER」画像の表示スケール倍率
	static constexpr int HINT_FONT_SIZE = 32;				// ヒントテキストのフォントサイズ
	static constexpr int HINT_Y = 480;						// ヒントテキストの表示位置
	static constexpr int HINT_BG_ALPHA = 150;				// ヒント背景の透明度
	static constexpr int HINT_BG_PADDING_X = 20;			// ヒント背景の横方向の余白
	static constexpr int HINT_BG_PADDING_Y = 5;				// ヒント背景の縦方向の余白
	static constexpr int HINT_BG_HEIGHT = 35;				// ヒント背景の高さ
	static constexpr int HINT_SHADOW_OFFSET = 2;			// テキストの影のずらし量
	static constexpr int COLOR_HINT_TEXT_R = 255;			// ヒント文字の色R
	static constexpr int COLOR_HINT_TEXT_G = 230;			// ヒント文字の色G
	static constexpr int COLOR_HINT_TEXT_B = 180;			// ヒント文字の色B
	static constexpr int OVER_FONT_SIZE_SMALL = 16;			// 小さいサイズのフォント

	static constexpr int COLOR_PARTICLE_R = 220;			// パーティクル色R
	static constexpr int COLOR_PARTICLE_G = 220;			// パーティクル色G
	static constexpr int COLOR_PARTICLE_B = 220;			// パーティクル色B


	OverScene(void);	// コンストラクタ

	~OverScene(void);	// デストラクタ

	void Init(void) override;	// 初期化処理
	void Update(void) override;	// 毎フレームの更新処理
	void Draw(void) override;	// 描画処理
	void DrawUI(void) override;	// 描画処理(UI)
	void UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)override;	// カメラの更新処理


private:
	// 音
	int overBgm_;	// ゲームオーバー時に流れるBGMのハンドル
	bool isBgm_;     // BGMが再生中かどうかを判定するフラグ
	int enterSe_;    // 決定キーなどを押した時のSEハンドル

	void DrawParticles(void);	// クリア画面で舞うパーティクルを描画する関数

	int imgOver_;	// 画像


	struct Particle {
		float x, y;   // 位置座標
		float vx, vy; // 移動速度（x方向, y方向）
		int size;     // パーティクルの大きさ
		int alpha;    // パーティクルの透明度（不透明度）
	}

	particles_[PARTICLE_COUNT];  // 複数のパーティクルを保持する配列
};
