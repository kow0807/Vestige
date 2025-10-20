#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Renderer/PixelMaterial.h"
#include "../Renderer/PixelRenderer.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/Stage/SkyDome.h"
#include "../Object/Stage/Stage.h"
#include "../Object/Stage/Mist.h"
#include "../Object/Actor/Player/Player.h"
#include "../Object/Actor/Player/PlayerDummy.h"
#include "../Object/Stage/Planet.h"
#include "../Object/Actor/Enemy/EnemyBoss/EnemyBoss.h"
#include "../Object/Actor/Enemy/EnemyBoss/EnemyBossDummy.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	gameBgm_(resMng_.Load(ResourceManager::SRC::GAME_BGM).handleId_),
	bgmVol_(160),
	isBgm_(true),
	deadSe_(resMng_.Load(ResourceManager::SRC::DEAD_SE).handleId_),
	helpSe_(resMng_.Load(ResourceManager::SRC::HELP_SE).handleId_),
	deathFadeAlpha_(0),
	youDiedTimer_(0),
	enemySlainFadeAlpha_(0),
	enemySlainTimer_(0),
	enemySlain_(false),
	isHelp_(false),
	helpBackImage_(-1),
	wasHelp_(false),
	postEffectScreen_(MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true)),
	lowHpVisionMaterial_(std::make_unique<PixelMaterial>("LowHpVision.cso", VISION_CONST_BUF_FLOAT4_SIZE)),
	lightningMaterial_(std::make_unique<PixelMaterial>("Lightning.cso", LIGHTNING_CONST_BUF_FLOAT4_SIZE)),
	lightningTime_(-1),
	lightningPower_(0.0f)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	ChangeVolumeSoundMem(bgmVol_, gameBgm_);

	ChangeVolumeSoundMem(255, deadSe_);

	ChangeVolumeSoundMem(220, helpSe_);

	ChangeFont("MS ゴシック");

	// プレイヤー
	player_ = std::make_shared<Player>();
	player_->Init();

	playerDummy_ = std::make_shared<PlayerDummy>(*player_);

	// boss
	enemyBoss_ = std::make_shared<EnemyBoss>(*player_);
	enemyBoss_->Init();

	// bossダミー
	enemyBossDummy_ = std::make_shared<EnemyBossDummy>(*enemyBoss_);

	mist_ = std::make_shared<Mist>();
	mist_->Init();

	// ステージ
	stage_ = std::make_unique<Stage>(*player_, *enemyBoss_);
	stage_->Init();

	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();


	SceneManager::GetInstance().GetCamera()->LockonSoundLoad();
	SceneManager::GetInstance().GetCamera()->SetFollow(&player_->GetTransform());
	SceneManager::GetInstance().GetCamera()->SetFollowEnemy(&enemyBoss_->GetTransform());
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW_MOUSE);
	player_->SetEnemy(enemyBoss_);
	player_->SetEnemyDummy(enemyBossDummy_);
	player_->SetMist(mist_);
	enemyBoss_->SetMist(mist_);

	lowHpVisionMaterial_->AddConstBuf({ 1.0f, 1.0f, 1.0f, 1.0f });

	float hpRate = static_cast<float>(player_->GetHp()) / static_cast<float>(Player::MAX_HP);
	lowHpVisionMaterial_->AddConstBuf({ hpRate, 0.0f, 0.0f, 0.0f });

	lowHpVisionMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	lowHpVisionRenderer_ = std::make_unique<PixelRenderer>(*lowHpVisionMaterial_);
	lowHpVisionRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);

	lightningMaterial_ = std::make_unique<PixelMaterial>("Lightning.cso", 1);

	lightningTime_ = GetNowCount() / 1000.0f;
	lightningPower_ = 0.0f;

	lightningMaterial_->AddConstBuf({ lightningTime_, lightningPower_, 1.0f, 1.0f });

	lightningMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	lightningRenderer_ = std::make_unique<PixelRenderer>(*lightningMaterial_);
	lightningRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);
}

