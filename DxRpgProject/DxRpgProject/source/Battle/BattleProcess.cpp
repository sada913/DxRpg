#include "Battle/BattleProcess.h"
#include "StdCommon.h"
#include "ResourceLoader.h"
#include "GameController.h"
#include "Battle/AbstractBattleCharacter.h"
#include "Battle/BattleCharacter.h"
#include "Battle/BattleMonster.h"
#include "Battle/BattleAnimation.h"

namespace Battle
{

BattleProcess::BattleProcess(BattleCharacter *chr, BattleMonster *mon)
    :isWin_(false), isGameover_(false),
     isWinMoveNext_(false), isGameoverMoveNext_(false)
{
    gc_ = ::GameController::getInstance();
    rl_ = ::ResourceLoader::getInstance();

    chr_ = chr;
    mon_ = mon;
    ba_ = new BattleAnimation();

    initSakura();
}

BattleProcess::~BattleProcess()
{
    delete ba_;
}


void BattleProcess::initSakura()
{
    int r;
    double rand;
    for (int i = 0; i < SakuraMax; i++)
    {
        r = DxLib::GetRand(static_cast<int>(WindHeight * sqrt(2.0)));
        if (i < 60)
        {
            sakura_[i].x = DxLib::GetRand(WindWidth) + r;
            sakura_[i].y = 0.0 - r;
        }
        else
        {
            sakura_[i].x = WindWidth + r;
            sakura_[i].y = DxLib::GetRand(WindHeight) - r;
        }
        sakura_[i].cnt = 0;
        sakura_[i].flag = true;
        if (i < 30 || (60 <= i && i < 80))
        {
            rand = DxLib::GetRand(80) / 100.0 + 0.1;
        }
        else
        {
            rand = DxLib::GetRand(40) / 100.0 + 0.1;
        }
        sakura_[i].speed = rand * 2;
        sakura_[i].enlarge = rand;
        sakura_[i].rotation = DxLib::GetRand(60) / 100.0 + 0.3;
    }
}

void BattleProcess::main()
{
    calculateAtb();  // バトル用のカウンタ(=ATB)を上げる
 
    if (!isGameover_ && !isWin_)
    {
        // 味方
        if (!chr_->getMotionFlag() && chr_->getAtbCnt() == AtbCntMax)
        {
            selectMenu();
        }
        // 敵
        if (!mon_->getMotionFlag() && mon_->getAtbCnt() == AtbCntMax)
        {
            mon_->setMotionFlag(true);
        }
    }
    // 以下の処理は戦闘終了後(Win or Gameover)でも一部行う
    // 攻撃のモーション
    if (chr_->getMotionFlag())
    {
        doAction(chr_, mon_, Character);
    }
    if (mon_->getMotionFlag())
    {
        doAction(mon_, chr_, Enemy);
    }

    decideGameover();
    decideWin();

    graphBattle();
    displayMeter();

    ba_->graphAnimation(chr_, mon_);

    countupAbility();

    calcSakura();
    graphSakura();
    graphBoard();
    graphLine();
    // モーションしてなくてATBカウンタがMAXなら選択項目を描画
    if (!chr_->getMotionFlag() && chr_->getAtbCnt() == AtbCntMax)
    {
        graphIcon();
    }

    if (isGameover_)
    {
        // ゲームオーバー処理
        graphGameover();
    }
    else if (isWin_)
    {
        // 勝ったときの処理
        graphWin();
    }

}

bool BattleProcess::getWinMoveNext() const
{
    return isWinMoveNext_;
}

bool BattleProcess::getGameoverMoveNext() const
{
    return isGameoverMoveNext_;
}

void BattleProcess::calculateAtb()
{
    if (mon_->getAtbCntFlag() && !isGameover_ && !isWin_)
    {
        mon_->increaseAtbCnt();
    }
    if (chr_->getAtbCntFlag() && !isGameover_ && !isWin_)
    {
        chr_->increaseAtbCnt();
    }
}

void BattleProcess::selectMenu()
{
    if (gc_->getKey(KEY_INPUT_UP) == ::GameController::PressedNow)
    {
        chr_->pressedKeyUp();
        PlaySoundMem(rl_->getHdlSoundEffect(2), DX_PLAYTYPE_BACK);
    }
    else if (gc_->getKey(KEY_INPUT_DOWN) == ::GameController::PressedNow)
    {
        chr_->pressedKeyDown();
        PlaySoundMem(rl_->getHdlSoundEffect(2), DX_PLAYTYPE_BACK);
    }

    if (gc_->getKey(KEY_INPUT_X) == ::GameController::PressedNow)
    {
        chr_->pressedKeyX();
        if (chr_->getMenuSelect() < 10000)   // 選択がすべて完了していなければ
        {
            PlaySoundMem(rl_->getHdlSoundEffect(1), DX_PLAYTYPE_BACK);
        }
        else
        {
            chr_->resetSelectState();
            PlaySoundMem(rl_->getHdlSoundEffect(3), DX_PLAYTYPE_BACK);
            chr_->setMotionFlag(true);
        }
    }
    else if (gc_->getKey(KEY_INPUT_Z) == ::GameController::PressedNow)
    {
        chr_->pressedKeyZ();
        PlaySoundMem(rl_->getHdlSoundEffect(4), DX_PLAYTYPE_BACK);
    }
}

void BattleProcess::doAction(AbstractBattleCharacter *self,
    AbstractBattleCharacter *opponent,
    CharacterEnemyKind k)
{
    if (isGameover_ || isWin_)
    {
        self->setActionCnt(0);
        return;
    }
    // 前にずれるアクション
    if (20 < self->getActionCnt() && self->getActionCnt() <= 40)
    {
        //　味方
        if (k == Character)
        {
            self->decreaseX();
        }
        // 敵
        else
        {
            self->increaseX();
        }
    }
    // 攻撃モーションを開始
    if (self->getActionCnt() == 50)
    {
        // 味方
        if (k == Character)
        {
            ba_->startAnimation(self->getMenuSelect(), 0, 100);
        }
        // 敵
        else
        {
            // 10000は通常攻撃
            ba_->startAnimation(10000, 100, 0);
        }
    }

    // 今の瞬間ダメージアクションをするフラグが立っていたら START
    int damage = 0;
    if (self->getActionFlag())    // BattleAnimationクラスでONになる
    {
        // 敵の場合
        if (k == Enemy)
        {
            // 通常攻撃選択
            self->setMenuSelect(10000);
        }
        char message[MessageLenMax];
        switch (self->getMenuSelect())
        {
            case 10000:
            case 10100:
            case 20100:
            case 20200:
            case 20300:
            case 20400:
                damage = attackAction(self, opponent, k);
                ba_->setPopDamage(damage, opponent->getCenterX() - 20, opponent->getCenterY() - 20, 0);
                // 味方
                if (k == Character)
                {
                    sprintf(message, "敵に%dのダメージを与えた", damage);
                }
                // 敵
                else
                {
                    sprintf(message, "%dのダメージを受けた", damage);
                }             
                addBoard(message);
                break;

            case 20000:
                damage = cureAction(self);
                ba_->setPopDamage(damage, self->getCenterX() - 20, self->getCenterY() - 20, 1);
                // 味方
                if (k == Character)
                {
                    sprintf(message, "HPが%d回復した", damage);
                }
                // 敵
                else
                {
                    sprintf(message, "敵のHPが%d回復した", damage);
                }       
                addBoard(message);
                break;
            
            case 30000:
            case 30100:
            case 30200:
            case 30300:
            case 30400:
                // 味方のみ
                damage = abilityAction(self);
                // 30300のときのみ
                if (self->getMenuSelect() == 30300) {
                    ba_->setPopDamage(damage, self->getCenterX() - 20, self->getCenterY() - 20, 0);
                }
                break;

            default:
                break;
        }
        self->setActionFlag(false);
        self->increaseDamageActionCnt();
    }

    // 今の瞬間ダメージアクションをするフラグが立っていたら END

    // 後ろに下がる
    if (0 < self->getDamageActionCnt() && self->getDamageActionCnt() <= 20)
    {
        // 後ろに動く
        if (k == Character)
        {
            self->increaseX();
        }
        else
        {
            self->decreaseX();
        }
    }

    self->increaseActionCnt();    // ここは必ず毎回実行
    if (self->getDamageActionCnt() > 0)
    {
        self->increaseDamageActionCnt();
    }
    if (self->getDamageActionCnt() == 30)
    {
        // 各項目を初期化
        self->setActionCnt(0);
        self->setDamageActionCnt(0);

        self->setMenuSelect(1);
        self->setMenuSelectY(0);
        self->setMotionFlag(false);
        self->setAtbCntFlag(true);
        self->setAtbCnt(0);
    }

    // ゲームオーバーか勝利
    if (isGameover_ || isWin_)
    {
        self->setActionCnt(0);
        self->setDamageActionCnt(0);
    }
}

int BattleProcess::attackAction(
    AbstractBattleCharacter *self,
    AbstractBattleCharacter *opponent,
	CharacterEnemyKind k)
{
    int damage = decideDamage(self, ((k == Character)?0:1), self->getMenuSelect());
    if (self->getAbility(3).flag)
    {
        damage *= 2;
    }
    opponent->setDamage(damage);  // ダメージをセット
    // 体力が時間差で減っていく、最終的な終着位置
    //opponent->setToHp(opponent->getScs()->hp_ - opponent->getDamage());
    
    opponent->calculateTp();
    // 1回の攻撃で自分に8～13 TPがたまる
    self->chargeTp();

    // 魔法なら
    if (20000 <= self->getMenuSelect() && self->getMenuSelect() < 30000)
    {
        self->useMp();
    }

    self->changeAbility();
    return damage;
}


int BattleProcess::cureAction(AbstractBattleCharacter *c)
{
    int damage = decideDamage(c, 0, c->getMenuSelect());
    c->decreaseMp(9);
    c->increaseHp(damage);
	// 2015/07/27 DEL
	//c->increaseToHp(damage);
    
	// 回復しすぎたぶん戻す
    if (c->getScs()->hp_ > c->getScs()->hpMax_)
    {
        c->getScs()->hp_ = c->getScs()->hpMax_;
        //c->setToHp(c->getScs()->hp_);
    }
    if (c->getTp() > 300)
    {
        c->setTp(300);
    }
	c->increaseHp(0);   // 回復量を戻す

    c->changeAbility();
    return damage;
}

int BattleProcess::abilityAction(AbstractBattleCharacter *c)
{
    char message[MessageLenMax];
    c->setTp(c->getTp() + 8 + DxLib::GetRand(5));
    if (c->getTp() > 300)
    {
        c->setTp(300);
    }
    int damage = 0;

    switch (c->getMenuSelect())
    {
        case 30000:
            c->getAbility(0).flag = true;
            break;
        case 30100:
            c->getStatus().defense = c->getScs()->defense_ / 3;
            c->getStatus().power = - c->getScs()->power_ / 3;
            c->getAbility(1).flag = true;
            c->getAbility(1).cnt = 0;
            addBoard("防御力が上がった");
            addBoard("攻撃力が下がった");
            break;
        case 30200:
            c->getStatus().defense = -c->getScs()->defense_ / 3;
            c->getStatus().power = c->getScs()->power_ / 3;
            c->getAbility(2).flag = true;
            c->getAbility(2).cnt = 0;
            addBoard("攻撃力が上がった");
            addBoard("防御力が下がった");
            break;
        case 30300:
            damage = decideDamage(c, 2, c->getMenuSelect());
            //c->setToHp(damage);
            //if (c->getToHp() < 0)
            //{
            //    c->setToHp(0);
            //}
            c->getAbility(3).flag = true;
            c->getAbility(3).cnt = 3;
            addBoard("次回の攻撃時２倍のダメージ");
            sprintf(message, "武力の泉の反動で%dダメージを受けた", damage);
            addBoard(message);
            break;
        case 30400:
            c->getAbility(4).flag = true;
            c->getAbility(4).cnt = 1;
            addBoard("次回詠唱時間が４分の１");
            break;
        default:
            break;
    }
    return damage;
}

// to: 0:味方->敵、1:敵->味方
int  BattleProcess::decideDamage(AbstractBattleCharacter *c, int to, int actKind)
{
    int power = 0;
    int strength = 0;
    int weapon = 0;
    int level = 0;
    int defense = 0;
    int equip = 0;
    switch (to)
    {
        case 0:     // 味方の「通常攻撃」
            chr_->getOffenseParam(&power, &strength, &weapon, &level);
            mon_->getDefenseParam(&defense, &equip);
            if (actKind % 10000 == 2)      // 武力の泉の時(下1ケタが2)
            {
                power += 100 * (actKind % 10000) / 100;
            }
			if (actKind % 10000 == 100)   // 特殊攻撃
			{
				power *= 4;
			}
            break;
        case 1:     // 敵の「通常攻撃」
            mon_->getOffenseParam(&power, &strength, &weapon, &level);
            chr_->getDefenseParam(&defense, &equip);
            break;
        case 2:
            break;
        case 3:
            break;
    }
    if (actKind < 40000 && actKind != 20000)
    {
        return static_cast<int>(level * 2 + (power + weapon - (defense + equip))
            * strength * genRandom(0.01, 0.05) + strength * genRandom(0.2, 1.0));
    }
    if (actKind == 20000)    // 回復
    {
        return static_cast<int>(c->getScs()->magic_ * 10.0 * genRandom(0.2, 1.0));
    }
    return -1;
}


void BattleProcess::addBoard(char *message)
{
    int i = 0;
    for (i = 0; i < BoardMax - 1; i++)
    {
        if (!board_[i].flag)
        {
            break;
        }
    } 
    board_[i].flag = true;
    board_[i].cnt = 0;
    strcpy(board_[i].message, message);
}

void BattleProcess::decideGameover()
{
    if (chr_->getScs()->hp_ <= 0)
    {
        isGameover_ = true; // ゲームオーバー
    }
    else
    {
        return;
    }
    if (DxLib::CheckSoundMem(rl_->getHdlBattleSound(0) == 1))
    {
        StopSoundMem(rl_->getHdlBattleSound(0));
    }
}

void BattleProcess::decideWin()
{

    if (mon_->getScs()->hp_ <= 0)
    {
        isWin_ = true; // 勝利
    }
    else
    {
        return;
    }
    if (DxLib::CheckSoundMem(rl_->getHdlBattleSound(0)) == 1)
    {
        StopSoundMem(rl_->getHdlBattleSound(0));
    }

}

void BattleProcess::graphBattle()
{
    DxLib::DrawGraph(0, 0, rl_->getHdlImgBackGround(0), FALSE);
    DxLib::DrawGraph(0, WindHeight - 134, rl_->getHdlImgBackGround(10), FALSE);

    if (mon_->getGraphFlag())
    {
        DxLib::DrawGraph(
            mon_->getX(),
            mon_->getY(),
            rl_->getHdlImgMonster(mon_->getScs()->kind_),
            TRUE);
    }

    if (chr_->getGraphFlag())
    {
        DxLib::DrawGraph(
            chr_->getX(),
            chr_->getY(),
            rl_->getHdlImgChar(chr_->getScs()->kind_, 5),
            TRUE);
    }
}

void BattleProcess::displayMeter()
{
    // メータの開始位置の座標
    const int MonMtX = 15;
    const int MonMtY = 370;
    const int ChrMtX = 250;
    const int ChrMtY = 370;

    decreaseHpAction(mon_);
    graphBar(MonMtX, MonMtY,
        mon_->getScs()->hp_, mon_->getScs()->hpMax_,
        //mon_->getToHp(),
        mon_->getScs()->mp_, mon_->getScs()->mpMax_,
        mon_->getTp(), mon_->getAtbCnt(),
        mon_->getActionCnt(),
        mon_->getDamageActionCnt()
        );
    decreaseHpAction(chr_);
    graphBar(ChrMtX, ChrMtY,
        chr_->getScs()->hp_, chr_->getScs()->hpMax_,
        //chr_->getToHp(),
        chr_->getScs()->mp_, chr_->getScs()->mpMax_,
        chr_->getTp(), chr_->getAtbCnt(),
        chr_->getActionCnt(),
        chr_->getDamageActionCnt()
        );
}

void BattleProcess::countupAbility()
{
    stopAbi(chr_);
    stopAbi(mon_);
}

void BattleProcess::stopAbi(AbstractBattleCharacter *c)
{
    int time[AbilityMax] = {
        AbilityTime0,
        AbilityTime1,
        AbilityTime2,
        AbilityTime3,
        AbilityTime4
    };
    
    for (int i = 0; i < AbilityMax; i++)
    {
        if (c->getAbility(i).flag)
        {
            c->getAbility(i).cnt++;
            if (c->getAbility(i).cnt == time[i] * 60)
            {
                if (i == 1)
                {
                    c->getStatus().defense = 0;
                    c->getStatus().power = 0;
                    addBoard("ディフェンダーの効果が切れた");
                }
                else if (i == 2)
                {
                    c->getStatus().defense = 0;
                    c->getStatus().power = 0;
                    addBoard("バーサクの効果が切れた");
                }
                else if (i == 3)
                {
                    if (c->getAbility(i).flag)
                    {
                        addBoard("武力の泉の効果が切れた");
                    }

                }
                else if (i == 4)
                {
                    if (c->getAbility(i).flag)
                    {
                        addBoard("魔力の泉の効果が切れた");
                    }
                }
                c->getAbility(i).flag = false;
            }
        }
    }
}

// HP現象のモーション 一瞬で減るように改造
void BattleProcess::decreaseHpAction(AbstractBattleCharacter *c)
{
    /*int speed;*/
    if (isGameover_ && isWin_)
    {
        return;
    }
	/*
    if (c->getScs()->hp_ == c->getToHp())
    {
        return;
    }
    if (c->getScs()->hpMax_ < 300)
    {
        speed = 1;
    }
    else
    {
        speed = c->getScs()->hpMax_ / 300;
    }
    if (c->getToHp() < 0)
    {
        c->setToHp(0);
    }
    if (c->getScs()->hp_ - speed > c->getToHp())
    {
        c->getScs()->hp_ = c->getScs()->hp_ - speed;
    }
    if (c->getScs()->hp_ - speed <= c->getToHp())
    {
        c->getScs()->hp_ = c->getToHp();
    }
	*/
	if (c->getScs()->hp_ - c->getDamage() < 0) {
		c->getScs()->hp_ = 0;
	}
	else {
		c->getScs()->hp_ -= c->getDamage();
	}
	c->setDamage(0);
}

void BattleProcess::graphBar(int x, int y, int hp, int hpMax,
    /*int toHp,*/
	int mp, int mpMax, int tp, int atbCnt,
    int actCnt, int damActCnt)
{
    const int MeterHeight = 5;
    const int OffsetY = 25;

    int barColor;
    int tp_gage[2];

    tp_gage[0] = tp % 101;
    tp_gage[1] = tp % 101;
    if (tp > 100)
    {
        tp_gage[0] = 100;
        if (tp > 200)
        {
            tp_gage[1] = 100;
        }
    }
    // ATBメータの色
    if (atbCnt < 240)
    {
        // メーターがまだ満タンじゃなかったら 灰色
        barColor = DxLib::GetColor(50, 150, 150);  // 灰色
    }
    else
    {
        // レインボー
        barColor = GetColor(
            (gc_->getGCount() * 3) % 256,
            (gc_->getGCount() * 3 + 85) % 256,
            (gc_->getGCount() * 3 + 170) % 256);
    }

    // HPメータの枠
    DxLib::DrawBox(
        x,
        y,
        x + 150,
        y + MeterHeight,
        DxLib::GetColor(0, 255, 255), FALSE);
    // HPメータの中身
    DxLib::DrawBox(
        x,
        y,
        x + 150 * hp / hpMax,
        y + MeterHeight,
        DxLib::GetColor(0, 255, 255), TRUE);
    // HPメーターの中身
    DxLib::DrawBox(
        //x + 150 + toHp / hpMax,
        x + 150 + hp / hpMax,
		y,
        x + 150 * hp / hpMax,
        y + MeterHeight,
        DxLib::GetColor(200, 0, 0), TRUE);
    //decorateMeter(x, y, MeterHeight, 150 * toHp / hpMax);
	decorateMeter(x, y, MeterHeight, 150 * hp / hpMax);

    // MPメータの枠
    DxLib::DrawBox(
        x,
        y + OffsetY,
        x + 150,
        y + OffsetY + MeterHeight,
        DxLib::GetColor(200, 200, 0), FALSE);
    // MPメータの中身
    DxLib::DrawBox(
        x,
        y + OffsetY,
        x + 150 * mp / mpMax,
        y + OffsetY + MeterHeight,
        DxLib::GetColor(200, 0, 0), TRUE);
    decorateMeter(x, y + OffsetY, MeterHeight, 150 * mp / mpMax);

    // TPメーターの枠を描画
    DxLib::DrawBox(
        x,
        y + (OffsetY * 2),
        x + 150,
        y + (OffsetY * 2) + MeterHeight,
        DxLib::GetColor(100, 100, 100), FALSE);
    // TPメーターの中身を描画
    DxLib::DrawBox(
        x,
        y + (OffsetY * 2),
        x + 150 * tp_gage[0] / 100,
        y + (OffsetY * 2) + MeterHeight,
        DxLib::GetColor(100, 100, 100), TRUE);
    if (tp > 100)
    {
        // TPメーターの中身を描画
        DxLib::DrawBox(
            x,
            y + (OffsetY * 2),
            x + 150 * tp_gage[1] / 100,
            y + (OffsetY * 2) + MeterHeight,
            DxLib::GetColor(0, 255, 100), TRUE);
    }
    if (tp > 200)
    {
        // TPメーターの中身を描画
        DxLib::DrawBox(
            x,
            y + (OffsetY * 2),
            x + 150 * (tp - 200) / 100,
            y + (OffsetY * 2) + MeterHeight,
            DxLib::GetColor(255, 0, 0), TRUE);
    }
    // デコレーション
    if (tp <= 100)
    {
        decorateMeter(x, y + (OffsetY * 2), MeterHeight, 150 * tp / 100);
    }
    else if (tp <= 200)
    {
        decorateMeter(x, y + (OffsetY * 2), MeterHeight, 150 * (tp - 100) / 100);
    }
    else
    {
        decorateMeter(x, y + (OffsetY * 2), MeterHeight, 150 * (tp - 200) / 100);
    }

    // ATBメーター
    // ATBメーターの枠を描画
    DxLib::DrawBox(
        x,
        y + (OffsetY * 3),
        x + 150,
        y + (OffsetY * 3) + MeterHeight,
        barColor, FALSE);
    // ATBメーターの中身描画
    DxLib::DrawBox(
        x,
        y + (OffsetY * 3),
        x + 150 * atbCnt / 240,
        y + (OffsetY * 3) + MeterHeight,
        barColor, TRUE);

    DxLib::DrawFormatStringToHandle(x, y + 4, DxLib::GetColor(255, 255, 255),
        rl_->getHdlFont(0), "HP %4d/%4d", hp, hpMax);	// HP数値表示
    DxLib::DrawFormatStringToHandle(x, y + OffsetY + 4, DxLib::GetColor(255, 255, 255),
        rl_->getHdlFont(0), "MP %4d/%4d", mp, mpMax);	// MP数値表示
    DxLib::DrawFormatStringToHandle(x, y + (OffsetY * 2) + 4, DxLib::GetColor(255, 255, 255),
        rl_->getHdlFont(0), "TP %8d%%", tp);	// TP数値表示
    DxLib::DrawFormatStringToHandle(x, y + (OffsetY * 3) + 4, DxLib::GetColor(255, 255, 255),
        rl_->getHdlFont(0), "ATB %7d", atbCnt);	// ATB数値表示

#ifdef _DEBUG
    DxLib::DrawFormatStringToHandle(x, y - (OffsetY * 3) + 4, DxLib::GetColor(255, 255, 255),
        rl_->getHdlFont(0), "ACT    : %3d", actCnt);
    DxLib::DrawFormatStringToHandle(x, y - (OffsetY * 2) + 4, DxLib::GetColor(255, 255, 255),
        rl_->getHdlFont(0), "DAM ACT: %3d", damActCnt);
#endif

}

// 左から右に流れる白い線をスーっとメーターにデコレート
void BattleProcess::decorateMeter(int x, int y, int h, int len)
{
    const int MdLen = 50;
    // ゼロ割禁止
    if (len == 0)
    {
        return;
    }
    int x2 = x + gc_->getGCount() % len;
    for (int i = 0; i < MdLen; i++)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200 / MdLen * i);
        DrawLine(x2, y, x2, y + h, DxLib::GetColor(255, 255, 255));
        x2++;
        if (x2 >= x + len)
        {
            x2 = x;
        }
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void  BattleProcess::calcSakura()
{
    for (int i = 0; i < SakuraMax; i++)
    {
        if (sakura_[i].x < -24)
        {
            sakura_[i].x = WindWidth + 24;
        }
        if (sakura_[i].y > WindHeight + 24)
        {
            sakura_[i].y = -24;
        }
        sakura_[i].x -= sakura_[i].speed;
        sakura_[i].y += sakura_[i].speed;
    }
}

void  BattleProcess::graphSakura()
{
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
    for (int i = 0; i < SakuraMax; i++)
    {
        if (sakura_[i].flag)
        {
            DxLib::DrawRotaGraph(
                static_cast<int>(sakura_[i].x),
                static_cast<int>(sakura_[i].y),
                sakura_[i].enlarge,    // 拡大率
                PI * 2.0 / 180.0
                    * (static_cast<int>(sakura_[i].cnt
                        * sakura_[i].rotation) % 180),   // アングル ラジアン
                rl_->getHdlImgEtc(2),
                TRUE);
        }

        sakura_[i].cnt++;
    }
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}


void  BattleProcess::graphBoard()
{
    for (int i = 0; i < BoardMax; i++)
    {
        if (board_[i].flag)
        {
            if (board_[i].cnt == 120)
            {
                board_[i].flag = false;
                continue; 
            }
            // メッセージ部
            DxLib::DrawGraph(10, 26, rl_->getHdlImgEtc(1), TRUE);
            DxLib::DrawFormatString(14, 28,
                DxLib::GetColor(255, 255, 255), "%s", board_[i].message);
            board_[i].cnt++;
            break;
        }
    }
}

void  BattleProcess::graphLine()
{
    int x1 = mon_->getX() + 85;
    int y1 = mon_->getY();
    int x2 = chr_->getX() + 10;
    int y2 = chr_->getY();
    int y3 = 70;
    for (int x = x1; x <= x2; x++)
    {
        DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 72);
        if (x > x1 + line_.cnt)
        {
            break;
        }
        DxLib::DrawLine(x, calcLine(x1, y1, x2, y2, y3, x),
            x + 1, calcLine(x1, y1, x2, y2, y3, x + 1),
            DxLib::GetColor(0, 255, 255));
        DxLib::DrawLine(x, calcLine(x1, y1, x2, y2, y3, x) + 1,
            x + 1, calcLine(x1, y1, x2, y2, y3, x + 1) + 1,
            DxLib::GetColor(255, 255, 255));
        DxLib::DrawLine(x, calcLine(x1, y1, x2, y2, y3, x) + 2,
            x + 1, calcLine(x1, y1, x2, y2, y3, x + 1) + 2,
            DxLib::GetColor(0, 255, 255));
    }
    for (int i = x1 - 128 + (gc_->getGCount() * 5 % (x2 - x1 + 128)),
        j = 0;
        i <= x1 + (gc_->getGCount() * 5 % (x2 - x1 + 128));
        i++, j++)
    {
        if (i < x1)
        {
            continue;
        }
        if (i > x2)
        {
            break;
        }
        DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, j * 3);
        DxLib::DrawLine(i, calcLine(x1, y1, x2, y2, y3, i),
            i + 1, calcLine(x1, y1, x2, y2, y3, i + 1),
            DxLib::GetColor(0, 255, 255));
        DxLib::DrawLine(i, calcLine(x1, y1, x2, y2, y3, i) + 1,
            i + 1, calcLine(x1, y1, x2, y2, y3, i + 1) + 1,
            DxLib::GetColor(0, 255, 255));
        DxLib::DrawLine(i, calcLine(x1, y1, x2, y2, y3, i) + 2,
            i + 1, calcLine(x1, y1, x2, y2, y3, i + 1) + 2,
            DxLib::GetColor(0, 255, 255));
    }

    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    if (line_.cnt < 480)
    {
        line_.cnt += 4;
    }
}

