#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/SceneManager.h"
#include "EnemyBoss.h"
#include "EnemyBossDummy.h"

EnemyBossDummy::EnemyBossDummy(EnemyBoss& enemyBoss)
	:
	enemyBoss_(enemyBoss),
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	transform_({})
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

EnemyBossDummy::~EnemyBossDummy(void)
{
}

void EnemyBossDummy::Init(void)
{
}

void EnemyBossDummy::Update(void)
{
	transform_.pos = enemyBoss_.GetTransform().pos;
	transform_.Update();
}

void EnemyBossDummy::Draw(void)
{
	MV1DrawModel(transform_.modelId);
}

const Transform& EnemyBossDummy::GetTransform(void) const
{
	return transform_;
}
