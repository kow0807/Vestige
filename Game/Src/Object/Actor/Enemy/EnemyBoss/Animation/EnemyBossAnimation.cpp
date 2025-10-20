#include "../../../../../Application.h"
#include "../../../../Common/AnimationController.h"
#include "EnemyBossAnimation.h"

EnemyBossAnimation::EnemyBossAnimation(int modelId) :modelId_(modelId), controller_(std::make_unique<AnimationController>(modelId_)), currentAnim_(ANIM_TYPE::NONE)
{
}

EnemyBossAnimation::~EnemyBossAnimation()
{
}

void EnemyBossAnimation::AddAnimation(ANIM_TYPE type, const std::string& path, float speed)
{
	int animType = static_cast<int>(type);
	std::string fPath = Application::PATH_MODEL + "Enemy/";

	if (!fPath.empty() && fPath.back() != '/' && fPath.back() != '\\') {
		fPath += "/";
	}

	std::string fullPath = fPath + path;
	int modelId = MV1LoadModel(fullPath.c_str());

	controller_->Add((int)type, fPath + path, speed);
	animIdMap_[type] = animType;
}

void EnemyBossAnimation::Play(ANIM_TYPE anim, bool loop, float blendRate, float startStep, float endStep, bool isStop, bool force)
{
	if (currentAnim_ == anim && !force)return;

	int animType = animIdMap_[anim];
	controller_->Play((int)anim, loop, blendRate, startStep, endStep, isStop, force);
	currentAnim_ = anim;
}

bool EnemyBossAnimation::IsEnd() const
{
	if (controller_->GetPlayType() != (int)currentAnim_)return false;
	return controller_->IsEnd();
}

void EnemyBossAnimation::Update()
{
	controller_->Update();
}

EnemyBossAnimation::ANIM_TYPE EnemyBossAnimation::GetCurrentAnim() const
{
	return currentAnim_;
}