int BattleProcess::calcLine(int ix1, int iy1, int ix2, int iy2, int iy3, int ix)
{
    double x1 = static_cast<int>(ix1);
    double y1 = static_cast<int>(iy1);
    double x2 = static_cast<int>(ix2);
    double y2 = static_cast<int>(iy2);
    double y3 = static_cast<int>(iy3);
    double x = static_cast<int>(ix);

    double y = ((sqrt(y1 - y3) + sqrt(y2 - y3)) 
        / (x2 - x1))*((sqrt(y1 - y3) + sqrt(y2 - y3))
        / (x2 - x1))*((x - x1) - sqrt(y1 - y3)*(x2 - x1)
        / (sqrt(y1 - y3) + sqrt(y2 - y3)))*((x - x1) - sqrt(y1 - y3)*(x2 - x1)
        / (sqrt(y1 - y3) + sqrt(y2 - y3))) + y3;
    return static_cast<int>(y);
}

/**
* @param  sigma
* @param  average
* @return 散らばり度合いとして正規分布した乱数を返す
*/
double BattleProcess::genRandom(double sigma, double average)
{
    static double r1, r2, s, r;
    static int valid = 0;
    double c;
    if (!valid)
    {
        r1 = DxLib::GetRand(RAND_MAX) / static_cast<double>(RAND_MAX + 1);
        r2 = DxLib::GetRand(RAND_MAX) / static_cast<double>(RAND_MAX + 1);
        s = sqrt(-2.0 * log(1.0 - r2));
    }
    valid = !valid;
    c = 2.0 * PI * r1;
    r = valid ? s * sin(c) : s + cos(c);
    return r * sigma + average;
}

