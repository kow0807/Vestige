#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "ItemBase.h"

ItemBase::ItemBase(void)
	: resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance())
{
}

ItemBase::~ItemBase(void)
{
	transform_.Release();
}

const Transform& ItemBase::GetTransform(void) const
{
	return transform_;
}
