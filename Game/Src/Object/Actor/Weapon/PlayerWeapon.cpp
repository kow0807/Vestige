#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Common/Quaternion.h"
#include "../../../Utility/AsoUtility.h"
#include "PlayerWeapon.h"

PlayerWeapon::PlayerWeapon(void)
{
}

PlayerWeapon::~PlayerWeapon()
{
}

void PlayerWeapon::Init(void)
{
	WeaponBase::Init();
}

void PlayerWeapon::Update(void)
{
	WeaponBase::Update();
}

void PlayerWeapon::Draw(void)
{
	WeaponBase::Draw();
}
