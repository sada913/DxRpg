#include "Battle/BattleAnimation.h"
#include "Battle/BattleCharacter.h"
#include "Battle/BattleMonster.h"
#include "ResourceLoader.h"
#include "StdCommon.h"

namespace Battle
{

BattleAnimation::BattleAnimation()
{
    rl_ = ResourceLoader::getInstance();
}


BattleAnimation::~BattleAnimation()
{
}

// 攻撃アニメーションの開始
void BattleAnimation::startAnimation(int selected, int target, int actor)
{
    // 使っていないアニメーションを探す
    int i = 0;
    for (i = 0; i < AnimationMax - 1; i++)   // 1個減らす
    {
        // フラグが立っていない
        if (!animation_[i].flag)
        {
            break;
        }
    }
    animation_[i].flag = true;
    animation_[i].target = target;   // 現状 100:味方,  0:敵
    animation_[i].kind = selected;   // menuSelect()の値
    animation_[i].cnt = 0;
    animation_[i].actor = actor;     // 現状 100:味方,  0:敵

}

void BattleAnimation::graphAnimation(BattleCharacter *chr, BattleMonster *mon)
{
    for (int i = 0; i < AnimationMax; i++)
    {
        if (animation_[i].flag)
        {
            int k = animation_[i].kind / 10000;
            switch (k) // アニメーションの種類によって処理を分岐
            {
                case 1:    // 攻撃
                    // 敵
                    if (animation_[i].target == 0)    // ターゲットが敵no.0だったら
                    {
                        animation0(i, mon, chr, mon->getX() + 5, mon->getY() + 5);
                    }
                    // 味方
                    if (animation_[i].target == 100)  // ターゲットが味方no.0なら
                    {
                        animation0(i, chr, mon, chr->getX() - 32, chr->getY() - 16);
                    }
                    break;
                case 2:    // 魔法
                    // 回復魔法
                    if (animation_[i].kind == 20000)
                    {
                        // 敵
                        if (animation_[i].actor == 0)
                        {
                            animation1(mon, i, 0, 0);
                        }
                        // 味方
                        else if (animation_[i].actor == 100)
                        {
                            animation1(chr, i, -30, 20);
                        }
                    }
                    // 攻撃魔法
                    else
                    {
                        // 敵
                        if (animation_[i].target == 0)    // ターゲットが敵no.0だったら
                        {
                            animation0(i, mon, chr, mon->getX() + 5, mon->getY() + 5);
                        }
                        // 味方
                        if (animation_[i].target == 100)  // ターゲットが見方no.0なら
                        {
                            animation0(i, chr, mon, chr->getX() - 32, chr->getY() - 16);
                        }
                    }
                    break;
                case 3:    // 特殊能力 Ability
                    // 敵
                    if (animation_[i].actor == 0)
                    {
                        animation2(mon, i, 0, 0);
                    }
                    // 味方
                    else if (animation_[i].actor == 100)
                    {
                        animation2(chr, i, -30, 20);
                    }
                    break;
                default:
                    exit(9999);
                    break;
            }
        }
    }
    graphDamage();   // Animationの後にDamage
}

void BattleAnimation::graphDamage()
{
    // 途中が飛んでいてもよい breakしない。
    for (int i = 0; i < PopDamMax; i++)
    {
        if (popDamage_[i].flag)
        {           
            calcDamage(i);
            
            popDamage_[i].cnt++;
            // たってから120(2秒)たったらフラグを落とす
            if (popDamage_[i].cnt == 120)
            {
                popDamage_[i].flag = false;
            }
        }
    }

}

void BattleAnimation::calcDamage(int t)
{
    // 位置調整 揺れて数字が出る様子を表現
    const int ArraySize = 20;
    int y[ArraySize] =
    {
        -1, -3, -4, -5, -4, -3, -1, 1, 3, 7,
        11, 15, 22, 16, 13, 12, 13, 16, 22, 21
    };
    char damage[6];
    sprintf(damage, "%d", popDamage_[t].damage);
    int cy;
    // ダメージの桁数だけループ
    for (int i = 0; i < static_cast<int>(strlen(damage)); i++)
    {
        if (popDamage_[t].cnt - 4 * i < 0)
        {
            return;
        }
        if (popDamage_[t].cnt - 4 * i < ArraySize)
        {
            cy = y[popDamage_[t].cnt - 4 * i];
        }
        else
        {
            cy = 22;
        }
        // ここも敵と味方で微妙に位置が違う変えるべき
        // ポップダメージ描画
        DxLib::DrawGraph(popDamage_[t].x + 15 * i,
            popDamage_[t].y + cy - popDamage_[t].type * 14,
            rl_->getHdlImgNumber(2 + popDamage_[t].type, damage[i] - '0'),
            TRUE);
    }
}

void  BattleAnimation::animation0(int idx, AbstractBattleCharacter *c1,
    AbstractBattleCharacter *c2, int x, int y)
{
    const int ComBlinkTime = 10;  // 調整

    int animationTime[5] = { -15, 40, 60, 80, 100 };
    int eishoTime[5]     = {  40, 40, 60, 80, 100 };
    int blinkTime[5]     = { -20, 40, 60, 80, 100 };

    // アビリティ4の時だけ 特殊処理
    if (c2->getAbility(4).flag)
    {
        for (int i = 1; i < 5; i++)
        {
            animationTime[i] /= 4;
            eishoTime[i] /= 4;
            blinkTime[i] /= 4;
        }
    }
    int kind = animation_[idx].kind % 10000 / 100;

    int t1 = animationTime[kind];
    int t2 = eishoTime[kind];
    int t3 = blinkTime[kind];
    animation_[idx].cnt++;

    bool cond_normal_attack = animation_[idx].kind == 10000;
    bool cond_magical_attack = 20000 <= animation_[idx].kind && animation_[idx].kind < 30000;


    if (animation_[idx].cnt == 5 && cond_normal_attack)
    {
        // シュッ
        DxLib::PlaySoundMem(rl_->getHdlSoundEffect(6), DX_PLAYTYPE_BACK);
    }
    // すべての種別の攻撃で判定
    if (animation_[idx].cnt == 5 + t2)
    {
        // それぞれの効果音 現状 100 ～ 304まで
        DxLib::PlaySoundMem(
            rl_->getHdlSoundEffect(animation_[idx].kind / 100), DX_PLAYTYPE_BACK);
#ifdef _DEBUG
        DxLib::printfDx("sound idx: %d\n", animation_[idx].kind / 100);
#endif
        // バックグラウンド再生 鳴らし始めるとすぐ次の処理に移る
    }
    // 詠唱ロジック
    if (animation_[idx].cnt == 5 && cond_magical_attack)
    {
        // 詠唱音
        DxLib::PlaySoundMem(rl_->getHdlSoundEffect(5), DX_PLAYTYPE_LOOP);
    }
    // 詠唱音が再生中の場合
    if (DxLib::CheckSoundMem(rl_->getHdlSoundEffect(5)) == SEPlaying
        && cond_magical_attack)
    {
        DxLib::DrawGraph(c2->getX() - 60 + DxLib::GetRand(2),
            c2->getY() - 35 + DxLib::GetRand(2),
            rl_->getHdlImgEtc(0), TRUE);
        DxLib::DrawBox(c2->getX() + 20,
            c2->getY() - 25,
            c2->getX() + 70,
            c2->getY() - 20,
            DxLib::GetColor(255, 255, 0), FALSE);
        DxLib::DrawBox(c2->getX() + 20,
            c2->getY() - 25,
            static_cast<int>(
            c2->getX() + 20
            + (static_cast<double>(animation_[idx].cnt - 5) / (t3 + 20))
            * 50.0
            ),
            c2->getY() - 20,
            DxLib::GetColor(255, 255, 0), TRUE);
    }
    if (animation_[idx].cnt == 25 + t2 && cond_magical_attack)
    {
        // 詠唱の場合 stop
        DxLib::StopSoundMem(rl_->getHdlSoundEffect(5));
    }

    // 点滅処理
    for (int i = 0; i < 4; i++)
    {
        if (animation_[idx].cnt == 70 + t3 + ComBlinkTime * i)
        {
            c1->setGraphFlag(false);
        }
        else
        {
            c1->setGraphFlag(true);
        }
    }
    // 動作アニメーション描画
    if (30 + t1 <= animation_[idx].cnt && animation_[idx].cnt < 70 + t1)
    {
        int kind2 = (animation_[idx].cnt - (30 + t1))
            / 5 + (animation_[idx].kind % 10000) / 100 * 8;
        DxLib::DrawGraph(x, y, rl_->getHdlImgAnimation(1, kind2), TRUE);
    }

    if (animation_[idx].cnt == 70 + t1)  // 調整 -> 直後に
    {
        animation_[idx].flag = false;
        c2->setActionFlag(true);    // 重要
    }
    // アビリティ4の時だけ
    if (c2->getAbility(4).flag)
    {
        for (int i = 1; i < 5; i++)
        {
            animationTime[i] *= 4;
            eishoTime[i] *= 4;
            blinkTime[i] *= 4;
        }
    }
}

// キュア
void  BattleAnimation::animation1(AbstractBattleCharacter *c, int idx, int x, int y)
{
    if (animation_[idx].cnt == 0)
    {
        DxLib::PlaySoundMem(rl_->getHdlSoundEffect(200), DX_PLAYTYPE_BACK);
    }
    if (animation_[idx].cnt < 50)
    {
        DxLib::DrawGraph(c->getX() + x,
            c->getY() + y - animation_[idx].cnt,
            rl_->getHdlImgAnimation(0, 10 + animation_[idx].cnt / 5), TRUE);
    }
    animation_[idx].cnt++;

    if (animation_[idx].cnt == 50)
    {
        animation_[idx].flag = false;
        c->setActionFlag(true);
    }
}

// アビリティ
void  BattleAnimation::animation2(AbstractBattleCharacter *c, int idx, int x, int y)
{
    if (animation_[idx].cnt == 0)
    {
        DxLib::PlaySoundMem(rl_->getHdlSoundEffect(c->getMenuSelect() / 100),
            DX_PLAYTYPE_BACK);
    }
    if (animation_[idx].cnt < 50)
    {
        DxLib::DrawGraph(c->getX() + x,
            c->getY() + y - animation_[idx].cnt,
            rl_->getHdlImgAnimation(0, 20 + animation_[idx].cnt / 10), TRUE);
    }
    animation_[idx].cnt++;

    if (animation_[idx].cnt == 50)
    {
        animation_[idx].flag = false;
        c->setActionFlag(true);
    }
}

void BattleAnimation::setPopDamage(int damage, int x, int y, int type)
{
    int i = 0;
    for (i = 0; i < PopDamMax - 1; i++)   // 1個減らす
    {
        if (!popDamage_[i].flag) { break; }
    }
    popDamage_[i].flag = true;
    popDamage_[i].cnt = 0;
    popDamage_[i].damage = damage;
    popDamage_[i].x = x;
    popDamage_[i].y = y;
    popDamage_[i].type = type;  // 1: cure, 0: others

    // ダメージアニメーション後にフラグがたってしまう。
    // つまりanimationFlagが落ちてからたつ模様。
#ifdef _DEBUG
    DxLib::printfDx("i: %d, damage: %d\n", i, popDamage_[i].damage);
#endif
}

}