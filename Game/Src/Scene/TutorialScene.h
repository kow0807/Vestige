#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class Stage;
class Mist;
class SkyDome;
class Player;
class EnemyBoss;
class EnemyScarecrow;
class EnemyScarecrowDummy;
class PixelMaterial;
class PixelRenderer;

class TutorialScene :
	public SceneBase
{
public:

	static constexpr int TUTORIAL_BGM_VOL = 150;	// チュートリアルBGMの音量
	static constexpr int NEXT_SE_VOL = 220;			// 次のステップに進む時のSE音量
	static constexpr int HELP_SE_VOL = 220;			// ヘルプ表示時のSE音量

	static constexpr float MILLISECONDS_TO_SECONDS = 1000.0f;	// ミリ秒を秒に換算する係数
	static constexpr float TIME_TO_NEXT_SCENE = 5.0f;			// チュートリアル終了後、次のシーンへ移行するまでの時間（秒）

	static constexpr float LIGHTNING_POWER_MAX = 1.0f;		// 雷の最大強度
	static constexpr float LIGHTNING_DECAY_RATE = 0.05f;	// 雷の強度減衰率
	static constexpr int LIGHTNING_DAMAGE = 300;			// 雷攻撃によるダメージ

	static constexpr float TUTORIAL_BOX_WIDTH_RATIO = 0.41f;	// チュートリアルボックス幅（画面幅に対する比率）
	static constexpr int TUTORIAL_BOX_HEIGHT = 100;			// チュートリアルボックス高さ
	static constexpr int TUTORIAL_BOX_X = 20;				// チュートリアルボックスX座標
	static constexpr int TUTORIAL_BOX_CORNER_RADIUS = 14;	// チュートリアルボックス角丸半径
	static constexpr int BG_ALPHA = 120;						// 背景透明度
	static constexpr int BORDER_LAYER_COUNT = 3;			// 枠線レイヤー数
	static constexpr int BORDER_ALPHA_START = 50;			// 枠線の初期透明度
	static constexpr int BORDER_ALPHA_STEP = 15;			// 枠線透明度の階段ステップ
	static constexpr int BORDER_COLOR_R = 100;				// 外枠赤
	static constexpr int BORDER_COLOR_G = 20;				// 外枠緑
	static constexpr int BORDER_COLOR_B = 40;				// 外枠青
	static constexpr int INNER_BORDER_COLOR_R = 70;			// 内枠赤
	static constexpr int INNER_BORDER_COLOR_G = 70;			// 内枠緑
	static constexpr int INNER_BORDER_COLOR_B = 75;			// 内枠青

	static constexpr int MAIN_FONT_SIZE = 18;				// メインテキストサイズ
	static constexpr int SUB_FONT_SIZE = 16;				// サブテキストサイズ
	static constexpr int TEXT_LEFT_PADDING = 22;			// テキスト左余白
	static constexpr int TEXT_TOP_PADDING = 24;				// テキスト上余白
	static constexpr int SUBTEXT_SPACING = 10;				// サブテキスト行間
	static constexpr int SHADOW_OFFSET = 1;					// 影のオフセット1
	static constexpr int SHADOW_OFFSET_2 = 2;				// 影のオフセット2（複数レイヤー用）
	static constexpr int MAIN_TEXT_SHADOW_COLOR1_R = 15;	// メインテキスト影色1R
	static constexpr int MAIN_TEXT_SHADOW_COLOR1_G = 10;	// メインテキスト影色1G
	static constexpr int MAIN_TEXT_SHADOW_COLOR1_B = 20;	// メインテキスト影色1B
	static constexpr int MAIN_TEXT_SHADOW_COLOR2_R = 40;	// メインテキスト影色2R
	static constexpr int MAIN_TEXT_SHADOW_COLOR2_G = 30;	// メインテキスト影色2G
	static constexpr int MAIN_TEXT_SHADOW_COLOR2_B = 50;	// メインテキスト影色2B
	static constexpr int MAIN_TEXT_COLOR_R = 210;			// メインテキスト色R
	static constexpr int MAIN_TEXT_COLOR_G = 210;			// メインテキスト色G
	static constexpr int MAIN_TEXT_COLOR_B = 220;			// メインテキスト色B
	static constexpr int SUB_TEXT_SHADOW_COLOR_R = 40;		// サブテキスト影色R
	static constexpr int SUB_TEXT_SHADOW_COLOR_G = 35;		// サブテキスト影色G
	static constexpr int SUB_TEXT_SHADOW_COLOR_B = 50;		// サブテキスト影色B
	static constexpr int SUB_TEXT_COLOR_R = 170;			// サブテキスト色R
	static constexpr int SUB_TEXT_COLOR_G = 160;			// サブテキスト色G
	static constexpr int SUB_TEXT_COLOR_B = 180;			// サブテキスト色B
	static constexpr int ALPHA_SHIFT = 24;					// 文字やUIの透明度変化量

	static constexpr int LOOK_MOUSE_MOVE_THRESHOLD = 2;	// マウスで視点操作を判定する最小移動量
	static constexpr int LOOK_STICK_THRESHOLD = 100;	// スティックで視点操作を判定する最小移動量
	static constexpr int REQUIRED_LOOK_MOVES = 60;		// 視点移動を完了とみなす回数

	static constexpr int STICK_MOVE_THRESHOLD = 500;	// スティック移動量の閾値

	static constexpr float REQUIRED_RUN_TIME = 2.0f;	// ダッシュ/走行判定に必要な時間

	static constexpr float JUDGE_MARGIN = 1.0f;			// 判定余白（攻撃判定や操作判定用）

	static constexpr int PAD_BUTTON_LOCKON = 9;			// ロックオンボタン
	static constexpr int PAD_BUTTON_ULT = 4;			// ウルトラ攻撃ボタン
	static constexpr int TUTORIAL_DAMAGE = 10;			// チュートリアルで受ける通常ダメージ
	static constexpr int PAD_BUTTON_ITEM = 2;			// アイテム使用ボタン

	static constexpr int WHEEL_THRESHOLD = 3;			// マウスホイール操作判定閾値
	static constexpr int POV_RIGHT = 9000;				// POV右方向
	static constexpr int POV_LEFT = 27000;				// POV左方向

	static constexpr int HELP_FONT_SIZE = 18;			// ヘルプテキストフォントサイズ
	static constexpr int HELP_LINE_SPACING = 14;		// ヘルプ行間
	static constexpr int HELP_BOX_WIDTH = 660;			// ヘルプボックス幅
	static constexpr int HELP_TITLE_TOP_MARGIN = 40;	// ヘルプタイトル上余白
	static constexpr int HELP_PADDING = 20;				// ヘルプ内部余白
	static constexpr int HELP_CONTENT_TOP_OFFSET = 60;	// ヘルプコンテンツ開始Yオフセット
	static constexpr int HELP_LABEL_LEFT = 40;			// ラベル左位置
	static constexpr int HELP_INPUT_LEFT = 170;			// 入力説明左位置
	static constexpr int HELP_INPUT_OFFSET = 60;		// 入力説明オフセット

	static constexpr int COLOR_WHITE_R = 255;			// 白色R
	static constexpr int COLOR_WHITE_G = 255;			// 白色G
	static constexpr int COLOR_WHITE_B = 255;			// 白色B
	static constexpr int COLOR_COLOR_BLACK_R = 0;		// 黒色R
	static constexpr int COLOR_COLOR_BLACK_G = 0;		// 黒色G
	static constexpr int COLOR_COLOR_BLACK_B = 0;		// 黒色B

	static constexpr int CONST_BUF_FLOAT4_SIZE = 1;		// 定数バッファの確保サイズ

	enum class TutorialStep {
		LOOK,			// 視点操作
		MOVE,			// 移動操作
		DASH,			// ダッシュ
		ROLLING,		// 回避（ローリング）
		ATTACK,			// 攻撃
		COMBO_ATTACK,	// コンボ攻撃
		HEAVY_ATTACK,	// 強攻撃
		LOCKON,			// ロックオン
		ULT,			// ウルトラ攻撃
		ITEM,			// アイテム使用
		ITEM_CHANGE,	// アイテム切替
		HELP,			// ヘルプ表示
		DEFEAT_ENEMY,	// 敵撃破
		NEXT			// 次のステップへ
	};

	TutorialScene(void);	// コンストラクタ

	~TutorialScene(void);	// デストラクタ

	void Init(void) override;	// 初期化処理
	void Update(void) override;	// 毎フレームの更新処理
	void Draw(void) override;	// 描画処理
	void DrawUI(void) override;	// 描画処理(UI)
	void UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)override;	// カメラの更新処理

