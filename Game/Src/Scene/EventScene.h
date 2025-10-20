#pragma once
#include <memory>
#include <string>
#include "SceneBase.h"
class EventStage;
class SkyDome;
class EventPlayer;
class EventEnemy;
class Mist;

class EventScene : public SceneBase
{
public:

	// フォント関連
	static constexpr int FONT_SIZE = 24;					// フォントサイズ
	static constexpr int FONT_THICK = 6;					// フォントの太さ
	static constexpr int FONT_MAX_COL = 255;				// フォントの色の最大値
	static constexpr int FONT_GRAY_COL = 50;				// フォントの灰色
	static constexpr int BAR_HEIGHT = 20;					// プログレスバーの高さ
	static constexpr int BAR_MAX_WIDTH = 200;				// プログレスバーの最大幅
	static constexpr int BAR_X_ADJUSTMOMENT = 220;			// プログレスバーのX位置調整
	static constexpr int BAR_Y_ADJUSTMOMENT = 32;			// プログレスバーのY位置調整
	static constexpr int PROGRESS_COL_R = 125;				// プログレスバーのレッドの値
	static constexpr int PROGRESS_COL_G = 252;				// プログレスバーのグリーンの値
	static constexpr int PROGRESS_COL_B = 0;				// プログレスバーのブルーの値



	static constexpr int FONT_MARGIN_RIGHT = 30;			// フォントの右余白
	static constexpr int FONT_MARGIN_UNDER = 64;			// フォントの下余白

	// カメラワーク関連
	static constexpr float START_TIME_LIMIT = 1.5f;			// 最初にプレイヤーが歩く時間
	static constexpr float LOOK_TIME_LIMIT = 4.0f;			// プレイヤーが周りを見渡す時間
	static constexpr float PAN_TIME_LIMIT_FIRST = 0.5f;		// カメラが敵にパンするまでの時間
	static constexpr float PAN_TIME_LIMIT_SECOND = 2.0f;	// プレイヤーが動いたとみなす距離

	static constexpr float PLAYER_MOVED_LIMIT = 300.0f;		// プレイヤーが動いたとみなす距離

	// スキップ機能の待機時間
	static constexpr int LONG_PRESS_DURATION_MS = 2000;		// 長押しとみなす時間(ミリ秒)

	// サウンド関連
	static constexpr int BGM_VALUME = 255;					// BGM音量
	static constexpr int INPUTKEY_VALUME = 255;				// キー入力音量
	static constexpr int COUNTMAX_VALUME = 255;				// カウントマックス音量

	// イベントシーン用カメラ関連
	static constexpr VECTOR EVENT_CAMERA_POS = { -50.0f, -60.0f, 260.0f };			// カメラ初期座標
	static constexpr VECTOR EVENT_TARGET_POS = { 0.0f, 80.0f, 0.0f };				// カメラ注視点初期座標

	static constexpr VECTOR EVENT_START_CAMERA_POS = { -50.0f, -60.0f, 260.0f };	// カメラ初期座標
	static constexpr VECTOR EVENT_START_TARGET_POS = { 0.0f, 80.0f, 0.0f };			// カメラ注視点初期座標

	static constexpr VECTOR EVENT_STEP_CAMERA_POS = { -50.0f, 30.0f, 500.0f };		// プレイヤーが歩くときのカメラ座標
	static constexpr VECTOR EVENT_STEP_TARGET_POS = { -100.0f, 30.0f, 200.0f };		// プレイヤーが歩くときのカメラ注視点座標

	static constexpr float EVENT_LOOK_CAMER_POS_X = 50.0f;							// プレイヤーが周りを見るときのカメラ座標
	static constexpr float EVENT_LOOK_CAMER_POS_Y = 120.0f;							// プレイヤーが周りを見るときのカメラ座標
	static constexpr float EVENT_LOOK_CAMER_POS_Z = 300.0f;							// プレイヤーが周りを見るときのカメラ座標
	static constexpr VECTOR EVENT_LOOK_TARGET_POS = { 20.0f, 50.0f, 0.0f };			// プレイヤーが周りを見るときのカメラ注視点座標

	static constexpr VECTOR EVENT_PAN_CAMERA_START_POS = { 0.0f, 80.0f, 0.0f };		// カメラが敵にパンする時の開始座標
	static constexpr VECTOR EVENT_PAN_CAMERA_END_POS = { 0.0f, 50.0f, 300.0f };		// カメラが敵にパンする時の終了座標
	static constexpr VECTOR EVENT_PAN_ENEMY_POS = { 0.0f, 100.0f, 0.0f };			// カメラが敵にパンする時の注視点座標


	static constexpr float EVENT_ROAR_CAMERA_POS_X = -30.0f;						// 敵が咆哮する時のカメラ座標
	static constexpr float EVENT_ROAR_CAMERA_POS_Y = 25.0f;							// 敵が咆哮する時のカメラ座標
	static constexpr float EVENT_ROAR_TIME_ADJUSTMOMENT = 40.0f;					// 敵が咆哮する時のカメラ座標
	static constexpr float EVENT_ROAR_CAMERA_POS_Z = -200.0f;						// 敵が咆哮する時のカメラ座標
	static constexpr VECTOR EVENT_ROAR_ENEMY_POS = { 0.0f, 80.0f, 0.0f };			// 敵が咆哮する時のカメラ注視点座標


	enum class STATE {
		START,
		STEP_PLAYER_FORWARD,
		LOOK_AROUND,
		CAMERA_PAN_TO_ENEMY,
		ENEMY_ROAR,
	};

	// コンストラクタ
	EventScene(void);

	// デストラクタ
	~EventScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void DrawUI(void) override;
	void UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)override;

	// 現在の状態が開始フレームかどうか
	bool IsStateEnterFrame(void) const;

private:

	// 現在の状態
	STATE currentState_;

	// 状態遷移タイマー
	float stateTimer_;

	// ステージ
	std::shared_ptr<EventStage> stage_;

	// 霧
	std::shared_ptr<Mist> mist_;

	// スカイドーム
	std::unique_ptr<SkyDome> skyDome_;

	// プレイヤー
	std::shared_ptr<EventPlayer> player_;

	// 敵
	std::shared_ptr<EventEnemy> enemy_;

	// 状態の名前取得用(デバック用)
	std::string GetStateName(STATE state);

	// 長押し関連
	bool isLongPressing = false;
	int longPressStartTime_; // 長押し開始時刻 (DxLibのGetNowCount()で取得)

	bool isInitialized_;	// 初期化されるかどうか

	// プログレスバーUIの色
	int progressColor; // 緑

	// フォントハンドル
	int customFontHandle_;

	void DrawProgressBar(int elapsedTime, int totalDuration);

	void InitSound(void);

	// サウンドハンドル
	int bgmSHandle_;			// メインサウンド
	int iKeySHandle_;			// 何かボタンが押されたときのサウンド
	int cMaxSHandle_;			// カウントがマックス値になったときのサウンド

	// 毎フレーム音を生成させないため
	bool isBgm_;

	// キーを押しているフレームが0かどうか
	bool isStateFrame_;
};