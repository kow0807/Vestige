#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../Player/Player.h"
#include "EnemyScarecrow.h"

EnemyScarecrow::EnemyScarecrow(Player& player) 
    :
    player_(player),
    hp_(0),
    delayedHpRate_(0.0f),
    isAlive_(false)
{
}

EnemyScarecrow::~EnemyScarecrow(void)
{
}

void EnemyScarecrow::Init(void)
{
	// モデルの基本設定
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_SCARECROW));
	transform_.scl = { SIZE, SIZE, SIZE };
	transform_.pos = INIT_POS;
	transform_.quaRot = Quaternion();
    transform_.quaRotLocal =
        Quaternion::Euler(INIT_LOCAL_ROT);
	transform_.Update();

	hp_ = MAX_HP;
    delayedHpRate_ = HP_RATE;
	isAlive_ = true;
}

void EnemyScarecrow::Update(void)
{
    if (hp_ <= 0)
    {
        isAlive_ = false;
        hp_ = 0;  // 0未満にならないように念のため
    }

    // メインゲージの割合
    float hpRate = static_cast<float>(hp_) / MAX_HP;

    // 追従ゲージの割合を少しずつメインゲージに近づける
    const float followSpeed = FOLLOW_SPEED;  // 減る速度。数値を調整して好みのスピードに
    if (delayedHpRate_ > hpRate)
    {
        delayedHpRate_ -= followSpeed;
        if (delayedHpRate_ < hpRate)
        {
            delayedHpRate_ = hpRate;
        }
    }
    else
    {
        // HP回復時は追従ゲージもすぐに戻す（必要に応じて）
        delayedHpRate_ = hpRate;
    }

    transform_.Update();
}

void EnemyScarecrow::Draw(void)
{
	MV1DrawModel(transform_.modelId);
}

void EnemyScarecrow::DrawHpBar(void)
{
    int width = WIDTH;
    int height = HEIGHT;  // ボスと同じ高さに

    int x = (Application::SCREEN_SIZE_X - width) / 2 + X_ADJUSTMENT;
    int y = Application::SCREEN_SIZE_Y - height - Y_ADJUSTMENT;

    float hpRate = static_cast<float>(hp_) / MAX_HP;
    int filledWidth = static_cast<int>(width * hpRate);

    int delayedWidth = static_cast<int>(width * delayedHpRate_);

    // 敵の名前表示準備
    const char* enemyName = "訓練用のかかし";
    SetFontSize(FONT_SIZE);
    int nameWidth = GetDrawStringWidth(enemyName, static_cast<int>(strlen(enemyName)));
    int nameHeight = NAME_HEIGHT;

    int paddingLeft = PADDING_LEFT;
    int nameX = x + paddingLeft;
    int nameY = y - nameHeight - PADDING;

    int padding = PADDING;
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, NAME_BLEND_PARAM);
    DrawBox(nameX - padding / 2, nameY - padding / 2,
        nameX + nameWidth + padding, nameY + nameHeight + padding / 2,
        GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    int outlineColor = GetColor(0, 0, 0);
    int textColor = GetColor(TEXCOL_R, TEXCOL_G, TEXCOL_B);
    DrawString(nameX - 1, nameY, enemyName, outlineColor);
    DrawString(nameX + 1, nameY, enemyName, outlineColor);
    DrawString(nameX, nameY - 1, enemyName, outlineColor);
    DrawString(nameX, nameY + 1, enemyName, outlineColor);
    DrawString(nameX, nameY, enemyName, textColor);

    // 枠色設定
    int borderDark = GetColor(BORDER_DARK_COL, BORDER_DARK_COL, BORDER_DARK_COL);
    int borderLight = GetColor(BORDER_LIGHT_COL, BORDER_LIGHT_COL, BORDER_LIGHT_COL);
    int bgColor = GetColor(A_QUARTER_COL, A_QUARTER_COL, A_QUARTER_COL);

    // ダメージ色（追従ゲージ）
    int delayedColorLight = GetColor(DELAYED_COL_LIGHT_R, DELAYED_COL_LIGHT, DELAYED_COL_LIGHT);
    int delayedColorTop = GetColor(DELAYED_COL_TOP_R, DELAYED_COL_TOP_GB, DELAYED_COL_TOP_GB);
    int delayedColorBottom = GetColor(DELAYED_COL_BOTTOM_R, DELAYED_COL_BOTTOM_GB, DELAYED_COL_BOTTOM_GB);

    // HPバー色（上下グラデ）
    int hpColorTop = GetColor(HP_COL_TOP_R, HP_COL_TOP_GB, HP_COL_TOP_GB);
    int hpColorBottom = GetColor(HP_COL_BOTTOM_R, HP_COL_BOTTOM_GB, HP_COL_BOTTOM_GB);

    // 枠2重描画
    DrawBox(x - BORDER_DARK_ADJUSTMENT, y - BORDER_DARK_ADJUSTMENT, x + width + BORDER_DARK_ADJUSTMENT, y + height + BORDER_DARK_ADJUSTMENT, borderDark, TRUE);
    DrawBox(x - BORDER_LIGHT_ADJUSTMENT, y - BORDER_LIGHT_ADJUSTMENT, x + width + BORDER_LIGHT_ADJUSTMENT, y + height + BORDER_LIGHT_ADJUSTMENT, borderLight, TRUE);

    // 背景バー
    DrawBox(x, y, x + width, y + height, bgColor, TRUE);

    // 追従ゲージをHPバーの右側に半透明グラデで描画
    if (delayedWidth > filledWidth)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, DAMAGE_BLEND_PARAM);

        int halfH = height / 2;
        // 白ベースっぽく明るめ背景
        DrawBox(x + filledWidth, y, x + delayedWidth, y + height, delayedColorLight, TRUE);
        DrawBox(x + filledWidth, y, x + delayedWidth, y + halfH, delayedColorTop, TRUE);
        DrawBox(x + filledWidth, y + halfH, x + delayedWidth, y + height, delayedColorBottom, TRUE);

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // HPバー（上下グラデ）
    if (filledWidth > 0)
    {
        int halfH = height / 2;
        DrawBox(x, y, x + filledWidth, y + halfH, hpColorTop, TRUE);
        DrawBox(x, y + halfH, x + filledWidth, y + height, hpColorBottom, TRUE);
    }

    // 枠線（白）
    DrawBox(x, y, x + width, y + height, GetColor(MAX_COL, MAX_COL, MAX_COL), FALSE);

    SetFontSize(DEFAULT_FONT_SIZE);
}

void EnemyScarecrow::Damage(int damage)
{
	hp_ -= damage;
}

bool EnemyScarecrow::IsAlive(void)
{
	return isAlive_;
}
