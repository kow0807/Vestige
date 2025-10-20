#pragma once
#include "WeaponBase.h"

class EnemyWeapon :
    public WeaponBase
{
public:
    static constexpr float SIZE = 0.15f;
    static constexpr float QUAROT_LOCAL = 0.15f;

    EnemyWeapon(void);
    ~EnemyWeapon();

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;

private:
};

