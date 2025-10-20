#pragma once
#include <map>
#include <string>
#include <memory>
#include "Resource.h"

class ResourceManager
{

public:

	// リソース名
	enum class SRC
	{
		TITLE,
		OVER,
		CLEAR,
		LIGHT,
		PUSH_SPACE,
		SPEECH_BALLOON,
		PLAYER,
		PLAYER_DUMMY,
		PLAYER_SHADOW,
		GAUGE_FRAME,
		MAGIC_CIRCLE,
		RUNE,
		HP_POTION,
		ULT_POTION,
		BOOK,
		NOW_LOADING,
		ENEMY_BASE,
		ENEMY_DUMMY,
		SKY_DOME,
		MAIN_PLANET,
		MIST,
		FOOT_SMOKE,
		WARP_STAR,
		WARP_STAR_ROT_EFF,
		WARP_ORBIT,
		BLACK_HOLE,
		GOAL_STAR,
		TANK_BODY,
		TANK_BARREL,
		TANK_WHEEL,
		SWORD,
		ENEMY_BOSS,
		ENEMY_SCARECROW,
		ULT_EFFECT,
		HIT_EFFECT,
		LIGHTNING_EFFECT,
		BUFF_EFFECT,
		EXPLOSION_EFFECT,
		ROAR_01,
		ROAR_02,
		STUN_EFFECT,

		TITLE_BGM,
		TUTORIAL_BGM,
		GAME_BGM,
		CLEAR_BGM,
		OVER_BGM,
		CLICK_SE,
		ROLLING_SE,
		ATTACK_SE,
		HIT_SE,
		LOCKON_SE,
		ULT_SE,
		DAMAGE_SE,
		HP_POTION_SE,
		ULT_POTION_SE,
		BOOK_SE,
		LIGHTNING_SE,
		NEXT_SE,
		DEAD_SE,
		ROAR_SE,
		EXP_SE,
		UNSHEATHED_BGM,
		WALK_BGM,
		GAUGE_BGM1,
		GAUGE_BGM2,
		EVENT_BGM,
		HELP_SE,
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// リソース管理の対象
	std::map<SRC, std::shared_ptr<Resource>> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// 内部ロード
	Resource& _Load(SRC src);

};
