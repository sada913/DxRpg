#include "GameController.h"
#include "StdCommon.h"

GameController *GameController::getInstance()
{
    static GameController instance;    // 唯一のインスタンス
    return &instance;
}

// FPS計測関数
void GameController::calcFps()
{
    fps_[gCount_ % MetricTimes] = waitTime_;               // １周の時間を格納
    if ( (gCount_ % MetricTimes) == (MetricTimes - 1) )    // 計測回数に達したら
    {
        fpsAverage_ = 0;
        for ( int i = 0; i < MetricTimes; i++ )
        {
            fpsAverage_ += fps_[i];
        }
        fpsAverage_ /= MetricTimes;
    }
}

// FPS表示
void GameController::graphFps()
{
    if ( fpsAverage_ != 0 )    // 0割り禁止により、FPS平均が0じゃなかったらFPS表示
    {
        DxLib::DrawFormatString(0, 0, DxLib::GetColor(255, 255, 255),
            "FPS %.1f", 1000.0 / (double)(fpsAverage_) );
    }
}

// FPS制御
void GameController::controlFps()
{
    waitTime_ = DxLib::GetNowCount() - prevTime_;    //１周の処理にかかった時間を計算
    if ( prevTime_ == 0 )
    {                                            // t == 0 つまり一番最初に処理が行われたときは16を代入
        waitTime_ = OneFrameMSec;    
    }
    prevTime_ = DxLib::GetNowCount();            // 現在の時刻を格納
    if ( OneFrameMSec > waitTime_ )              // １周かかった時間がFPS60つまり１周16msよりも早く行われたとき
    {
        Sleep(OneFrameMSec - waitTime_);         // FPS60になるように、つまり１周16msまで待つ。
    }
}

char GameController::getKey(int input) const
{
    return key_[input];
}

int GameController::getAllKeyPressed()
{
    return DxLib::GetHitKeyStateAll( key_ );
}

void GameController::increaseGCount()
{
    gCount_++;                                
    if (gCount_ >= GCountMax)
    {
        gCount_ = 0;
    }
}

int GameController::getGCount() const
{
    return gCount_;
}

// マップ画面の移動以外で、移動速度が早くなりすぎるのを防ぐための機能
// メインループの最初に必ず書かなければならない
void GameController::adjustKeyState()
{
    for (int i = 0; i < KeyKindNum; i++)
    {
        // 前に押してなくて、今押していたら
        if (prevKey_[i] == NotPressed && key_[i] == Pressed)
        {
            key_[i] = PressedNow;     // 今の瞬間押されたという意味の2を代入。
        }
        prevKey_[i] = key_[i];        // 今の入力状態を過去に入力されたデータとしてコピー
    }
}

// 通常はメインループの中でこれだけを呼べばよい
void GameController::control()
{
    controlFps();        // FPS制御（FPSを60くらいに保つための関数）
#ifdef _DEBUG
    calcFps();           // 現在のFPSを計算
    graphFps();
#endif
    increaseGCount();    // 内部カウンタを1増加
}