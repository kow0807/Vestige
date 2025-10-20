#include <sstream>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Renderer/PixelMaterial.h"
#include "../Renderer/PixelRenderer.h"
#include "../Object/Stage/SkyDome.h"
#include "../Object/Stage/Stage.h"
#include "../Object/Stage/Mist.h"
#include "../Object/Actor/Player/Player.h"
#include "../Object/Actor/Enemy/EnemyBoss/EnemyBoss.h"
#include "../Object/Actor/Enemy/EnemyScarecrow.h"
#include "../Object/Actor/Enemy/EnemyScarecrowDummy.h"
#include "TutorialScene.h"

TutorialScene::TutorialScene(void)
	:
	tutorialBgm_(resMng_.Load(ResourceManager::SRC::TUTORIAL_BGM).handleId_),
	isBgm_(true),
	nextSe_(resMng_.Load(ResourceManager::SRC::NEXT_SE).handleId_),
	helpSe_(resMng_.Load(ResourceManager::SRC::HELP_SE).handleId_),
	tutorialStep_(TutorialStep::LOOK),
	lookCounter_(0),
	prevLookX_(0),
	prevLookY_(0),
	movedW_(false),
	movedA_(false),
	movedS_(false),
	movedD_(false),
	dashed_(false),
	rolled_(false),
	attacked_(false),
	comboAttacked_(false),
	heavyAttacked_(false),
	lockedOn_(false),
	usedUlt_(false),
	usedItem_(false),
	itemChanged_(false),
	nextStepTimer_(0.0f),
	isHelp_(false),
	helpBackImage_(-1),
	wasHelp_(false),
	postEffectScreen_(MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, true)),
	lightningMaterial_(std::make_unique<PixelMaterial>("Lightning.cso", CONST_BUF_FLOAT4_SIZE)),
	lightningTime_(GetNowCount() / MILLISECONDS_TO_SECONDS),
	lightningPower_(0.0f)

{

}

TutorialScene::~TutorialScene(void)
{
	if (helpBackImage_ != -1)
	{
		DeleteGraph(helpBackImage_);
		helpBackImage_ = -1;
	}
}

void TutorialScene::Init(void)
{
	ChangeFont("MS ゴシック");

	ChangeVolumeSoundMem(TUTORIAL_BGM_VOL, tutorialBgm_);

	ChangeVolumeSoundMem(NEXT_SE_VOL, nextSe_);

	ChangeVolumeSoundMem(HELP_SE_VOL, helpSe_);

	// プレイヤー
	player_ = std::make_shared<Player>();
	player_->Init();

	// boss
	enemyBoss_ = std::make_shared<EnemyBoss>(*player_);
	enemyBoss_->Init();

	// かかし
	enemyScarecrow_ = std::make_shared<EnemyScarecrow>(*player_);
	enemyScarecrow_->Init();

	// かかしダミー
	enemyScarecrowDummy_ = std::make_shared<EnemyScarecrowDummy>(*enemyScarecrow_);


	// ステージ
	stage_ = std::make_unique<Stage>(*player_, *enemyBoss_);
	stage_->Init();

	// ミスト
	mist_ = std::make_shared<Mist>();
	mist_->Init();

	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	// スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	SceneManager::GetInstance().GetCamera()->LockonSoundLoad();
	SceneManager::GetInstance().GetCamera()->SetFollow(&player_->GetTransform());
	SceneManager::GetInstance().GetCamera()->SetFollowEnemy(&enemyScarecrow_->GetTransform());
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW_MOUSE);
	player_->SetEnemyScarecrow(enemyScarecrow_);
	player_->SetEnemyScarecrowDummy(enemyScarecrowDummy_);
	player_->SetMist(mist_);


	lightningMaterial_->AddConstBuf({ lightningTime_, lightningPower_, 1.0f, 1.0f });

	lightningMaterial_->AddTextureBuf(SceneManager::GetInstance().GetMainScreen());
	lightningRenderer_ = std::make_unique<PixelRenderer>(*lightningMaterial_);
	lightningRenderer_->MakeSquereVertex(
		Vector2(0, 0),
		Vector2(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y)
	);
}