void BattleProcess::graphGameover()
{
    static int cnt = 0;

    DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DxLib::DrawBox(0, 0, 640, 480, DxLib::GetColor(255, 0, 0), TRUE);
    DxLib::DrawString(300, 230, "GAME OVER", DxLib::GetColor(255, 255, 255));
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    cnt++;
    if (cnt == 120)
    {
        cnt = 0;
        isGameoverMoveNext_ = true;
    }
}

void BattleProcess::graphWin()
{
    static int cnt = 0;

    DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DxLib::DrawBox(0, 0, 640, 480, DxLib::GetColor(0, 255, 255), TRUE);
    DxLib::DrawString(300, 230, "VICTORY!!", DxLib::GetColor(255, 255, 255));
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    cnt++;
    if (cnt == 120)
    {
        cnt = 0;
        isWinMoveNext_ = true;
    }
}

// バトルのメニュー
void BattleProcess::graphIcon()
{
    const int OffsetX = 480;
    const int OffsetY = 370;

    // 選択枠
    DxLib::DrawGraph(WindWidth - 180, WindHeight - 134, rl_->getHdlImgBackGround(11), FALSE);
    // メインメニュー
    // 選択状態が最初の状態
    if (chr_->getMenuSelect() < 100)
    {
        int y;
        y = chr_->getMenuSelectY(); // 描画ｙ位置をｙにセット
        for (int i = 0; i < MenuNum; i++)
        {
            int x = 0;
            //現在選択されている項目は若干左にする
            if (i == y)
            {
                x = -5;
            }
            //DxLib::DrawGraph(OffsetX + x, OffsetY + 18 * i,
            //    rl_->getHdlImgMainIcon(i), TRUE); // 項目描画
            // 文字列表示
            DxLib::DrawFormatStringToHandle(OffsetX + x, OffsetY + 18 * i,
                DxLib::GetColor(255, 255, 255),
                rl_->getHdlFont(1), "%s", Main[i]);
        }
    }
    // サブメニュー
    for (int i = 0; i < SubMenuKind; i++)
    {
        // 戦う 魔法 アビリティは100ごとにわかれている
        if (chr_->getMenuSelect() >= 100 + 100 * i
            && chr_->getMenuSelect() < 200 + 100 * i)
        {
            int y;
            y = chr_->getMenuSelectY();
            for (int j = 0; j < MenuNum; j++)
            {
                int x = 0;
                if (j == y)
                {
                    x = -5;
                }
                // 文字列表示
                DxLib::DrawFormatStringToHandle(OffsetX + x, OffsetY + 18 * j,
                    DxLib::GetColor(255, 255, 255),
                    rl_->getHdlFont(1), "%s", Sub[i][j]);
            }
        }
    }
    // →カーソル
    DxLib::DrawGraph(OffsetX - 20 + (gc_->getGCount() % 20) / 4 - 5,
        OffsetY + 18 * chr_->getMenuSelectY() + 1,
        rl_->getHdlImgCursor(), TRUE);
    // メッセージ部
    DxLib::DrawGraph(10, 5, rl_->getHdlImgEtc(1), TRUE);
    // メッセージ書き込み
    char str[MessageLenMax] = { 0 };
    DxLib::DrawFormatString(14, 7, DxLib::GetColor(255, 255, 255),
        "%s", decideBoardString(str));
}