void GameScene::Update(void)
{
	if (isBgm_)
	{
		PlaySoundMem(gameBgm_, DX_PLAYTYPE_LOOP, true);
		isBgm_ = false;
	}


	InputManager& ins = InputManager::GetInstance();

	if (player_->GetHp() > 0 && enemyBoss_->GetHp() > 0)
	{
		SceneManager::GetInstance().HelpUpdate();
		if (ins.IsTrgDown(KEY_INPUT_TAB) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::SEVEN))
		{
			PlaySoundMem(helpSe_, DX_PLAYTYPE_BACK, true);
			isHelp_ = !isHelp_;

			if (isHelp_)
			{
				// すでに作られていたら削除
				if (helpBackImage_ != -1)
				{
					DeleteGraph(helpBackImage_);
					helpBackImage_ = -1;
				}
				// ヘルプ用背景画像を作成
				helpBackImage_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true);

				// 現在のメイン画面をコピー（キャプチャ）
				int mainScreen = SceneManager::GetInstance().GetMainScreen();
				SetDrawScreen(helpBackImage_);
				DrawGraph(0, 0, mainScreen, false);
				SetDrawScreen(mainScreen); // 元に戻す
			}
		}
	}

	if (!isHelp_)
	{
		// スカイドームやオブジェクトの更新
		skyDome_->Update();
		stage_->Update();
		player_->Update();
		if (enemyBoss_->GetHp() > 0)
		{
			player_->CollisionEnemyBoss();
		}
		player_->CollisionMist();
		playerDummy_->Update();
		enemyBoss_->Update();
		enemyBoss_->CollisionMist();
		enemyBossDummy_->Update();

		lightningTime_ = GetNowCount() / 1000.0f;
		if (player_->IsLightning())
		{
			lightningPower_ = 1.0f;
		}
		if (lightningPower_ > 0.0f)
		{
			lightningPower_ -= 0.05f; // 1秒で0になる（= 20fps で0.05fずつ）
			if (lightningPower_ < 0.0f) lightningPower_ = 0.0f;
		}

		// 敵撃破時の演出開始（Scene遷移を遅らせる）
		if (!enemySlain_ && enemyBoss_->GetHp() <= 0)
		{
			enemySlain_ = true;
			enemySlainFadeAlpha_ = 0;
			enemySlainTimer_ = 0;
		}

		if (enemySlain_)
		{
			enemySlainTimer_++;

			// フェードイン（0～60フレーム）
			if (enemySlainTimer_ <= 60)
			{
				enemySlainFadeAlpha_ = enemySlainTimer_ * (255 / 60);
			}
			// 表示キープ（61～180フレーム）
			else if (enemySlainTimer_ <= 180)
			{
				enemySlainFadeAlpha_ = 255;
			}
			// フェードアウト（181～280フレーム）
			else if (enemySlainTimer_ <= 280)
			{
				enemySlainFadeAlpha_ = 255 - (enemySlainTimer_ - 180) * (255 / 60);
			}

			// 終了後にクリアシーンへ遷移
			if (enemySlainTimer_ > 280)
			{
				player_->StopEffect();
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
			}
		}

		// YOU DIED の処理
		if (player_->IsDead())
		{
			if (deathFadeAlpha_ < 190)
			{
				deathFadeAlpha_ += 2;
				if (deathFadeAlpha_ > 190) deathFadeAlpha_ = 190;
			}
			youDiedTimer_++;

			if (youDiedTimer_ == 1)
			{
				PlaySoundMem(deadSe_, DX_PLAYTYPE_BACK, true);
			}

			// リトライキー入力でシーン再初期化
			if (youDiedTimer_ > 90 &&
				(ins.IsTrgDown(KEY_INPUT_E) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT)))
			{
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
				return;
			}

			if (youDiedTimer_ > 660)
			{
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
			}

		}
		if (player_->GetHp() <= 0)
		{
			if (bgmVol_ > 120)
			{
				bgmVol_--;
			}
			ChangeVolumeSoundMem(bgmVol_, gameBgm_);
		}
	}
}

