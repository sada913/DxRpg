#pragma once
#include "SharedCharacterStatus.h"  // 不完全クラス型へのポインターが参照できないそうなので

namespace Battle
{

const int  AbilityMax = 5;
const int  TpMax = 300;
const int  UseMp[] = { 9, 8, 16, 24, 32 };

struct Status
{
    int power;
    int defense;
    int strength;
    int magic;
    int weapon;
    int equip;
    Status()
        : power(0), defense(0), strength(0), magic(0),
        weapon(0), equip(0) { }
};

struct Ability
{
    int  flag;
    int  kind;
    int  cnt;
    Ability()
        : flag(0), kind(0), cnt(0) { }
    void change()
    {
        if (flag == 1)
        {
            flag = 2;
        }
    }
};

class AbstractBattleCharacter
{
public:
    AbstractBattleCharacter();
    virtual ~AbstractBattleCharacter();

    int     getX() const;
    void    setX(int x);
    int     getY() const;
    void    setY(int y);
    int     getTp() const;
    void    setTp(int tp);
    void    calculateTp();
    void    chargeTp();
    //int     getToHp() const;
    //void    setToHp(int toHp);
    //void    increaseToHp(int damage);

    Status  &getStatus();
    void    setStatus(Status status);
    Ability &getAbility(int i);
    void    setAbility(Ability ability, int i);

    int     getMenuSelect() const;
    int     getMenuSelectY() const;
    void    setMenuSelect(int menuSelect);
    void    setMenuSelectY(int menuSelectY);

    void    changeAbility();

    void    getOffenseParam(int *power, int *strength, int *weapon, int *level);
    void    getDefenseParam(int *defense, int *equip);

    int     getActionCnt() const;
    void    setActionCnt(int actionCnt);
    void    increaseActionCnt();
    int     getDamageActionCnt() const;
    void    setDamageActionCnt(int damageActionCnt);
    void    increaseDamageActionCnt();

    void    increaseX();
    void    decreaseX();

    bool    getActionFlag() const;
    void    setActionFlag(bool actionFlag);

    bool    getGraphFlag() const;
    void    setGraphFlag(bool graphFlag);

    int     getAtbCnt() const;
    void    setAtbCnt(int atbCnt);
    void    increaseAtbCnt();

    bool    getAtbCntFlag() const;
    void    setAtbCntFlag(bool atbCntFlag);

    bool    getMotionFlag() const;
    void    setMotionFlag(bool motionFlag);
    bool    getAnimationFlag() const;
    void    setAnimationFlag(bool animationFlag);
    bool    getDamageFlag() const;
    void    setDamageFlag(bool damageFlag);

    int     getDamage() const;
    void    setDamage(int damage);

    int     getCenterX() const;
    void    setCenterX(int centerX);
    int     getCenterY() const;
    void    setCenterY(int centerY);

    void    increaseHp(int hp);
    void    decreaseMp(int mp);
    void    useMp();

    ::SharedCharacterStatus *getScs();

protected:
    virtual void initializeParameter() = 0;
    int     x_;
    int     y_;

    int     tp_;
    double  cntSpeed_;
    bool    abiFlag_[60];
    int     atbCnt_;
    bool    atbCntFlag_;
    bool    motionFlag_;
    //int     toHp_;   // ダメージ等を受けたときに時間差で到達するHP
    int     menuSelect_;
    int     menuSelectY_;
    int     actionCnt_;
    int     damageActionCnt_;
    bool    actionFlag_;
    bool    graphFlag_;
    bool    animationFlag_;
    bool    damageFlag_;
    int     damage_;

    Status  status_;
    Ability ability_[AbilityMax];
    int     centerX_;
    int     centerY_;

    ::SharedCharacterStatus *scs_;
};

}