#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/Stage/SkyDome.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	imgTitle_(resMng_.Load(ResourceManager::SRC::TITLE).handleId_),
	titleBgm_(resMng_.Load(ResourceManager::SRC::TITLE_BGM).handleId_),
	clickSe_(resMng_.Load(ResourceManager::SRC::CLICK_SE).handleId_),
	particles_{},
	menuSelected_(0),
	menuItems_{},
	menuEffectPhase_(0.0f),
	isConfirmingTutorial_(false),
	confirmSelected_(0),
	prevPOVDirection_(-1),
	highlightPosY_(INIT_MENU_HIGHLIGHT_Y),
	confirmHighlightPosX_(0.0f),
	confirmHighlightPosY_(0.0f)
{

}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	ChangeFont("Garamond");

	ChangeVolumeSoundMem(TITLE_BGM_VOL, titleBgm_);
	ChangeVolumeSoundMem(CLICK_SE_VOL, clickSe_);
	PlaySoundMem(titleBgm_, DX_PLAYTYPE_LOOP, true);

	// 定点カメラ
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles_[i].x = static_cast<float>(rand() % Application::SCREEN_SIZE_X);
		particles_[i].y = static_cast<float>(rand() % Application::SCREEN_SIZE_Y);
		particles_[i].vx = ((rand() % PARTICLE_VX_RAND_MAX) / PARTICLE_VX_DIVISOR - PARTICLE_VX_CENTER) * PARTICLE_VX_RANGE;
		particles_[i].vy = PARTICLE_VY_BASE + (rand() % PARTICLE_VY_RAND_MAX) / PARTICLE_VY_DIVISOR;
		particles_[i].size = PARTICLE_SIZE_MIN + rand() % PARTICLE_SIZE_VARIATION;
		particles_[i].alpha = PARTICLE_ALPHA_MIN + rand() % PARTICLE_ALPHA_VARIATION;
	}

	menuItems_[0] = "Start Game";
	menuItems_[1] = "Exit";
}

