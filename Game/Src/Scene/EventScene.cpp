#define NOMINMAX
#include <algorithm>
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h"
#include "../Object/Stage/SkyDome.h"
#include "../Object/Stage/EventStage.h"
#include "../Object/Stage/Mist.h"
#include "../Object/Actor/Player/EventPlayer.h"
#include "../Object/Actor/Enemy/EventEnemy.h"
#include "EventScene.h"


EventScene::EventScene(void)
	:
	currentState_(STATE::START),
	stateTimer_(0.0f),
	isLongPressing(false),
	longPressStartTime_(-1),
	isInitialized_(false),
	progressColor(-1),
	customFontHandle_(-1),
	bgmSHandle_(-1),
	iKeySHandle_(-1),
	cMaxSHandle_(-1),
	isBgm_(false),
	isStateFrame_(false)
{
}

EventScene::~EventScene(void)
{
	currentState_ = STATE::START;
	stateTimer_ = 0.0f;
	if (customFontHandle_ != -1)
	{
		DeleteFontToHandle(customFontHandle_);
	}
}

void EventScene::Init(void)
{
	// プレイヤー
	player_ = std::make_shared<EventPlayer>();
	player_->Init();

	// 敵
	enemy_ = std::make_shared<EventEnemy>(*player_);
	enemy_->Init();

	// ステージ
	stage_ = std::make_shared<EventStage>(*player_, *enemy_);
	stage_->Init();

	// 霧
	mist_ = std::make_shared<Mist>();
	mist_->Init();

	// ステージの初期設定
	stage_->ChangeStage(EventStage::NAME::MAIN_PLANET);

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	InitSound();

	// フォントのロード
	customFontHandle_ = CreateFontToHandle("Garamond", FONT_SIZE, FONT_THICK, DX_FONTTYPE_EDGE);
	if (customFontHandle_ == -1) {
		return;
	}

	// フォントの変更
	ChangeFont("Garamond");

	// カメラの設定
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::EVENT);
	SceneManager::GetInstance().GetCamera()->ResetEventCameraState();

	currentState_ = STATE::START;
	isLongPressing = false;
	longPressStartTime_ = 0; // 長押し開始時刻を初期化
	progressColor = GetColor(PROGRESS_COL_R, PROGRESS_COL_G, PROGRESS_COL_B);
}

void EventScene::Update(void)
{
	float dt = SceneManager::GetInstance().GetDeltaTime();
	stateTimer_ += dt;

	if (!isBgm_)
	{
		PlaySoundMem(bgmSHandle_, DX_PLAYTYPE_LOOP, true);
		isBgm_ = true;
	}

	InputManager& ins = InputManager::GetInstance();

	// EnterキーまたはAボタンが押されているかをチェック
	bool isPressingButton = CheckHitKey(KEY_INPUT_RETURN) || ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (isPressingButton)
	{
		if (!isLongPressing) 
		{
			// 長押し開始
			isLongPressing = true;
			longPressStartTime_ = GetNowCount();
			PlaySoundMem(iKeySHandle_, DX_PLAYTYPE_LOOP, true);
		}
	}
	else 
	{ 
		// 左クリックが離された、またはボタン領域外にマウスがある
		// 長押し中断
		StopSoundMem(iKeySHandle_);
		isLongPressing = false;
		longPressStartTime_ = 0; // リセット
	}

	if (isLongPressing) 
	{
		int currentTime = GetNowCount();
		int elapsedTime = currentTime - longPressStartTime_;

		// 長押し完了判定
		if (elapsedTime >= LONG_PRESS_DURATION_MS) 
		{
			// 長押し完了！シーン遷移
			StopSoundMem(iKeySHandle_);
			PlaySoundMem(cMaxSHandle_, DX_PLAYTYPE_BACK, true);
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		}
	}

	if (stateTimer_ == 0.0f)
	{
		isStateFrame_ = true;
	}
	else 
	{
		isStateFrame_ = false;
	}

	switch (currentState_)
	{
	case STATE::START:
		player_->ChangeState(EventPlayer::STATE::WALK);
		if (stateTimer_ >= START_TIME_LIMIT)
		{
			currentState_ = STATE::STEP_PLAYER_FORWARD;
			stateTimer_ = 0.0f;
		}
		break;
	case EventScene::STATE::STEP_PLAYER_FORWARD:
		player_->SetGoalRotate(0.0f);
		player_->ChangeState(EventPlayer::STATE::WALK);
		if (player_->GetTransform().pos.z > PLAYER_MOVED_LIMIT)
		{
			currentState_ = STATE::LOOK_AROUND;
			stateTimer_ = 0.0f;
		}
		break;
	case EventScene::STATE::LOOK_AROUND:
		player_->ChangeState(EventPlayer::STATE::LOOK);
		if (stateTimer_ >= LOOK_TIME_LIMIT)
		{
			currentState_ = STATE::CAMERA_PAN_TO_ENEMY;
			stateTimer_ = 0.0f;
		}
		break;
	case EventScene::STATE::CAMERA_PAN_TO_ENEMY:
		player_->ChangeState(EventPlayer::STATE::IDLE);
		enemy_->Update();
		if (stateTimer_ > PAN_TIME_LIMIT_FIRST) 
		{
			enemy_->ChangeState(EventEnemy::STATE::WALK);
		}
		if (stateTimer_ > PAN_TIME_LIMIT_SECOND)
		{
			currentState_ = STATE::ENEMY_ROAR;
			stateTimer_ = 0.0f;
			enemy_->ChangeState(EventEnemy::STATE::STANDBY); // 武器構え
		}
		break;
	case EventScene::STATE::ENEMY_ROAR:
		enemy_->Update();
		StopEffekseer3DEffect(enemy_->GetEffectRoarFPlayId());
		if (enemy_->IsEndLanding(EventEnemy::ANIM_TYPE::STANDBY))
		{
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			stateTimer_ = 0.0f;
		}
		break;
	default:
		break;
	}

	// 通常更新
	skyDome_->Update();
	player_->Update();
	enemy_->Update();
	stage_->Update();
}