private:

	int tutorialBgm_;	// チュートリアルBGM
	bool isBgm_;		// BGMが再生中かどうか
	int nextSe_;		// 次ステップSE
	int helpSe_;		// ヘルプ表示SE

	std::unique_ptr<Stage> stage_;			// ステージ

	std::shared_ptr<Mist> mist_;			// ミスト
	std::unique_ptr<SkyDome> skyDome_;		// スカイドーム

	std::shared_ptr<Player> player_;							// プレイヤー
	std::shared_ptr<EnemyBoss> enemyBoss_;						// ボス敵
	std::shared_ptr<EnemyScarecrow> enemyScarecrow_;			// カカシ型敵
	std::shared_ptr<EnemyScarecrowDummy> enemyScarecrowDummy_;	// カカシのダミー敵

	TutorialStep tutorialStep_;	// 現在のチュートリアルステップ

	int lookCounter_;			// 視点移動判定用カウンタ
	int prevLookX_;				// 前フレームの視点X座標
	int prevLookY_;				// 前フレームの視点Y座標

	bool movedW_;				// Wキーで前進したか
	bool movedA_;				// Aキーで左移動したか
	bool movedS_;				// Sキーで後退したか
	bool movedD_;				// Dキーで右移動したか

	bool dashed_;				// ダッシュしたか
	bool rolled_;				// ローリングしたか
	bool attacked_;				// 攻撃したか
	bool comboAttacked_;		// コンボ攻撃したか
	bool heavyAttacked_;		// 強攻撃したか
	bool lockedOn_;				// ロックオンしたか
	bool usedUlt_;				// ウルトラ攻撃使用済みか
	bool usedItem_;				// アイテム使用済みか
	bool itemChanged_;			// アイテム切替済みか

	float nextStepTimer_;		// 次ステップ移行までのカウント

	void DrawTutorialText(const char* text, const char* subTex);	// メイン＋サブテキスト描画

	void LookUpdate(void);				// 視点操作更新
	void MoveUpdate(void);				// 移動操作更新
	void DashUpdate(void);				// ダッシュ操作更新
	void RollingUpdate(void);			// ローリング更新
	void AttackUpdate(void);			// 攻撃更新
	void ComboAttackUpdate(void);		// コンボ攻撃更新
	void HeavyAttackUpdate(void);		// 強攻撃更新
	void LockonUpdate(void);			// ロックオン更新
	void UltUpdate(void);				// ウルトラ攻撃更新
	void ItemUpdate(void);				// アイテム使用更新
	void ItemChangeUpdate(void);		// アイテム切替更新
	void HelpUpdate(void);				// ヘルプ表示更新
	void DefeatEnemyUpdate(void);		// 敵撃破更新

	int postEffectScreen_;								// ポストエフェクト描画用スクリーン
	std::unique_ptr<PixelMaterial> lightningMaterial_;	// ライトニング用マテリアル
	std::unique_ptr<PixelRenderer> lightningRenderer_;	// ライトニング描画用レンダラー
	float lightningTime_;								// 雷演出タイマー
	float lightningPower_;								// 雷の強度

	bool isHelp_;				// 現在ヘルプ表示中か
	int helpBackImage_;			// ヘルプ背景画像
	bool wasHelp_;				// 前フレームでヘルプ表示されていたか
	void DrawHelp(void);		// ヘルプ描画関数

};

