#include "Battle/BattleMonster.h"
#include "DxLib.h"
#include "StdCommon.h"
#include "SharedCharacterStatus.h"

namespace Battle
{

BattleMonster::BattleMonster()
{
    // 敵の場合は毎度HP等ここで作成する
    scs_ = new ::SharedCharacterStatus();
    initializeParameter();
}

BattleMonster::~BattleMonster()
{
    delete scs_;
}

void BattleMonster::initializeParameter()
{
    x_ = BattleMonPosX1;
    y_ = BattleMonPosY1;
#ifdef _DEBUG
    scs_->hp_ = 500;
#else
    scs_->hp_ = 600 + DxLib::GetRand(20);
#endif
    scs_->hpMax_ = scs_->hp_;
    scs_->mp_ = 136;
    scs_->mpMax_ = 136;
    scs_->power_ = 20;               // 攻撃力
    scs_->defense_ = 20;             // 防御力
    scs_->strength_ = 20;            // 力
    scs_->magic_ = 20;
    scs_->weapon_ = 20;              // 武器の攻撃力
    scs_->equip_ = 20;               // 装備の防御力
    scs_->level_ = 10;
    
    tp_ = 0;
    cntSpeed_ = 1.0;              // カウンターが増加するスピード(double)
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
    animationFlag_ = false;            // アニメーションをするかのフラグ
    damageFlag_    = false;            // ダメージ表示するかのフラグ

}

}