void EventScene::Draw(void)
{
	skyDome_->Draw();
	mist_->Draw();
	stage_->Draw();
	enemy_->Draw();

	if (currentState_ != STATE::CAMERA_PAN_TO_ENEMY)
	{
		player_->Draw();
	}

	// プログレスバーの描画
	// キーが押されていなくてもプログレスバーの描画関数を呼び出す
	int elapsedTime = isLongPressing ? (GetNowCount() - longPressStartTime_) : 0;
	DrawProgressBar(elapsedTime, LONG_PRESS_DURATION_MS);

	// ボタンメッセージの描画
	const char* buttonMessage = "Press Enter or A";

	// カスタムフォントで描画した際のメッセージの幅と高さを取得します
	int msgWidth = GetDrawStringWidthToHandle(buttonMessage, static_cast<int>(strlen(buttonMessage)), customFontHandle_);
	int msgHeight = GetFontSizeToHandle(customFontHandle_);

	// 右下揃えのためのX座標を計算します:
	// 画面の幅 - メッセージの幅 - 右からの余白
	int msgX = Application::SCREEN_SIZE_X - msgWidth - FONT_MARGIN_RIGHT;

	// 右下揃えのためのY座標を計算します:
	// 画面の高さ - メッセージの高さ - 下からの余白
	int msgY = Application::SCREEN_SIZE_Y - FONT_MARGIN_UNDER;
	DrawStringToHandle(msgX, msgY, buttonMessage, GetColor(FONT_MAX_COL, FONT_MAX_COL, FONT_MAX_COL), customFontHandle_);
}

void EventScene::DrawUI(void)
{

}

void EventScene::UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)
{
	VECTOR playerPos = player_->GetTransform().pos;;
	VECTOR enemyPos = enemy_->GetTransform().pos;
	VECTOR initialPlayerPos = { 0.0f,0.0f, 0.0f };

	// イベントシーンに入った最初のフレームでのみ初期カメラ位置を設定
	if (!isInitialized_ && currentState_ == EventScene::STATE::START && IsStateEnterFrame())
	{
		initialPlayerPos = player_->GetTransform().pos;
		pos = VAdd(initialPlayerPos, EVENT_CAMERA_POS);
		targetPos = VAdd(initialPlayerPos, EVENT_TARGET_POS);
		isInitialized_ = true; // 初期化完了
	}

	float angle = 0.0f;
	float radius = 0.0f;
	float panAmount = 0.0f;
	float t = 0.0f;
	switch (currentState_)
	{
	case EventScene::STATE::START:
		if (!isInitialized_) 
		{
			// プレイヤーの左前上方にカメラを配置
			initialPlayerPos = player_->GetTransform().pos;

			// カメラの位置：プレイヤーの右後ろ上
			pos = VAdd(initialPlayerPos, EVENT_START_CAMERA_POS);
			isInitialized_ = true;
		}
		// ターゲット: プレイヤーの位置（プレイヤーを見る）
		targetPos = VAdd(initialPlayerPos, EVENT_START_TARGET_POS);
		break;
	case EventScene::STATE::STEP_PLAYER_FORWARD:
		if (!isInitialized_) 
		{
			// プレイヤーの左前上方にカメラを配置
			initialPlayerPos = player_->GetTransform().pos;
			// カメラの位置：プレイヤーの右後ろ上
			pos = VAdd(playerPos, EVENT_STEP_CAMERA_POS);
			isInitialized_ = true;
		}

		pos = VAdd(initialPlayerPos, EVENT_STEP_TARGET_POS); // 初期に固定
		targetPos = player_->GetTransform().pos;
		break;

	case EventScene::STATE::LOOK_AROUND:

		// カメラが右上→左下、左上→右下にスイング
		angle = sinf(stateTimer_ * 1.0f);
		pos = VAdd(playerPos, VGet(EVENT_LOOK_CAMER_POS_X * angle, EVENT_LOOK_CAMER_POS_Y, EVENT_LOOK_CAMER_POS_Z));
		targetPos = VAdd(playerPos, EVENT_LOOK_TARGET_POS);
		break;

	case EventScene::STATE::CAMERA_PAN_TO_ENEMY:

		t = std::clamp(stateTimer_ / 2.0f, 0.0f, 1.0f);

		static VECTOR startPanPos;
		static VECTOR endPanPos;

		startPanPos = VAdd(playerPos, EVENT_PAN_CAMERA_START_POS);
		endPanPos = VAdd(playerPos, EVENT_PAN_CAMERA_END_POS);

		pos = AsoUtility::Lerp(startPanPos, endPanPos, t);
		enemyPos = VAdd(enemyPos, EVENT_PAN_ENEMY_POS);
		targetPos = enemyPos;

		break;

	case EventScene::STATE::ENEMY_ROAR:
		// 敵の足元 → 上方へ
		pos = VAdd(enemyPos, VGet(EVENT_ROAR_CAMERA_POS_X,
			EVENT_ROAR_CAMERA_POS_Y + stateTimer_ * EVENT_ROAR_TIME_ADJUSTMOMENT, EVENT_ROAR_CAMERA_POS_Z));
		enemyPos = VAdd(enemyPos, EVENT_ROAR_ENEMY_POS);
		targetPos = enemyPos;
		break;
	default:
		break;
	}
}