void GameScene::Draw(void)
{
	if (isHelp_ && helpBackImage_ != -1)
	{
		return; // 通常描画はスキップ
	}

	// 背景
	skyDome_->Draw();
	stage_->Draw();
	mist_->Draw();

	enemyBoss_->Draw();

	player_->Draw();

	SceneManager::GetInstance().GetCamera()->DrawLockonMarker();

	int mainScreen = SceneManager::GetInstance().GetMainScreen();

	// ポストエフェクト(プレイヤーの視界)
	//-----------------------------------------
	float hpRate = static_cast<float>(player_->GetHp()) / static_cast<float>(Player::MAX_HP);
	lowHpVisionMaterial_->SetConstBuf(1, { hpRate, 0.0f, 0.0f, 0.0f });

	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	lowHpVisionRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------

	// ポストエフェクト(ライトニング)
	//-----------------------------------------
	lightningMaterial_->SetConstBuf(0, { lightningTime_, lightningPower_, 0.0f, 0.0f });

	SetDrawScreen(postEffectScreen_);

	// 画面を初期化
	ClearDrawScreen();

	lightningRenderer_->Draw();

	// メインに戻す
	SetDrawScreen(mainScreen);
	DrawGraph(0, 0, postEffectScreen_, false);
	//-----------------------------------------
}

void GameScene::DrawUI(void)
{
	player_->DrawHp(static_cast<float>(player_->GetHp()), Player::MAX_HP);
	player_->DrawStamina(static_cast<float>(player_->GetStamina()), Player::MAX_STAMINA);
	player_->DrawItem();
	player_->DrawUlt();


	enemyBoss_->DrawHpBar();

	// 「ENEMY SLAIN」演出
	if (enemySlain_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, enemySlainFadeAlpha_);

		int screenW = Application::SCREEN_SIZE_X;
		int screenH = Application::SCREEN_SIZE_Y;
		const char* msg = "ENEMY SLAIN";

		int fontSize = 80;
		SetFontSize(fontSize);

		int msgW = GetDrawStringWidth(msg, static_cast<int>(strlen(msg)));
		int msgH = fontSize;
		int x = (screenW - msgW) / 2;
		int y = (screenH - msgH) / 2;

		// 背景
		int padding = 10;
		DrawBox(0, y - padding, screenW, y + msgH + padding, GetColor(0, 0, 0), TRUE);

		// 影付き文字
		int shadowOffset = 5;
		DrawString(x + shadowOffset, y + shadowOffset, msg, GetColor(0, 0, 0));
		DrawString(x, y, msg, GetColor(255, 215, 0));

		SetFontSize(16);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 「YOU DIED」演出
	if (player_->IsDead())
	{
		// 画面の黒オーバーレイ（暗転）
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, deathFadeAlpha_);
		DrawBox(0, 0, 1024, 640, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// 「YOU DIED」の文字をフェードイン表示
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, deathFadeAlpha_);

		int screenW = Application::SCREEN_SIZE_X;
		int screenH = Application::SCREEN_SIZE_Y;
		const char* msg = "YOU DIED";

		int fontSize = 80;
		SetFontSize(fontSize);

		int msgW = GetDrawStringWidth(msg, static_cast<int>(strlen(msg)));
		int msgH = fontSize;

		// 中央位置
		int x = (screenW - msgW) / 2;
		int y = (screenH - msgH) / 2;

		// === 背後===
		int padding = 10;
		DrawBox(
			0,
			y - padding,
			screenW,
			y + msgH + padding,
			GetColor(0, 0, 0),
			TRUE
		);

		// === 影付き文字 ===
		int shadowOffset = 5; // 影のズレ量（ピクセル）
		// 影（黒文字）を少し下・右にズラして描く
		DrawString(x + shadowOffset, y + shadowOffset, msg, GetColor(0, 0, 0));

		// 本体（赤文字）を上に重ねる
		DrawString(x, y, msg, GetColor(255, 0, 0));
		SetFontSize(16);

		// Retryメッセージ表示（一定時間経過後）
		if (youDiedTimer_ > 90)
		{
			const char* retryMsg = "Eキー or Xボタン でリトライ";

			int retryFontSize = 32;
			SetFontSize(retryFontSize);

			int retryW = GetDrawStringWidth(retryMsg, static_cast<int>(strlen(retryMsg)));
			int retryH = retryFontSize;

			int rx = (screenW - retryW) / 2;
			int ry = y + msgH + 80;

			// 点滅＋白から赤へ変化させるアルファ計算
			float time = youDiedTimer_ / 60.0f;  // 秒数
			int blinkAlpha = (int)((sin(time * 3.14f) * 0.5f + 0.5f) * 255);
			if (blinkAlpha > 255) blinkAlpha = 255;
			if (blinkAlpha < 0) blinkAlpha = 0;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, blinkAlpha);

			// 影
			DrawString(rx + 2, ry + 2, retryMsg, GetColor(0, 0, 0));
			// 本体
			DrawString(rx, ry, retryMsg, GetColor(255, 255, 255));

			// 「自動遷移まで残りX秒」のメッセージを描画
			int autoMsgFontSize = 20;
			SetFontSize(autoMsgFontSize);

			int autoTransitionTimeSec = 10;

			int remaining = (660 - youDiedTimer_ + 59) / 60;
			if (remaining < 0)
			{
				remaining = 0;
			}

			std::string autoMsg = std::to_string(remaining) + " 秒後諦めます";

			int autoMsgW = GetDrawStringWidth(autoMsg.c_str(), (int)autoMsg.size());
			int ax = (screenW - autoMsgW) / 2;
			int ay = ry + retryH + 10;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
			DrawString(ax, ay, autoMsg.c_str(), GetColor(255, 255, 255));
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			SetFontSize(16);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// ヘルプ
	if (isHelp_ && helpBackImage_ != -1)
	{
		DrawGraph(0, 0, helpBackImage_, false);
		DrawHelp();
	}
}

