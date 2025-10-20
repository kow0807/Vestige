#pragma once
#include <DxLib.h>
#include "../ActorBase.h"

class WeaponBase :public ActorBase
{
public:

	// •Ší‚Ì‘å‚«‚³
	static constexpr float SCALE = 0.1f;

	WeaponBase();
	virtual ~WeaponBase(void);

	virtual void Init(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;


	int GetModelHandle(void);

protected:

	void DrawDebug(void);
};