bool EventScene::IsStateEnterFrame(void) const
{
	return isStateFrame_;
}

std::string EventScene::GetStateName(STATE state)
{
	switch (state)
	{
	case STATE::STEP_PLAYER_FORWARD: return "STEP_PLAYER_FORWARD";
	case STATE::LOOK_AROUND:         return "LOOK_AROUND";
	case STATE::CAMERA_PAN_TO_ENEMY: return "CAMERA_PAN_TO_ENEMY";
	case STATE::ENEMY_ROAR:          return "ENEMY_ROAR";
	default:                         return "UNKNOWN";
	}
}

void EventScene::DrawProgressBar(int elapsedTime, int totalDuration)
{
	int barX = Application::SCREEN_SIZE_X - BAR_X_ADJUSTMOMENT;
	int barY = Application::SCREEN_SIZE_Y - BAR_Y_ADJUSTMOMENT; // ボタンの下に表示
	int barHeight = BAR_HEIGHT;
	int barMaxWidth = BAR_MAX_WIDTH;

	// 進行度を計算 (0.0 ～ 1.0)
	double progress = (double)elapsedTime / totalDuration;
	progress = std::max(0.0, std::min(1.0, progress));
	int currentBarWidth = (int)(barMaxWidth * progress);

	// プログレスバーの背景 (白枠)
	DrawBox(barX - 1, barY - 1, barX + barMaxWidth + 1, barY + barHeight + 1, GetColor(FONT_MAX_COL, FONT_MAX_COL, FONT_MAX_COL), TRUE);

	// プログレスバーの背景 (濃いグレー)
	DrawBox(barX, barY, barX + barMaxWidth, barY + barHeight, GetColor(FONT_GRAY_COL, FONT_GRAY_COL, FONT_GRAY_COL), TRUE);

	// プログレスバー本体 (緑)
	DrawBox(barX, barY, barX + currentBarWidth, barY + barHeight, progressColor, TRUE);

	int underprogreCol = GetColor(50, 205, 50);

	// プログレスバー本体(下) (緑)
	DrawBox(barX, barY + 10, barX + currentBarWidth, barY + barHeight, underprogreCol, TRUE);

	const char* skipText = "Skip";

	int textWidth = GetDrawStringWidthToHandle(skipText, static_cast<int>(strlen(skipText)), customFontHandle_);
	int fontHeight = GetFontSizeToHandle(customFontHandle_);

	// テキストをバーのY座標の中央に配置
	int textX = barX + (barMaxWidth - textWidth) / 2;
	int textY = barY + (barHeight - fontHeight) / 2;

	DrawStringToHandle(textX, textY, skipText, GetColor(FONT_MAX_COL, FONT_MAX_COL, FONT_MAX_COL), customFontHandle_);
}

void EventScene::InitSound(void)
{
	// bgm
	bgmSHandle_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::EVENT_BGM).handleId_;
	ChangeVolumeSoundMem(BGM_VALUME, bgmSHandle_);

	// 再生中かどうか
	isBgm_ = false;

	// 入力されているときの音
	iKeySHandle_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::GAUGE_BGM2).handleId_;
	ChangeVolumeSoundMem(INPUTKEY_VALUME, iKeySHandle_);

	// 入力が完了したときの音
	cMaxSHandle_ = ResourceManager::GetInstance().Load(
		ResourceManager::SRC::GAUGE_BGM1).handleId_;
	ChangeVolumeSoundMem(COUNTMAX_VALUME, cMaxSHandle_);
}