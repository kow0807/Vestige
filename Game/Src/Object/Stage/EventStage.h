#pragma once
#include <map>
#include "../Common/Transform.h"
class ResourceManager;
class WarpStar;
class Planet;
class EventPlayer;
class EventEnemy;

class EventStage
{
public:

	// ステージの切り替え間隔
	static constexpr float TIME_STAGE_CHANGE = 1.0f;

	// ステージ名
	enum class NAME
	{
		MAIN_PLANET
	};

	// コンストラクタ
	EventStage(EventPlayer& player, EventEnemy& enemy);

	// デストラクタ
	~EventStage(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	// ステージ変更
	void ChangeStage(NAME type);

	// 対象ステージを取得
	std::weak_ptr<Planet> GetPlanet(NAME type);

private:

	// シングルトン参照
	ResourceManager& resMng_;

	EventPlayer& player_;

	EventEnemy& enemy_;

	// ステージアクティブになっている惑星の情報
	NAME activeName_;
	std::weak_ptr<Planet> activePlanet_;

	// 惑星
	std::map<NAME, std::shared_ptr<Planet>> planets_;

	// 空のPlanet
	std::shared_ptr<Planet> nullPlanet = nullptr;

	float step_;

	// 最初のステージ
	void MakeMainStage(void);
};

