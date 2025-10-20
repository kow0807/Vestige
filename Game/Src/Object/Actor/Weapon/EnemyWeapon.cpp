#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Common/Quaternion.h"
#include "../../../Utility/AsoUtility.h"
#include "EnemyWeapon.h"

EnemyWeapon::EnemyWeapon(void)
{
}

EnemyWeapon::~EnemyWeapon()
{
}

void EnemyWeapon::Init(void)
{
	WeaponBase::Init();
	transform_.scl = { SIZE,SIZE ,SIZE };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(QUAROT_LOCAL), 0.0f });
}

void EnemyWeapon::Update(void)
{
	WeaponBase::Update();
}

void EnemyWeapon::Draw(void)
{
	WeaponBase::Draw();
}
