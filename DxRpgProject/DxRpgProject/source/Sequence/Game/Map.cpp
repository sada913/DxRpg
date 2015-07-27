#include "Sequence/Game/Map.h"
#include "Sequence/Game/Parent.h"
#include "Map/AbstractMapCharacter.h"
#include "ResourceLoader.h"
#include "Map/CsvMapReader.h"


//#define FIXED_      // 主人公の座標が固定されないモード

namespace Sequence
{
namespace Game
{

// マップ画面の初期化処理
Map::Map(Sequence::Game::Parent *parent, int stage)
    : hasChanged_(true), mapStage_(stage) , nextSequence_(NextMap)
{
    for (int i = 0; i < YBlock * XBlock; i++)
    {
        isPassable_[i] = Through;
    }

    parent_ = parent;
    cmr_ = new ::Map::CsvMapReader();
    rl_ = ::ResourceLoader::getInstance();
}

Map::~Map()
{
    delete cmr_;
}

void Map::update(Sequence::Game::Parent *parent)
{
    UNREFERENCED_PARAMETER(parent);
    // 音楽再生
    if (DxLib::CheckSoundMem(rl_->getHdlMapSound(0)) == 0)
    {
        DxLib::PlaySoundMem(rl_->getHdlMapSound(0), DX_PLAYTYPE_LOOP);
    }
    if (hasChanged_)
    {
        initMapState();
        initCharState();
    }
    else
    {
        // マップの描画
        drawMap();
        // キャラクタの描画
        drawCharacter();
    }
    // マップ位置の計算及び歩行状態の決定
    moveCharacter();
    // マップ変更判定
    changeMap();
}

// マップ画面の初期化処理
void Map::initMapState()
{
    cmr_->load(MapFile[mapStage_]);
    // マップの状態から通過フラグを作成
    for (int y = 0; y < YBlock; y++)            // 縦の15区間
    {
        for (int x = 0; x < XBlock; x++)        // 横の20区間
        {
            if (cmr_->getMapData(x, y) > (0 | 1))    // 通れない場所なら
            {
                isPassable_[y * XBlock + x] = NoThrough;    // 通れないフラグを立てる
            }
            // 通れる場所なら
            else
            {
                // 四隅 + その一つ内周なら(これが移動時に万が一重なるバグを回避する手段)
                // やりすぎ感があるがまあいいか
                if (y == 0 || y == 1 || y == YBlock - 2 || y == YBlock - 1
                    || x == 0 || x == 1 || x == XBlock - 2 || x == XBlock - 1)
                {   // 町キャラが画面遷移しないようにする
                    isPassable_[y * XBlock + x] = MainCharOnly;
                }
                else
                {
                    isPassable_[y * XBlock + x] = Through;
                }
            }
        }
    }
}

void Map::initCharState()
{
    int ty;     // 計算用Y座標
    int tx;     // 計算用X座標
    int dir;    // 向き
    int kind;   // キャラクタの種類
    // キャラクターを配置する。
    for (int i = 0; i < MainCharMax + CompCharMax;)
    {
        // 主人公
        if (i == MainCharIndex)
        {
            tx = parent_->getCharacter(MainCharIndex)->getX();
            ty = parent_->getCharacter(MainCharIndex)->getY();
            dir = parent_->getCharacter(MainCharIndex)->getDir();
            kind = Down;
        }
        // 町の人
        else
        {
            // 四隅とその２つ内周(-4からさらに-1していることに注意[GetRandの特性])を避ける
            int rx = DxLib::GetRand(XBlock - 5) + 2;
            int ry = DxLib::GetRand(YBlock - 5) + 2;
            // 町の人で乱数で決めた場所が通れない場所なら
            if (isPassable_[ry * XBlock + rx] >= MainCharOnly)
            {
                continue;    // やり直し
            }
            tx = rx * BlockLen;
            ty = ry * BlockLen;
            dir = DxLib::GetRand(3);        // 向きをランダムに決定 0-3
            kind = i % (CharKindMax - 1) + 1;        // 人の種類を決定 ※0は主人公  1-3
        }

        parent_->getCharacter(i)->initMapState(tx, ty, dir, kind);
        // 主人公か町の人がいるところは通れない場所とする
        isPassable_[ty / BlockLen * XBlock + tx / BlockLen] = NoThrough;
        i++;    // 位置決めが成功したときのみカウンタ増加
    }
    hasChanged_ = false;
}


// キャラの移動制御
void Map::moveCharacter()
{
    for (int i = 0; i < MainCharMax + CompCharMax; i++)
    {
        // 敵に遭遇
        if (parent_->getCharacter(i)->move(isPassable_))
        {
            // 敵に遭遇したら画面キャプチャ＆音楽停止＆モード遷移
            // このタイミングで呼ばないとキャプチャできない(ClearDrawScreenの前)
            rl_->captureImgMap();
            DxLib::StopSoundMem(rl_->getHdlMapSound(0));
            // 遷移先設定
            parent_->moveTo(Parent::NextBattle);
        }
        // 分割イメージをセット
        parent_->getCharacter(i)->setImg(
            rl_->getHdlImgChar(
                parent_->getCharacter(i)->getKind(),
                parent_->getCharacter(i)->getAnimePicPos()
                )
            );
    }
}

// マップ切り替え
void Map::changeMap()
{
    if (parent_->getCharacter(MainCharIndex)->getX() <= 0
        || parent_->getCharacter(MainCharIndex)->getY() <= 0
        || parent_->getCharacter(MainCharIndex)->getX() >= MapWidth - BlockLen
        || parent_->getCharacter(MainCharIndex)->getY() >= MapHeight - BlockLen)
    {
        // 右に出たら
        if (parent_->getCharacter(MainCharIndex)->getX() >= MapWidth - BlockLen)
        {
            // yそのままで一番左の一個右へ
            parent_->getCharacter(MainCharIndex)->setX(BlockLen);
            // マップ変更
            mapStage_++;
        }
        // 左に出たら
        if (parent_->getCharacter(MainCharIndex)->getX() <= 0)
        {
            // yそのままで一番の右一個左へ
            parent_->getCharacter(MainCharIndex)->setX(MapWidth - BlockLen * 2);
            // マップ変更
            mapStage_--;
        }
        // 必要があれば実装
        // 上、下
        // マップ変更
        hasChanged_ = true;
        parent_->setMapStage(mapStage_);
    }
}

// マップの描画
#ifdef FIXED_
void Map::drawMap()
{
    for (int y = 0; y < YBlock; y++)        // 縦の区間個数分ループ
    {
        for (int x = 0; x < XBlock; x++)    // 横の区間個数
        {
            // いったん芝生を描画
            DxLib::DrawGraph(
                x * BlockLen - BlockLen / 2,    // 端が半端なため半分ずらす
                y * BlockLen,
                rl_->getHdlImgMapchip(0), TRUE);
            // 本来、芝生でなく他の何かの描画であれば
            if (cmr_->getMapData(x, y) != 0)
            {                                // その画像を描画
                DxLib::DrawGraph(
                    x * BlockLen - BlockLen / 2,
                    y * BlockLen,
                    rl_->getHdlImgMapchip(cmr_->getMapData(x, y)), TRUE);
            }
        }
    }
}
#else
void Map::drawMap()
{
    for (int y = 0; y < YBlock; y++)        // 縦の区間個数分ループ
    {
        // 主人公を中心とする座標で画像の左端/上端開始点を求める。
        int ofsY = WindHeight / 2 - BlockLen / 2 - parent_->getCharacter(MainCharIndex)->getY();     // 主人公相対座標
        for (int x = 0; x < XBlock; x++)    // 横の区間個数
        {
            int ofsX = WindWidth / 2 - BlockLen / 2 - parent_->getCharacter(MainCharIndex)->getX();  // 主人公相対座標
            // いったん芝生を描画
            DxLib::DrawGraph(
                x * BlockLen + ofsX,    // 描画相対位置
                y * BlockLen + ofsY,                   // 描画相対位置
                rl_->getHdlImgMapchip(0), TRUE);
            // 本来、芝生でなく他の何かの描画であれば
            if (cmr_->getMapData(x, y) != 0)
            {                                // その画像を描画
                DxLib::DrawGraph(
                    x * BlockLen + ofsX,
                    y * BlockLen + ofsY,
                    rl_->getHdlImgMapchip(cmr_->getMapData(x, y)), TRUE);
            }
        }
    }
}
#endif

// キャラクタの描画
#ifdef FIXED_
void Map::drawCharacter()
{
    // キャラがスライドしている分、重なりを考慮して上から描画
    for (int y = 0; y < YBlock; y++)
    {
        for (int i = 0; i < MainCharMax + CompCharMax; i++)
        {
            int ty = parent_->getCharacter(i)->getY();
            // 調査しているところと同じ範囲の座標なら描画
            // こうしないと手前のキャラが前に描画されない（重なりがおかしくなる）
            // 画面奥から順に描画するこの方法をZソートという
            if (y == ty / BlockLen)
            {
                int tx = parent_->getCharacter(i)->getX();
                // x,yは左上頂点の座標で、
                // x:マップを1個分x座標に追加しているためのずらし
                DxLib::DrawGraph(
                    tx - BlockLen / 2,
                    // キャラが縦長の48pixcelのため、
                    // 32pixelからはみ出る16pixel分上にずらしている。
                    ty - SlidePicVal, parent_->getCharacter(i)->getImg(), TRUE);
            }
        }
    }
}
#else
void Map::drawCharacter()
{
    // キャラがスライドしている分、重なりを考慮して上から描画
    for (int y = 0; y < YBlock; y++)
    {
        for (int i = 0; i < MainCharMax + CompCharMax; i++)
        {
            int ty = parent_->getCharacter(i)->getY();
            // 調査しているところと同じ範囲の座標なら描画
            // こうしないと手前のキャラが前に描画されない（重なりがおかしくなる）
            // 画面奥から順に描画するこの方法をZソートという
            if (y == ty / BlockLen)
            {
                if (i == 0)
                {
                    // 主人公
                    // 調査しているところと同じ範囲の座標なら描画
                    // 真ん中からの相対位置
                    DxLib::DrawGraph(
                        WindWidth / 2 - BlockLen / 2,    // 中心
                        // キャラが縦長の48pixcelのため、
                        // 32pixelからはみ出る16pixel分上にずらしている。
                        WindHeight / 2 - BlockLen / 2 - SlidePicVal,  // 中心 + スライド分
                        parent_->getCharacter(MainCharIndex)->getImg(), TRUE);
                }
                else
                {
                    // 主人公以外
                    // 真ん中からの相対位置
                    int tx = parent_->getCharacter(i)->getX();
                    int ofsY = WindHeight / 2 - BlockLen / 2 - parent_->getCharacter(MainCharIndex)->getY();  // 主人公相対座標
                    int ofsX = WindWidth / 2 - BlockLen / 2 - parent_->getCharacter(MainCharIndex)->getX();  // 主人公相対座標
                    // x,yは左上頂点の座標で、
                    // x:マップを1個分x座標に追加しているためのずらし
                    // 主人公が動いた分もずらす
                    DxLib::DrawGraph(
                        tx + ofsX,
                        // キャラが縦長の48pixcelのため、
                        // 32pixelからはみ出る16pixel分上にずらしている。
                        ty + ofsY - SlidePicVal,     // ブロック分のずらしはキャラについて
                        parent_->getCharacter(i)->getImg(), TRUE);
                }
            }
        }
    }
}
#endif

}

}