void TutorialScene::Update(void)
{
	if (isBgm_)
	{
		PlaySoundMem(tutorialBgm_, DX_PLAYTYPE_LOOP, true);
		isBgm_ = false;
	}
	SceneManager::GetInstance().HelpUpdate();

	InputManager& ins = InputManager::GetInstance();
	wasHelp_ = false;

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
		else
		{
			wasHelp_ = true;
		}
	}

	if (!isHelp_)
	{
		lightningTime_ = GetNowCount() / MILLISECONDS_TO_SECONDS;
		if (player_->IsLightning())
		{
			lightningPower_ = LIGHTNING_POWER_MAX;
		}
		if (lightningPower_ > 0.0f)
		{
			lightningPower_ -= LIGHTNING_DECAY_RATE;
			if (lightningPower_ < 0.0f) lightningPower_ = 0.0f;
		}

		// チュートリアル進行処理
		switch (tutorialStep_)
		{
		case TutorialStep::LOOK:
			LookUpdate();
			break;
		case TutorialScene::TutorialStep::MOVE:
			MoveUpdate();
			break;
		case TutorialStep::DASH:
			DashUpdate();
			break;
		case TutorialStep::ROLLING:
			RollingUpdate();
			break;
		case TutorialStep::ATTACK:
			AttackUpdate();
			break;
		case TutorialStep::COMBO_ATTACK:
			ComboAttackUpdate();
			break;
		case TutorialStep::HEAVY_ATTACK:
			HeavyAttackUpdate();
			break;
		case TutorialStep::LOCKON:
			LockonUpdate();
			break;
		case TutorialStep::ULT:
			UltUpdate();
			break;
		case TutorialStep::ITEM:
			ItemUpdate();
			break;
		case TutorialStep::ITEM_CHANGE:
			ItemChangeUpdate();
			break;
		case TutorialStep::HELP:
			HelpUpdate();
			break;
		case TutorialStep::DEFEAT_ENEMY:
			DefeatEnemyUpdate();
			break;
		case TutorialStep::NEXT:
			nextStepTimer_ += SceneManager::GetInstance().GetDeltaTime();

			// 5秒後にゲームシーンへ
			if (nextStepTimer_ >= TIME_TO_NEXT_SCENE) {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::EVENT);
			}
			break;
		}


		skyDome_->Update();

		stage_->Update();

		player_->Update();
		player_->CollisionEnemyScarecrow();

		enemyScarecrow_->Update();
		enemyScarecrowDummy_->Update();

		player_->CollisionMist();
	}
}

void TutorialScene::Draw(void)
{
	if (isHelp_ && helpBackImage_ != -1)
	{
		return; // 通常描画はスキップ
	}

	// 背景
	skyDome_->Draw();
	mist_->Draw();
	stage_->Draw();

	enemyScarecrow_->Draw();

	player_->Draw();
	SceneManager::GetInstance().GetCamera()->DrawLockonMarker();

	int mainScreen = SceneManager::GetInstance().GetMainScreen();

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

void TutorialScene::DrawUI(void)
{
	if (!isHelp_)
	{
		player_->DrawHp(player_->GetHp(), Player::MAX_HP);
		player_->DrawStamina(static_cast<float>(player_->GetStamina()), Player::MAX_STAMINA);
		player_->DrawItem();
		player_->DrawUlt();


		// チュートリアル進行処理
		switch (tutorialStep_)
		{
		case TutorialStep::LOOK:
			DrawTutorialText(
				"マウス or 右スティックを動かして\n視点を操作してみましょう",
				nullptr);
			break;
		case TutorialStep::MOVE:
			DrawTutorialText(
				"WASDキー or 左スティックで\n移動してみましょう",
				nullptr);
			break;
		case TutorialStep::DASH:
			DrawTutorialText(
				"Shiftを押しながら移動 or Aボタンを\n押しながら移動して\n数秒ダッシュしてみましょう",
				nullptr);
			break;
		case TutorialStep::ROLLING:
			DrawTutorialText(
				"スペースキー or Bボタンで\n回避してみましょう",
				nullptr);
			break;
		case TutorialStep::ATTACK:
			DrawTutorialText(
				"左クリック or RBボタンで\n攻撃してみましょう",
				nullptr);
			break;
		case TutorialStep::COMBO_ATTACK:
			DrawTutorialText(
				"左クリック連打 or RBボタン連打で\n3連続攻撃をしてみましょう",
				nullptr);
			break;
		case TutorialStep::HEAVY_ATTACK:
			DrawTutorialText(
				"右クリック or RTボタンで\n強攻撃してみましょう",
				nullptr);
			break;
		case TutorialStep::LOCKON:
			DrawTutorialText(
				"ホイールクリック or 右スティック押し込みで\nロックオン/解除してみましょう",
				nullptr);
			break;
		case TutorialStep::ULT:
			DrawTutorialText(
				"Qキー or LBボタンで\n必殺技を使ってみましょう",
				"\n(使用中は攻撃力UP!)");
			break;
		case TutorialStep::ITEM:
			DrawTutorialText(
				"Rキー or Xボタンで\nアイテムを使用してみましょう",
				nullptr);
			break;
		case TutorialStep::ITEM_CHANGE:
			DrawTutorialText(
				"マウスホイール or 十字キー左右で\nアイテムを変更してみましょう",
				nullptr);
			break;
		case TutorialStep::HELP:
			DrawTutorialText(
				"TABキー or メニューボタンを押して\n操作説明を表示してみましょう",
				"\n（もう一度押すと閉じられます）"
			);
			break;
		case TutorialStep::DEFEAT_ENEMY:
			DrawTutorialText(
				"敵を倒してみましょう！",
				nullptr);
			enemyScarecrow_->DrawHpBar();
			break;
		case TutorialStep::NEXT:
			int remain = static_cast<int>(ceilf(TIME_TO_NEXT_SCENE - nextStepTimer_));
			if (remain < 0) remain = 0;

			std::ostringstream oss;
			oss << "チュートリアル完了！\nゲームを開始します（" << remain << "）";

			DrawTutorialText(oss.str().c_str(), nullptr);
			break;
		}
	}
	// ヘルプ
	if (isHelp_ && helpBackImage_ != -1)
	{
		// 背景のキャプチャを描画
		DrawGraph(0, 0, helpBackImage_, false);
		DrawHelp();
	}
}

void TutorialScene::UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)
{
}