char *BattleProcess::decideBoardString(char *message)
{
    if (chr_->getAtbCnt() == 240
        && chr_->getMenuSelect() < 10000)
    {
        switch (chr_->getMenuSelect())
        {
            case 1:
                strcpy(message, "攻撃を行います。");
                break;
            case 2:
                strcpy(message, "魔法攻撃を行います。");
                break;
            case 3:
                strcpy(message, "アビリティを使用します。");
                break;
            case 4:
                strcpy(message, "アイテムを使用します。");
                break;
            case 5:
                strcpy(message, "戦闘から逃げます。");
                break;
            case 100:
                strcpy(message, "通常攻撃を行います。");
                break;
            case 101:
                strcpy(message, "TPを使用して特殊攻撃を行います。");
                break;
            case 200:
                strcpy(message, "MP9消費して回復魔法を行います。");
                break;
            case 201:
                strcpy(message, "MP8消費して風の魔法攻撃を行います。");
                break;
            case 202:
                strcpy(message, "MP16消費して火の魔法攻撃を行います。");
                break;
            case 203:
                strcpy(message, "MP24消費して氷の魔法攻撃を行います。");
                break;
            case 204:
                strcpy(message, "MP36消費して雷の魔法攻撃を行います。");
                break;
            case 300:
                strcpy(message, "挑発して相手を自分に引き付けます。(30秒後再使用可)");
                break;
            case 301:
                strcpy(message, "2分間攻撃力を下げて防御力を上げます。(2分後再使用可)");
                break;
            case 302:
                strcpy(message, "2分間攻撃力を上げて防御力を下げます。(2分後再使用可)");
                break;
            case 303:
                strcpy(message, "自分の体力を削って敵に2倍のダメージを与えます。(30秒後再使用可)");
                break;
            case 304:
                strcpy(message, "MPを2倍消費して詠唱時間を1/4にします。(30秒後再使用可)");
                break;
            case 400:
                strcpy(message, "HPが少し回復します。");
                break;
            case 401:
                strcpy(message, "HPが多く回復します。");
                break;
            case 402:
                strcpy(message, "攻撃力が増加します。");
                break;
            case 403:
                strcpy(message, "防御力が増加します。");
                break;
            case 404:
                strcpy(message, "MPが少し回復します。");
                break;
            case 500:
                strcpy(message, "戦闘から逃げます。");
                break;
            case 501:
                strcpy(message, "マネーを落として高確率で戦闘から逃げます。");
                break;
            default:
                strcpy(message, "");
                break;
        }
    }
    return message;
}


}