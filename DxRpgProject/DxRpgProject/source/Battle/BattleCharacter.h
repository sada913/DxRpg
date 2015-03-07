#pragma once
#include "AbstractBattleCharacter.h"

namespace Battle
{
const int    BattleCharPosX1 = 500;
const int    BattleCharPosY1 = 200;
class BattleCharacter : public AbstractBattleCharacter
{
public:
    BattleCharacter(SharedCharacterStatus *scs);
    ~BattleCharacter();

    void    pressedKeyUp();
    void    pressedKeyDown();
    void    pressedKeyX();
    void    pressedKeyZ();
    void    resetSelectState();
    void    changeAbility();

private:
    void    initializeParameter() override;
};

}