void TitleScene::Update(void)
{
	auto& ins = InputManager::GetInstance();
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);
	bool mouseClicked = ins.IsTrgMouseLeft();

	DINPUT_JOYSTATE input;
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	int currentPOV = input.POV[0];
	bool currentAPressed = (input.Buttons[0] != 0);

	float stickY = input.Y / STICK_SCALE;
	float stickX = input.X / STICK_SCALE;

	static bool stickUpPrev = false;
	static bool stickDownPrev = false;
	static bool stickLeftPrev = false;
	static bool stickRightPrev = false;

	bool stickUp = stickY < -STICK_THRESHOLD;
	bool stickDown = stickY > STICK_THRESHOLD;
	bool stickLeft = stickX < -STICK_THRESHOLD;
	bool stickRight = stickX > STICK_THRESHOLD;

	bool stickUpPressed = (stickUp && !stickUpPrev);
	bool stickDownPressed = (stickDown && !stickDownPrev);
	bool stickLeftPressed = (stickLeft && !stickLeftPrev);
	bool stickRightPressed = (stickRight && !stickRightPrev);

	bool upPressed = (currentPOV == POV_UP && prevPOVDirection_ != 0) || stickUpPressed;
	bool downPressed = (currentPOV == POV_DOWN && prevPOVDirection_ != POV_DOWN) || stickDownPressed;
	bool leftPressed = (currentPOV == POV_LEFT && prevPOVDirection_ != POV_LEFT) || stickLeftPressed;
	bool rightPressed = (currentPOV == POV_RIGHT && prevPOVDirection_ != POV_RIGHT) || stickRightPressed;
	bool aPressed = ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (isConfirmingTutorial_) {
		if (leftPressed || rightPressed) {
			confirmSelected_ = 1 - confirmSelected_;
		}
		if (aPressed) {
			PlaySoundMem(clickSe_, DX_PLAYTYPE_BACK, true);
			if (confirmSelected_ == 0)
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TUTORIAL);
			else
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::EVENT);
			goto EndInputUpdate;
		}

		// マウスでの選択
		SetFontSize(FONT_SIZE_CONFIRM);
		const char* options[CONFIRM_OPTION_COUNT] = { "はい", "いいえ" };
		int optionWidths[CONFIRM_OPTION_COUNT] = {
			static_cast<int>(GetDrawStringWidth(options[0], strlen(options[0]))),
			static_cast<int>(GetDrawStringWidth(options[1], strlen(options[1])))
		};
		int totalWidth = optionWidths[0] + optionWidths[1] + OPTION_PADDING;
		int startX = (Application::SCREEN_SIZE_X - totalWidth) / 2;

		for (int i = 0; i < CONFIRM_OPTION_COUNT; ++i) {
			int optX = startX;
			for (int j = 0; j < i; ++j)
				optX += optionWidths[j] + OPTION_PADDING;

			int optW = optionWidths[i];

			if (mouseX >= optX && mouseX <= optX + optW &&
				mouseY >= OPTION_Y_POS && mouseY <= OPTION_Y_POS + OPTION_HEIGHT) {
				confirmSelected_ = i;
				if (mouseClicked) {
					PlaySoundMem(clickSe_, DX_PLAYTYPE_BACK, true);
					StopSoundMem(titleBgm_);
					if (i == 0)
						SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TUTORIAL);
					else
						SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::EVENT);
					goto EndInputUpdate;
				}
			}
		}
		SetFontSize(FONT_SIZE_DEFAULT);

		int targetX = startX;
		for (int j = 0; j < confirmSelected_; ++j)
			targetX += optionWidths[j] + OPTION_PADDING;
		confirmHighlightPosX_ += (targetX - confirmHighlightPosX_) * CONFIRM_LERP_SPEED;
		confirmHighlightPosY_ += (OPTION_Y_POS - confirmHighlightPosY_) * CONFIRM_LERP_SPEED;
	}
	else {
		if (upPressed || downPressed) {
			menuSelected_ = (menuSelected_ + 1) % 2;
		}
		if (aPressed) {
			PlaySoundMem(clickSe_, DX_PLAYTYPE_BACK, true);
			if (menuSelected_ == 0) {
				ChangeFont("MS 明朝");
				confirmSelected_ = 0;

				SetFontSize(FONT_SIZE_CONFIRM);
				const char* options[MENU_ITEM_COUNT] = { "はい", "いいえ" };
				int optionWidths[MENU_ITEM_COUNT] = {
					static_cast<int>(GetDrawStringWidth(options[0], strlen(options[0]))),
					static_cast<int>(GetDrawStringWidth(options[1], strlen(options[1])))
				};
				int totalWidth = optionWidths[0] + optionWidths[1] + OPTION_PADDING;
				int startX = (Application::SCREEN_SIZE_X - totalWidth) / 2;

				int initX = startX;
				for (int j = 0; j < confirmSelected_; ++j)
					initX += optionWidths[j] + OPTION_PADDING;

				confirmHighlightPosX_ = (float)initX;
				confirmHighlightPosY_ = (float)OPTION_Y_POS;

				isConfirmingTutorial_ = true;  // ← フラグは最後に
			}
			else {
				SceneManager::GetInstance().SetGameEnd(true);
			}
			goto EndInputUpdate;
		}

		// マウスでのメニュー選択
		SetFontSize(FONT_SIZE_MENU);
		for (int i = 0; i < MENU_ITEM_COUNT; ++i) {
			const char* text = menuItems_[i];
			int textWidth = static_cast<int>(GetDrawStringWidth(text, strlen(text)));

			int x = (Application::SCREEN_SIZE_X - textWidth) / 2;
			int y = MENU_BASE_Y + i * MENU_ITEM_INTERVAL;

			int rectX1 = x - MENU_PADDING_X;
			int rectX2 = x + textWidth + MENU_PADDING_X;
			int rectY1 = y - MENU_PADDING_Y;
			int rectY2 = y + MENU_TEXT_HEIGHT + MENU_PADDING_Y;

			if (mouseX >= rectX1 && mouseX <= rectX2 &&
				mouseY >= rectY1 && mouseY <= rectY2) {
				menuSelected_ = i;
				if (mouseClicked) {
					PlaySoundMem(clickSe_, DX_PLAYTYPE_BACK, true);
					if (i == 0) {
						ChangeFont("MS 明朝");
						confirmSelected_ = 0;

						SetFontSize(FONT_SIZE_CONFIRM);
						const char* options[MENU_ITEM_COUNT] = { "はい", "いいえ" };
						int optionWidths[MENU_ITEM_COUNT] = {
							static_cast<int>(GetDrawStringWidth(options[0], strlen(options[0]))),
							static_cast<int>(GetDrawStringWidth(options[1], strlen(options[1])))
						};
						int totalWidth = optionWidths[0] + optionWidths[1] + OPTION_PADDING;
						int startX = (Application::SCREEN_SIZE_X - totalWidth) / 2;

						int initX = startX;
						for (int j = 0; j < confirmSelected_; ++j)
							initX += optionWidths[j] + OPTION_PADDING;

						confirmHighlightPosX_ = (float)initX;
						confirmHighlightPosY_ = (float)OPTION_Y_POS;

						isConfirmingTutorial_ = true;
					}
					else {
						SceneManager::GetInstance().SetGameEnd(true);
					}
					goto EndInputUpdate;
				}
			}
		}
		SetFontSize(FONT_SIZE_DEFAULT);

		int targetY = MENU_BASE_Y + menuSelected_ * MENU_ITEM_INTERVAL;
		highlightPosY_ += (targetY - highlightPosY_) * MENU_LERP_SPEED;
	}