void TutorialScene::DrawTutorialText(const char* text, const char* subText)
{
	const int screenW = Application::SCREEN_SIZE_X;
	const int screenH = Application::SCREEN_SIZE_Y;

	const int boxWidth = static_cast<int>(screenW * TUTORIAL_BOX_WIDTH_RATIO);
	const int boxY = (screenH - TUTORIAL_BOX_HEIGHT) / 2;

	// 【背景：黒を半透明にして周囲が少し見えるように】	
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BG_ALPHA);
	DrawRoundRect(TUTORIAL_BOX_X, boxY, TUTORIAL_BOX_X + boxWidth, boxY + TUTORIAL_BOX_HEIGHT, TUTORIAL_BOX_CORNER_RADIUS, TUTORIAL_BOX_CORNER_RADIUS, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 【枠線：外側に暗い赤紫のぼんやり光る縁取り】
	for (int i = 0; i < BORDER_LAYER_COUNT; ++i)
	{
		int alpha = BORDER_ALPHA_START - i * BORDER_ALPHA_STEP;
		if (alpha < 0) alpha = 0;
		DrawRoundRect(
			TUTORIAL_BOX_X - i, boxY - i, TUTORIAL_BOX_X + boxWidth + i, boxY + TUTORIAL_BOX_HEIGHT + i,
			TUTORIAL_BOX_CORNER_RADIUS + i, TUTORIAL_BOX_CORNER_RADIUS + i,
			GetColor(BORDER_COLOR_R, BORDER_COLOR_G, BORDER_COLOR_B) & 0x00FFFFFF | (alpha << ALPHA_SHIFT),
			FALSE
		);
	}

	// 【枠線：内側の錆びた鉄っぽいグレー】
	DrawRoundRect(TUTORIAL_BOX_X, boxY, TUTORIAL_BOX_X + boxWidth, boxY + TUTORIAL_BOX_HEIGHT,
		TUTORIAL_BOX_CORNER_RADIUS, TUTORIAL_BOX_CORNER_RADIUS, GetColor(INNER_BORDER_COLOR_R, INNER_BORDER_COLOR_G, INNER_BORDER_COLOR_B), FALSE);

	// 【メインテキスト：冷たい銀灰色、紫がかった黒影付き】
	SetFontSize(MAIN_FONT_SIZE);
	int mainTextHeight = GetFontSize();

	DrawString(TUTORIAL_BOX_X + TEXT_LEFT_PADDING + SHADOW_OFFSET_2, boxY + TEXT_TOP_PADDING + SHADOW_OFFSET_2, text, GetColor(MAIN_TEXT_SHADOW_COLOR1_R, MAIN_TEXT_SHADOW_COLOR1_G, MAIN_TEXT_SHADOW_COLOR1_B));
	DrawString(TUTORIAL_BOX_X + TEXT_LEFT_PADDING + SHADOW_OFFSET, boxY + TEXT_TOP_PADDING + SHADOW_OFFSET, text, GetColor(MAIN_TEXT_SHADOW_COLOR2_R, MAIN_TEXT_SHADOW_COLOR2_G, MAIN_TEXT_SHADOW_COLOR2_B));
	DrawString(TUTORIAL_BOX_X + TEXT_LEFT_PADDING, boxY + TEXT_TOP_PADDING, text, GetColor(MAIN_TEXT_COLOR_R, MAIN_TEXT_COLOR_G, MAIN_TEXT_COLOR_B));

	// 【サブテキスト：薄紫グレーで影付き】
	if (subText != nullptr)
	{
		SetFontSize(SUB_FONT_SIZE);

		DrawString(TUTORIAL_BOX_X + TEXT_LEFT_PADDING + SHADOW_OFFSET, boxY + TEXT_TOP_PADDING + mainTextHeight + SUBTEXT_SPACING + SHADOW_OFFSET, subText, GetColor(SUB_TEXT_SHADOW_COLOR_R, SUB_TEXT_SHADOW_COLOR_G, SUB_TEXT_SHADOW_COLOR_B));
		DrawString(TUTORIAL_BOX_X + TEXT_LEFT_PADDING, boxY + TEXT_TOP_PADDING + mainTextHeight + SUBTEXT_SPACING, subText, GetColor(SUB_TEXT_COLOR_R, SUB_TEXT_COLOR_G, SUB_TEXT_COLOR_B));
	}

	SetFontSize(SUB_FONT_SIZE);
}


