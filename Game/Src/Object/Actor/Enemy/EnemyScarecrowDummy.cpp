#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "EnemyScarecrow.h"
#include "EnemyScarecrowDummy.h"

EnemyScarecrowDummy::EnemyScarecrowDummy(EnemyScarecrow& enemyScarecrow) :enemyScarecrow_(enemyScarecrow)
{
	// ÉÇÉfÉãÇÃäÓñ{ê›íË
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::ENEMY_DUMMY));
	transform_.scl = { SIZE, SIZE, SIZE };
	transform_.pos = INIT_POS;
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 
			INIT_QUAROT_LOCAL_X,
			AsoUtility::Deg2RadF(INIT_QUAROT_LOCAL_Y),
			INIT_QUAROT_LOCAL_Z 
			});
	transform_.Update();
}

EnemyScarecrowDummy::~EnemyScarecrowDummy(void)
{
}

void EnemyScarecrowDummy::Init(void)
{
}

void EnemyScarecrowDummy::Update(void)
{
	transform_.pos.x = enemyScarecrow_.GetTransform().pos.x;
	transform_.pos.y = enemyScarecrow_.GetTransform().pos.y - POS_Y_ADJUSTMENT;
	transform_.pos.z = enemyScarecrow_.GetTransform().pos.z;
	transform_.Update();
}

void EnemyScarecrowDummy::Draw(void)
{
	MV1DrawModel(transform_.modelId);
}
