#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class ClearScene : public SceneBase
{

public:
	static constexpr int CLEAR_BGM_VOL = 180;   // クリア時のBGM音量
	static constexpr int ENTER_SE_VOL = 255;    // 決定音の音量

	static constexpr int PARTICLE_COUNT = 70;            // パーティクルの総数
	static constexpr float PARTICLE_VX_CENTER = 0.5f;    // パーティクルのX方向の基本速度
	static constexpr float PARTICLE_VX_RANGE = 0.2f;     // X方向速度のランダム幅
	static constexpr float PARTICLE_VY_BASE = 0.2f;      // Y方向の基準速度
	static constexpr int PARTICLE_VX_RAND_MAX = 100;     // X方向乱数の最大値
	static constexpr float PARTICLE_VX_DIVISOR = 100.0f; // X方向速度計算用の除数
	static constexpr int PARTICLE_VY_RAND_MAX = 10;      // Y方向乱数の最大値
	static constexpr float PARTICLE_VY_DIVISOR = 50.0f;  // Y方向速度計算用の除数
	static constexpr int PARTICLE_SIZE_MIN = 1;          // パーティクルの最小サイズ
	static constexpr int PARTICLE_SIZE_VARIATION = 3;    // パーティクルサイズの変動幅
	static constexpr int PARTICLE_ALPHA_MIN = 100;       // パーティクルの最小透過度
	static constexpr int PARTICLE_ALPHA_VARIATION = 100; // パーティクル透過度の変動幅

	static constexpr float PHASE_INCREMENT = 0.05f; // 演出の位相の進み幅

	static constexpr float CLEAR_IMAGE_SCALE = 0.7f;   // クリア画像のスケール倍率
	static constexpr int HINT_FONT_SIZE = 32;          // ヒント文字サイズ
	static constexpr int HINT_Y = 480;                 // ヒント表示のY座標
	static constexpr int HINT_BG_ALPHA = 150;          // ヒント背景の透明度
	static constexpr int HINT_BG_PADDING_X = 20;       // ヒント背景の横方向余白
	static constexpr int HINT_BG_PADDING_Y = 5;        // ヒント背景の縦方向余白
	static constexpr int HINT_BG_HEIGHT = 35;          // ヒント背景の高さ
	static constexpr int HINT_SHADOW_OFFSET = 2;       // ヒント文字の影のオフセット
	static constexpr int COLOR_HINT_TEXT_R = 255;      // ヒント文字の色R
	static constexpr int COLOR_HINT_TEXT_G = 230;      // ヒント文字の色G
	static constexpr int COLOR_HINT_TEXT_B = 180;      // ヒント文字の色B
	static constexpr int CLEAR_FONT_SIZE_SMALL = 16;   // 小さいサイズの文字

	static constexpr int COLOR_PARTICLE_R = 220;   // パーティクルの色R
	static constexpr int COLOR_PARTICLE_G = 220;   // パーティクルの色G
	static constexpr int COLOR_PARTICLE_B = 220;   // パーティクルの色B

	ClearScene(void);	// コンストラクタ

	~ClearScene(void);	// デストラクタ

	void Init(void) override;	// 初期化処理
	void Update(void) override;	// 毎フレームの更新処理
	void Draw(void) override;	// 描画処理
	void DrawUI(void) override;	// 描画処理(UI)
	void UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)override;	// カメラの更新処理

private:
	int clearBgm_;   // クリア時に流れるBGMのハンドル
	bool isBgm_;     // BGMが再生中かどうかを判定するフラグ
	int enterSe_;    // 決定キーなどを押した時のSEハンドル

	int imgClear_;   // 「CLEAR」などのクリア画像のハンドル

	void DrawParticles(void);  // クリア画面で舞うパーティクルを描画する関数

	float phase_;    // サイン波的なエフェクトや点滅制御に使う位相値

	struct Particle {
		float x, y;   // 位置座標
		float vx, vy; // 移動速度（x方向, y方向）
		int size;     // パーティクルの大きさ
		int alpha;    // パーティクルの透明度（不透明度）
	}

	particles_[PARTICLE_COUNT];  // 複数のパーティクルを保持する配列


};
