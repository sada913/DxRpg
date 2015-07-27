#include "Map/AbstractMapCharacter.h"

namespace Map
{

AbstractMapCharacter::AbstractMapCharacter()
    : x_(0), y_(0), walkPixel_(0), walkFlag_(false), dir_(0),
    flag_(0), img_(0), imgFlag_(0), kind_(0),
    animePicPos_(0), walkSpeed_(DefWalkSpeed)
{
}

int AbstractMapCharacter::getX() const
{
    return x_;
}

int AbstractMapCharacter::getY() const
{
    return y_;
}

int AbstractMapCharacter::getImg() const
{
    return img_;
}

bool AbstractMapCharacter::getImgFlag() const
{
    return imgFlag_;
}

int AbstractMapCharacter::getDir() const
{
    return dir_;
}

int AbstractMapCharacter::getKind() const
{
    return kind_;
}

int AbstractMapCharacter::getAnimePicPos() const
{
    return animePicPos_;
}

void AbstractMapCharacter::stop()
{
    walkPixel_ = 0;
    walkFlag_ = false;
}
void AbstractMapCharacter::walk()
{
    walkPixel_++;
}

void AbstractMapCharacter::setImg(int img)
{
    img_ = img;
}
void AbstractMapCharacter::setImgFlag(bool imgFlag)
{
    imgFlag_ = imgFlag;
}

void AbstractMapCharacter::setPosition()
{
    if (walkFlag_)
    {
        // walkSpeed分だけ方向に対してキャラの座標を増減する。
        x_ = x_ + OffsetX[dir_] * walkSpeed_;
        y_ = y_ + OffsetY[dir_] * walkSpeed_;
    }
}

int AbstractMapCharacter::decideAnimePic(int walkPixel, int dir)
{
    int axisX;
    int axisY;
    // walkPixelは1ピクセル歩くと1増加
    // 1区画のどのあたりにいるのかというのを、
    // 1区画を4分割した値でわってもとまるのがその絵となる。
    // アニメーションのどれかを特定（画像のx方向[1次元配列]）
    // 0-31, /, 8 ８でわれば0-3に収まる
    axisX = (walkPixel % BlockLen) / 8;
    // アニメーションの向きを特定(画像のy方向[1次元配列])
    axisY = dir * 4;
    // 0～3は下向き画像、4～7は左向き画像、
    // 8～11は右向き画像、12～15は上向き画像･･･と数値を変換
    return axisX + axisY;
}

bool AbstractMapCharacter::canMove(int isPassable)
{
    // 主人公
    if (kind_ == MainCharIndex)
    {
        if (isPassable == NoThrough)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    // 町キャラ
    else
    {
        if (isPassable >= MainCharOnly)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

void AbstractMapCharacter::setX(const int x)
{
    x_ = x;
}
void AbstractMapCharacter::setY(const int y)
{
    y_ = y;
}

}