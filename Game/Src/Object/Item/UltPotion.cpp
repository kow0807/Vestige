#include "../../Manager/ResourceManager.h"
#include "UltPotion.h"

UltPotion::UltPotion(void)
    :
    ultPotionSe_(resMng_.Load(ResourceManager::SRC::ULT_POTION_SE).handleId_)
{
}

UltPotion::~UltPotion(void)
{
}

void UltPotion::Init(void)
{
    ChangeVolumeSoundMem(220, ultPotionSe_);

    itemImg_ = resMng_.Load(ResourceManager::SRC::ULT_POTION).handleId_;
    count_ = 1;
}

void UltPotion::Update(void)
{
}

void UltPotion::Draw(void)
{
    // 背景
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DrawBox(UI_LEFT + 2, UI_TOP + 2, UI_RIGHT - 2, UI_BOTTOM - 2, 0x000000, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 枠線
    for (int i = 0; i < 3; ++i) {
        DrawBox(UI_LEFT - i, UI_TOP - i, UI_RIGHT + i, UI_BOTTOM + i, 0xffffff, false);
    }
    DrawBox(UI_LEFT + 3, UI_TOP + 3, UI_RIGHT - 3, UI_BOTTOM - 3, 0xaaaaaa, false);

    // 所持数0なら暗く表示
    if (count_ == 0) {
        SetDrawBright(100, 100, 100); // 暗めの色合いに
    }

    // アイテム画像
    DrawRotaGraph(100, 510, 0.08, 0.0, itemImg_, true);

    // 明るさを元に戻す
    SetDrawBright(255, 255, 255);

    // 「必殺薬」のラベルと背景
    SetFontSize(18);
    const char* label = "必殺薬";
    int textWidth = static_cast<int>(GetDrawStringWidth(label, strlen(label)));
    int textX = 100 - textWidth / 2;
    int textY = 590;
    int paddingX = 10;
    int paddingY = 4;

    // 背景帯（半透明黒）
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
    DrawBox(
        textX - paddingX,
        textY - paddingY,
        textX + textWidth + paddingX,
        textY + 16 + paddingY,
        0x000000, true
    );
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ラベル文字
    DrawString(textX, textY, label, 0xffffff);

    // 所持数の表示（通常表示）
    DrawFormatString(131, 561, 0x000000, "%d", count_);
    DrawFormatString(130, 560, 0xffffff, "%d", count_);
}

void UltPotion::Use(void)
{
    PlaySoundMem(ultPotionSe_, DX_PLAYTYPE_BACK, true);
    count_--;
}

int UltPotion::GetCount(void)
{
    return count_;
}
