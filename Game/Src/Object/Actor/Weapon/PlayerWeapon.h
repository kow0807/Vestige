#pragma once
#include "WeaponBase.h"

class PlayerWeapon :
    public WeaponBase
{
public:
    PlayerWeapon(void);
    ~PlayerWeapon();

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;

private:
};

