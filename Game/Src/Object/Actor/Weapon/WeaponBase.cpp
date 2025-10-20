#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Common/Quaternion.h"
#include "WeaponBase.h"

WeaponBase::WeaponBase()
{
	//modelHandle_ = 0;
}

WeaponBase::~WeaponBase(void)
{

}

void WeaponBase::Init(void)
{
	ResourceManager& ins = ResourceManager::GetInstance();
	transform_.modelId = MV1DuplicateModel(
		ins.LoadModelDuplicate(ResourceManager::SRC::SWORD));
	MV1SetScale(transform_.modelId, { SCALE, SCALE, SCALE });

}

void WeaponBase::Update(void)
{

}

void WeaponBase::Draw(void)
{
	MV1DrawModel(transform_.modelId);
}

int WeaponBase::GetModelHandle(void)
{
	return transform_.modelId;
}

void WeaponBase::DrawDebug(void)
{
}
