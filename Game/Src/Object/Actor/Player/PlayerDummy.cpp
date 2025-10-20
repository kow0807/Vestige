#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../Player/Player.h"
#include "PlayerDummy.h"

PlayerDummy::PlayerDummy(Player& playerDummy) :player_(playerDummy)
{
	// ÉÇÉfÉãÇÃäÓñ{ê›íË
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_DUMMY));
	transform_.scl = { SIZE, SIZE, SIZE };
	transform_.pos = { 0.0f, -30.0f, 660.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_.Update();
}

PlayerDummy::~PlayerDummy(void)
{
}

void PlayerDummy::Init(void)
{
}

void PlayerDummy::Update(void)
{
	transform_.pos = player_.GetTransform().pos;
	transform_.Update();
}

void PlayerDummy::Draw(void)
{
	MV1DrawModel(transform_.modelId);
}