void TutorialScene::LookUpdate(void)
{
	int nowX, nowY;
	GetMousePoint(&nowX, &nowY);

	int dx = nowX - prevLookX_;
	int dy = nowY - prevLookY_;

	if (abs(dx) > LOOK_MOUSE_MOVE_THRESHOLD || abs(dy) > LOOK_MOUSE_MOVE_THRESHOLD) {
		lookCounter_++;
	}

	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	if (input.Rx > LOOK_STICK_THRESHOLD)
	{
		lookCounter_++;
	}
	if (input.Rx < -LOOK_STICK_THRESHOLD)
	{
		lookCounter_++;
	}
	if (input.Ry > LOOK_STICK_THRESHOLD)
	{
		lookCounter_++;
	}
	if (input.Ry < -LOOK_STICK_THRESHOLD)
	{
		lookCounter_++;
	}


	prevLookX_ = nowX;
	prevLookY_ = nowY;

	if (lookCounter_ > REQUIRED_LOOK_MOVES) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::MOVE;
	}
}

void TutorialScene::MoveUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();
	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	if (ins.IsTrgDown(KEY_INPUT_W) || input.Y < -STICK_MOVE_THRESHOLD) movedW_ = true;
	if (ins.IsTrgDown(KEY_INPUT_A) || input.X < -STICK_MOVE_THRESHOLD) movedA_ = true;
	if (ins.IsTrgDown(KEY_INPUT_S) || input.Y > STICK_MOVE_THRESHOLD) movedS_ = true;
	if (ins.IsTrgDown(KEY_INPUT_D) || input.X > STICK_MOVE_THRESHOLD) movedD_ = true;

	if (movedW_ && movedA_ && movedS_ && movedD_) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::DASH;  // DASHへ
	}
}

void TutorialScene::DashUpdate(void)
{
	static float runTime = 0.0f;

	if (player_->IsRun()) {
		runTime += SceneManager::GetInstance().GetDeltaTime();
	}

	if (runTime >= REQUIRED_RUN_TIME) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::ROLLING;
		runTime = 0.0f;
	}
}

void TutorialScene::RollingUpdate(void)
{
	if (player_->GetRollingCnt() >= Player::MAX_ROLLING_CNT - JUDGE_MARGIN) {
		rolled_ = true;
	}

	if (rolled_) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::ATTACK;
	}
}

void TutorialScene::AttackUpdate(void)
{
	if (player_->GetAttackCnt() >= Player::MAX_ATTACK_CNT - JUDGE_MARGIN) {
		attacked_ = true;
	}

	if (attacked_) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::COMBO_ATTACK;
	}
}

void TutorialScene::ComboAttackUpdate(void)
{
	if (player_->GetAttackCnt2() <= 0.0f && player_->GetAttackCnt3() >= Player::MAX_ATTACK_CNT_3 - JUDGE_MARGIN) {
		comboAttacked_ = true;
	}

	if (comboAttacked_) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::HEAVY_ATTACK;
	}
}

void TutorialScene::HeavyAttackUpdate(void)
{
	if (player_->GetStrongAttackCnt() >= Player::MAX_STRONG_ATTACK_CNT - JUDGE_MARGIN) {
		heavyAttacked_ = true;
	}

	if (heavyAttacked_) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::LOCKON;
	}
}