void GameScene::UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)
{
}

void GameScene::DrawHelp(void)
{
	const int screenWidth = 1024;
	const int screenHeight = 640;

	// === テキスト設定 ===
	const int fontSize = 18;
	const int lineHeight = fontSize + 14;
	SetFontSize(18);

	// コマンドと説明に分ける
	const char* helpLabels[] = 
	{
		"視点操作", "移動", "ダッシュ", "回避", "攻撃", "強攻撃",
		"ロックオン", "必殺技", "アイテム使用", "アイテム変更",
		"操作確認"
	};
	const char* helpInputs[] = 
	{
		"マウス or 右スティック",
		"WASDキー or 左スティック",
		"Shift or Aボタン",
		"スペースキー or Bボタン",
		"左クリック or RBボタン",
		"右クリック or RTボタン",
		"ホイールクリック or 右スティック押し込み",
		"Qキー or LBボタン",
		"Rキー or Xボタン",
		"マウスホイール or 十字キー左右",
		"Tabキー or メニューボタン"
	};
	const int lineCount = sizeof(helpLabels) / sizeof(helpLabels[0]);

	// レイアウト
	int contentWidth = 660;
	int contentHeight = 40 + lineCount * lineHeight + 20;

	int boxX1 = (screenWidth - contentWidth) / 2;
	int boxY1 = (screenHeight - contentHeight) / 2;
	int boxX2 = boxX1 + contentWidth;
	int boxY2 = boxY1 + contentHeight;

	// 背景
	DrawBox(boxX1, boxY1, boxX2, boxY2, GetColor(0, 0, 0), TRUE);

	// タイトル
	DrawString(boxX1 + 20, boxY1 + 20, "【操作説明】", GetColor(255, 255, 255));

	// 描画位置
	int labelX = boxX1 + 40;
	int inputX = boxX1 + 170;
	int y = boxY1 + 60;

	for (int i = 0; i < lineCount; ++i)
	{
		DrawString(labelX, y + i * lineHeight, helpLabels[i], GetColor(255, 255, 255));
		DrawString(inputX, y + i * lineHeight, "：", GetColor(255, 255, 255));
		DrawString(inputX + 60, y + i * lineHeight, std::string(helpInputs[i]).c_str(), GetColor(255, 255, 255));
	}
	SetFontSize(16);
}