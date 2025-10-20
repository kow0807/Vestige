#pragma once
#include "SceneBase.h"
class Stage;
class SkyDome;
class Player;
class PlayerDummy;
class EnemyBoss;
class EnemyBossDummy;
class Mist;
class PixelMaterial;
class PixelRenderer;


class GameScene : public SceneBase
{

public:

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void DrawUI(void) override;
	void UpdateCamera(VECTOR& pos, VECTOR& targetPos, VECTOR& cameraUp)override;

	static constexpr int VISION_CONST_BUF_FLOAT4_SIZE = 2;		// 定数バッファの確保サイズ(視界)
	static constexpr int LIGHTNING_CONST_BUF_FLOAT4_SIZE = 1;		// 定数バッファの確保サイズ(ライトニング)

private:

	// 音
	int gameBgm_;
	int bgmVol_;
	bool isBgm_;
	int deadSe_;
	int helpSe_;

	// ステージ
	std::unique_ptr<Stage> stage_;

	// スカイドーム
	std::unique_ptr<SkyDome> skyDome_;

	std::shared_ptr<Mist> mist_;

	// プレイヤー
	std::shared_ptr<Player> player_;

	std::shared_ptr<PlayerDummy> playerDummy_;

	std::shared_ptr<EnemyBoss> enemyBoss_;

	std::shared_ptr<EnemyBossDummy> enemyBossDummy_;

	int deathFadeAlpha_;
	int youDiedTimer_;

	int enemySlainFadeAlpha_;
	int enemySlainTimer_;
	bool enemySlain_;

	// ポストエフェクト用スクリーン
	int postEffectScreen_;

	// ポストエフェクト用(プレイヤーの視界)
	std::unique_ptr<PixelMaterial> lowHpVisionMaterial_;
	std::unique_ptr<PixelRenderer> lowHpVisionRenderer_;

	// ポストエフェクト用(ライトニング)
	std::unique_ptr<PixelMaterial> lightningMaterial_;
	std::unique_ptr<PixelRenderer> lightningRenderer_;
	float lightningTime_;
	float lightningPower_;

	bool isHelp_;
	int helpBackImage_;
	bool wasHelp_;
	void DrawHelp(void);
};