void TutorialScene::LockonUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();

	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	if (ins.IsTrgMouseMiddle() || input.Buttons[PAD_BUTTON_LOCKON]) {
		lockedOn_ = true;
	}

	if (lockedOn_) {
		player_->TutorialUlt();
		player_->SetUltValue(Player::ULT_MAX_VALUE);
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::ULT;
	}
}

void TutorialScene::UltUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();
	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	if (ins.IsTrgDown(KEY_INPUT_Q) || input.Buttons[PAD_BUTTON_ULT]) {
		usedUlt_ = true;
	}

	if (usedUlt_) {
		player_->Damage(TUTORIAL_DAMAGE);
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::ITEM;
	}
}

void TutorialScene::ItemUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();
	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	if (ins.IsTrgDown(KEY_INPUT_R) || input.Buttons[PAD_BUTTON_ITEM]) {
		usedItem_ = true;
	}

	if (usedItem_) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::ITEM_CHANGE;
	}
}

void TutorialScene::ItemChangeUpdate(void)
{
	InputManager& ins = InputManager::GetInstance();
	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	// ホイールの累積回転量を static で保持
	static int wheelCounter = 0;
	int wheelDelta = ins.GetWheelDelta();

	// 回転量を蓄積
	wheelCounter += wheelDelta;

	if (wheelCounter >= WHEEL_THRESHOLD || input.POV[0] == POV_RIGHT) {
		itemChanged_ = true;
	}
	else if (wheelCounter <= -WHEEL_THRESHOLD || input.POV[0] == POV_LEFT)
	{
		itemChanged_ = true;
	}
	if (itemChanged_) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::HELP;
	}
}

void TutorialScene::HelpUpdate(void)
{
	if (wasHelp_) {
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::DEFEAT_ENEMY;
	}
}

void TutorialScene::DefeatEnemyUpdate(void)
{
	if (player_->IsLightning()) {
		enemyScarecrow_->Damage(LIGHTNING_DAMAGE);
	}
	if (player_->IsHitAttackEnemyScarecrow()) {
		enemyScarecrow_->Damage(player_->GetAttack());
	}
	if (!enemyScarecrow_->IsAlive())
	{
		PlaySoundMem(nextSe_, DX_PLAYTYPE_BACK, true);
		tutorialStep_ = TutorialStep::NEXT;
		nextStepTimer_ = 0.0f;
	}
}

void TutorialScene::DrawHelp(void)
{
	const int screenWidth = Application::SCREEN_SIZE_X;
	const int screenHeight = Application::SCREEN_SIZE_Y;

	// === テキスト設定 ===
	const int lineHeight = HELP_FONT_SIZE + HELP_LINE_SPACING;
	SetFontSize(HELP_FONT_SIZE);

	// コマンドと説明に分ける
	const char* helpLabels[] = {
		"視点操作", "移動", "ダッシュ", "回避", "攻撃", "強攻撃",
		"ロックオン", "必殺技", "アイテム使用", "アイテム変更",
		"操作確認"
	};
	const char* helpInputs[] = {
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
	int contentHeight = HELP_TITLE_TOP_MARGIN + lineCount * lineHeight + HELP_PADDING;

	int boxX1 = (screenWidth - HELP_BOX_WIDTH) / 2;
	int boxY1 = (screenHeight - contentHeight) / 2;
	int boxX2 = boxX1 + HELP_BOX_WIDTH;
	int boxY2 = boxY1 + contentHeight;

	// 背景
	DrawBox(boxX1, boxY1, boxX2, boxY2, GetColor(COLOR_COLOR_BLACK_R, COLOR_COLOR_BLACK_G, COLOR_COLOR_BLACK_B), TRUE);

	// タイトル
	DrawString(boxX1 + HELP_PADDING, boxY1 + HELP_PADDING, "【操作説明】", GetColor(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B));

	// 描画位置
	int labelX = boxX1 + HELP_LABEL_LEFT;
	int inputX = boxX1 + HELP_INPUT_LEFT;
	int y = boxY1 + HELP_CONTENT_TOP_OFFSET;

	for (int i = 0; i < lineCount; ++i)
	{
		DrawString(labelX, y + i * lineHeight, helpLabels[i], GetColor(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B));
		DrawString(inputX, y + i * lineHeight, "：", GetColor(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B));
		DrawString(inputX + HELP_INPUT_OFFSET, y + i * lineHeight, std::string(helpInputs[i]).c_str(), GetColor(COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B));
	}
	SetFontSize(DEFAULT_FONT_SIZE);
}