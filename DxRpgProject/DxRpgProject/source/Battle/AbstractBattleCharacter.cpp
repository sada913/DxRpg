#include "AbstractBattleCharacter.h"
#include "StdCommon.h"

namespace Battle
{

AbstractBattleCharacter::AbstractBattleCharacter()
    :x_(0), y_(0),
    tp_(0), cntSpeed_(0.0),
    atbCnt_(0), atbCntFlag_(false), motionFlag_(false), /*toHp_(0),*/
    menuSelect_(0), menuSelectY_(0), actionCnt_(false), damageActionCnt_(0),
    actionFlag_(false), graphFlag_(0), animationFlag_(false), damageFlag_(false),
    damage_(0), centerX_(0), centerY_(0)

{
    for (int i = 0; i < sizeof(abiFlag_); i++)
    {
        abiFlag_[i] = false;
    }
}

AbstractBattleCharacter::~AbstractBattleCharacter()
{
}

int AbstractBattleCharacter::getX() const
{
    return x_;
}

void AbstractBattleCharacter::setX(int x)
{
    x_ = x;
}

int AbstractBattleCharacter::getY() const
{
    return y_;
}

void AbstractBattleCharacter::setY(int y)
{
    y_ = y;
}

int AbstractBattleCharacter::getTp() const
{
    return tp_;
}

void AbstractBattleCharacter::setTp(int tp)
{
    tp_ = tp;
}

// ダメージと体力最大値よりTPを決定
void  AbstractBattleCharacter::calculateTp()
{
    if (damage_ * 100 / scs_->hp_ < 1)
    {
        tp_ += 1;
    }
    else
    {
        tp_ += damage_ * 100 / scs_->hp_;
    }
    if (tp_ > TpMax)
    {
        tp_ = TpMax;
    }
}

// TPをためる
void  AbstractBattleCharacter::chargeTp()
{
    tp_ += 8 + DxLib::GetRand(5);
    if (tp_ > TpMax) { tp_ = TpMax; }
}

int AbstractBattleCharacter::getAtbCnt() const
{
    return atbCnt_;
}

void AbstractBattleCharacter::setAtbCnt(int atbCnt)
{
    atbCnt_ = atbCnt;
}

void AbstractBattleCharacter::increaseAtbCnt()
{
    atbCnt_++;
    if (atbCnt_ == AtbCntMax)
    {
        atbCntFlag_ = false;
    }
}

bool AbstractBattleCharacter::getAtbCntFlag() const
{
    return atbCntFlag_;
}

void AbstractBattleCharacter::setAtbCntFlag(bool atbCntFlag)
{
    atbCntFlag_ = atbCntFlag;
}

bool AbstractBattleCharacter::getMotionFlag() const
{
    return motionFlag_;
}

void AbstractBattleCharacter::setMotionFlag(bool motionFlag)
{
    motionFlag_ = motionFlag;
}

//int   AbstractBattleCharacter::getToHp() const
//{
//    return toHp_;
//}

//void  AbstractBattleCharacter::setToHp(int toHp)
//{
//    toHp_ = toHp;
//}

//void  AbstractBattleCharacter::increaseToHp(int damage)
//{
//    toHp_ += damage;
//}


Status &AbstractBattleCharacter::getStatus()
{
    return status_;
}

void   AbstractBattleCharacter::setStatus(Status status)
{
    status_ = status;
}

Ability &AbstractBattleCharacter::getAbility(int i)
{
    return ability_[i];
}

void AbstractBattleCharacter::setAbility(Ability ability, int i)
{
    ability_[i] = ability;
}

int AbstractBattleCharacter::getMenuSelect() const
{
    return menuSelect_;
}

int AbstractBattleCharacter::getMenuSelectY() const
{
    return menuSelectY_;
}

void AbstractBattleCharacter::setMenuSelect(int menuSelect)
{
    menuSelect_ = menuSelect;
}
void AbstractBattleCharacter::setMenuSelectY(int menuSelectY)
{
    menuSelectY_ = menuSelectY;
}

void AbstractBattleCharacter::changeAbility()
{
    ability_[3].change();
    ability_[4].change();
}
void AbstractBattleCharacter::increaseHp(int hp)
{
    scs_->hp_ += hp;
}

void AbstractBattleCharacter::decreaseMp(int mp)
{
    scs_->mp_ -= mp;
}

void AbstractBattleCharacter::useMp()
{
    scs_->mp_ -= UseMp[(menuSelect_ - 20000) / 100];
    if (scs_->mp_ < 0)
    {
        scs_->mp_ = 0;
    }
}

void AbstractBattleCharacter::getOffenseParam(
    int *power, int *strength, int *weapon, int *level)
{
    *power = scs_->power_ + status_.power;
    *strength = scs_->strength_ + status_.strength;
    *weapon = scs_->weapon_ + status_.weapon;
    *level = scs_->level_;
}
void AbstractBattleCharacter::getDefenseParam(
    int *defense, int *equip)
{
    *defense = scs_->defense_ + status_.defense;
    *equip = scs_->equip_ + status_.equip;
}

int AbstractBattleCharacter::getActionCnt() const
{
    return actionCnt_;
}

void AbstractBattleCharacter::setActionCnt(int actionCnt)
{
    actionCnt_ = actionCnt;
}

void AbstractBattleCharacter::increaseActionCnt()
{
    actionCnt_++;
}

int AbstractBattleCharacter::getDamageActionCnt() const
{
    return damageActionCnt_;
}

void AbstractBattleCharacter::setDamageActionCnt(int damageActionCnt)
{
    damageActionCnt_ = damageActionCnt;
}

void AbstractBattleCharacter::increaseDamageActionCnt()
{
    damageActionCnt_++;
}

void AbstractBattleCharacter::increaseX()
{
    x_++;
}

void AbstractBattleCharacter::decreaseX()
{
    x_--;
}

bool AbstractBattleCharacter::getActionFlag() const
{
    return actionFlag_;
}

void AbstractBattleCharacter::setActionFlag(bool actionFlag)
{
    actionFlag_ = actionFlag;
}

bool AbstractBattleCharacter::getGraphFlag() const
{
    return graphFlag_;
}

void AbstractBattleCharacter::setGraphFlag(bool graphFlag)
{
    graphFlag_ = graphFlag;
}


bool AbstractBattleCharacter::getAnimationFlag() const
{
    return animationFlag_;
}

void AbstractBattleCharacter::setAnimationFlag(bool animationFlag)
{
    animationFlag_ = animationFlag;
}

bool AbstractBattleCharacter::getDamageFlag() const
{
    return damageFlag_;
}

void AbstractBattleCharacter::setDamageFlag(bool damageFlag)
{
    damageFlag_ = damageFlag;
}

int AbstractBattleCharacter::getDamage() const
{
    return damage_;
}

void AbstractBattleCharacter::setDamage(int damage)
{
    damage_ = damage;
}

int AbstractBattleCharacter::getCenterX() const
{
    return centerX_;
}
void AbstractBattleCharacter::setCenterX(int centerX)
{
    centerX_ = centerX;
}
int AbstractBattleCharacter::getCenterY() const
{
    return centerY_;
}
void AbstractBattleCharacter::setCenterY(int centerY)
{
    centerY_ = centerY;
}

::SharedCharacterStatus* AbstractBattleCharacter::getScs()
{
    return scs_;
}

}