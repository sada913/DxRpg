#include "Battle/BattleCharacter.h"
#include "StdCommon.h"
#include "SharedCharacterStatus.h"
#include <cstdlib>

namespace Battle
{

BattleCharacter::BattleCharacter(SharedCharacterStatus *scs)
{
    scs_ = scs;
    initializeParameter();
}


BattleCharacter::~BattleCharacter()
{
}

void BattleCharacter::initializeParameter()
{
    x_ = BattleCharPosX1;
    y_ = BattleCharPosY1;

    tp_ = 0;
    cntSpeed_ = 1.0;
    for (int i = 0; i < sizeof(abiFlag_); i++)
    {
        abiFlag_[i] = 0;
    }
    atbCnt_ = DxLib::GetRand(235);  // カウンター240で行動可
    atbCntFlag_ = true;
    motionFlag_ = false;
    toHp_ = scs_->hp_;
    menuSelect_ = 1;
    menuSelectY_ = 0;
    actionCnt_ = 0;
    damageActionCnt_ = 0;
    actionFlag_ = false;
    graphFlag_ = true;

    animationFlag_ = false;
    damageFlag_ = false;
}

void BattleCharacter::pressedKeyUp()
{
    menuSelect_--;
    menuSelectY_--;
    if (menuSelectY_ == -1) // 選択位置が一番上を超えたら
    {
        menuSelect_ += 5;     // 一番下へ 5は選択項目の最大数
        menuSelectY_ = 4;   // 一番下へ 4は選択表示項目の最大数-1
    }
}

void BattleCharacter::pressedKeyDown()
{
    menuSelect_++;
    menuSelectY_++;
    if (menuSelectY_ == 5) // 選択位置が一番上を超えたら
    {
        menuSelect_ -= 5;    // 一番上へ 5は選択項目の最大数
        menuSelectY_ = 0;   // 一番上へ 4は選択表示項目の最大数-1
    }
}

void BattleCharacter::pressedKeyX()
{
    menuSelect_ = menuSelect_ * 100;    // 決定ボタンが押されるたびに100倍に
    menuSelectY_ = 0;                    // 選択位置を初期状態に
}

void BattleCharacter::pressedKeyZ()
{
    menuSelect_ = 1;                    // 選択状態を戻す 
    menuSelectY_ = 0;                    // 選択位置を初期状態に
}

void BattleCharacter::resetSelectState()
{
    switch (menuSelect_)
    {
        case 10200:
        case 10300:
        case 10400:
            menuSelect_ = 10000;
            break;
        case 30000:
            menuSelect_ = 10000;
            break;
        case 40000:
        case 40100:
        case 40200:
        case 40300:
        case 40400:
            menuSelect_ = 10000;
            break;
        case 50000:
        case 50100:
        case 50200:
        case 50300:
        case 50400:
            menuSelect_ = 10000;
            break;
    }
}

}