EndInputUpdate:
	prevPOVDirection_ = currentPOV;
	stickUpPrev = stickUp;
	stickDownPrev = stickDown;
	stickLeftPrev = stickLeft;
	stickRightPrev = stickRight;
}

void TitleScene::Draw(void)
{
	// タイトル画像描画
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, TITLE_IMAGE_POS_Y, 1.0, 0.0, imgTitle_, true);

	// 灰パーティクル描画
	DrawParticles();

	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	if (isConfirmingTutorial_) {
		// --- チュートリアル確認画面の描画 ---
		const char* question = "チュートリアルを受けますか？";
		const char* options[CONFIRM_OPTION_COUNT] = { "はい", "いいえ" };

		SetFontSize(FONT_SIZE_CONFIRM);
		int qWidth = static_cast<int>(GetDrawStringWidth(question, strlen(question)));
		int qX = (Application::SCREEN_SIZE_X - qWidth) / 2;

		DrawString(qX + SHADOW_OFFSET, CONFIRM_QUESTION_Y + SHADOW_OFFSET, question, GetColor(COLOR_TEXT_SHADOW_R, COLOR_TEXT_SHADOW_G, COLOR_TEXT_SHADOW_B));
		DrawString(qX, CONFIRM_QUESTION_Y, question, GetColor(COLOR_TEXT_MAIN_R, COLOR_TEXT_MAIN_G, COLOR_TEXT_MAIN_B));

		int optionWidths[CONFIRM_OPTION_COUNT] = {
			static_cast<int>(GetDrawStringWidth(options[0], strlen(options[0]))),
			static_cast<int>(GetDrawStringWidth(options[1], strlen(options[1])))
		};

		int totalWidth = optionWidths[0] + optionWidths[1] + OPTION_PADDING;
		int startX = (Application::SCREEN_SIZE_X - totalWidth) / 2;

		// 背景ボックスを滑らかに動かす位置で描画
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALPHA_MENU_HIGHLIGHT);
		DrawBox((int)(confirmHighlightPosX_ - HIGHLIGHT_BOX_MARGIN_X), (int)(confirmHighlightPosY_ - HIGHLIGHT_BOX_MARGIN_Y),
			(int)(confirmHighlightPosX_ + optionWidths[confirmSelected_] + HIGHLIGHT_BOX_MARGIN_X), (int)(confirmHighlightPosY_ + OPTION_HEIGHT),
			GetColor(COLOR_HIGHLIGHT_BG_R, COLOR_HIGHLIGHT_BG_G, COLOR_HIGHLIGHT_BG_B), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		for (int i = 0; i < CONFIRM_OPTION_COUNT; ++i) {
			int optX = startX;
			for (int j = 0; j < i; ++j)
				optX += optionWidths[j] + OPTION_PADDING;

			bool isHovered = (mouseX >= optX && mouseX <= optX + optionWidths[i] &&
				mouseY >= OPTION_Y_POS && mouseY <= OPTION_Y_POS + OPTION_HEIGHT);

			int textColor = (isHovered || i == confirmSelected_) ? GetColor(COLOR_MENU_HIGHLIGHT_R, COLOR_MENU_HIGHLIGHT_G, COLOR_MENU_HIGHLIGHT_B) : GetColor(COLOR_MENU_NORMAL_R, COLOR_MENU_NORMAL_G, COLOR_MENU_NORMAL_B);
			DrawString(optX + SHADOW_OFFSET, OPTION_Y_POS + SHADOW_OFFSET, options[i], GetColor(COLOR_TEXT_SHADOW_R, COLOR_TEXT_SHADOW_G, COLOR_TEXT_SHADOW_B));
			DrawString(optX, OPTION_Y_POS, options[i], textColor);
		}

		SetFontSize(FONT_SIZE_DEFAULT);
		return;
	}

	// --- 通常メニュー描画 ---
	SetFontSize(FONT_SIZE_MENU);

	for (int i = 0; i < MENU_ITEM_COUNT; ++i)
	{
		const char* text = menuItems_[i];
		int textWidth = static_cast<int>(GetDrawStringWidth(text, strlen(text)));
		int x = (Application::SCREEN_SIZE_X - textWidth) / 2;
		int y = MENU_BASE_Y + i * MENU_ITEM_INTERVAL;

		int rectX1 = x - MENU_PADDING_X;
		int rectX2 = x + textWidth + MENU_PADDING_X;
		int rectY1 = y - MENU_PADDING_Y;
		int rectY2 = y + FONT_SIZE_MENU + MENU_PADDING_Y;

		bool isHovered = (mouseX >= rectX1 && mouseX <= rectX2 &&
			mouseY >= rectY1 && mouseY <= rectY2);

		if (i == menuSelected_) {
			float glowOffsetY = sin(menuEffectPhase_) * GLOW_OFFSET_Y;
			int boxY1 = (int)(highlightPosY_ - MENU_PADDING_Y + glowOffsetY);
			int boxY2 = (int)(highlightPosY_ + FONT_SIZE_MENU + MENU_PADDING_Y + glowOffsetY);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALPHA_MENU_NORMAL);
			DrawBox(rectX1, boxY1, rectX2, boxY2, GetColor(COLOR_SELECTED_BG_R, COLOR_SELECTED_BG_G, COLOR_SELECTED_BG_B), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		int textColor = (isHovered || i == menuSelected_) ? GetColor(COLOR_MENU_HIGHLIGHT_R, COLOR_MENU_HIGHLIGHT_G, COLOR_MENU_HIGHLIGHT_B) : GetColor(COLOR_MENU_NORMAL_R, COLOR_MENU_NORMAL_G, COLOR_MENU_NORMAL_B);
		DrawString(x + SHADOW_OFFSET, y + SHADOW_OFFSET, text, GetColor(COLOR_TEXT_SHADOW_R, COLOR_TEXT_SHADOW_G, COLOR_TEXT_SHADOW_B));
		DrawString(x, y, text, textColor);
	}
}

void TitleScene::DrawUI(void)
{
}

void TitleScene::UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)
{
}

void TitleScene::DrawParticles(void)
{
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles_[i].x += particles_[i].vx;
		particles_[i].y += particles_[i].vy;

		if (particles_[i].y > Application::SCREEN_SIZE_Y) particles_[i].y = 0;
		if (particles_[i].x < 0) particles_[i].x = Application::SCREEN_SIZE_X;
		if (particles_[i].x > Application::SCREEN_SIZE_X) particles_[i].x = 0;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, particles_[i].alpha);
		DrawCircle((int)particles_[i].x, (int)particles_[i].y, particles_[i].size, GetColor(COLOR_PARTICLE_R, COLOR_PARTICLE_G, COLOR_PARTICLE_B), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}