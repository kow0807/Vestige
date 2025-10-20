#include <EffekseerForDXLib.h>
#include "../../Manager/ResourceManager.h"
#include "Book.h"

Book::Book(void)
    :
    bookSe_(resMng_.Load(ResourceManager::SRC::BOOK_SE).handleId_),
    lightningSe_(resMng_.Load(ResourceManager::SRC::LIGHTNING_SE).handleId_),
    effectLightningResId_(ResourceManager::GetInstance().Load(ResourceManager::SRC::LIGHTNING_EFFECT).handleId_),
    effectLightningPlayId_(-1)
{
}

Book::~Book(void)
{
}

void Book::Init(void)
{
    ChangeVolumeSoundMem(220, bookSe_);

    ChangeVolumeSoundMem(220, lightningSe_);

    itemImg_ = resMng_.Load(ResourceManager::SRC::BOOK).handleId_;
    count_ = 1;

    float SCALE = 20.0f;
    SetScalePlayingEffekseer3DEffect(effectLightningPlayId_, SCALE, SCALE, SCALE);
}

void Book::Update(void)
{

}

void Book::Draw(void)
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
    DrawRotaGraph(100, 515, 0.08, 0.0, itemImg_, true);

    // 明るさを元に戻す
    SetDrawBright(255, 255, 255);

    // 「魔法書」のラベルと背景
    SetFontSize(18);
    const char* label = "魔法書";
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

void Book::Use(void)
{
    PlaySoundMem(bookSe_, DX_PLAYTYPE_BACK, true);
    count_--;
}

int Book::GetCount(void)
{
    return count_;
}

void Book::LightningUpdate(VECTOR pos)
{
    // エフェクト
    effectLightningPlayId_ = PlayEffekseer3DEffect(effectLightningResId_);
    // 大きさ
    float SCALE = 30.0f;
    SetScalePlayingEffekseer3DEffect(effectLightningPlayId_, SCALE, SCALE, SCALE);
    SetPosPlayingEffekseer3DEffect(effectLightningPlayId_, pos.x, pos.y - 200.0f, pos.z);
    PlaySoundMem(lightningSe_, DX_PLAYTYPE_BACK, true);
}
