#include "ResourceLoader.h"
#define THROWS_EX(ret, msg) { if (ret == -1) { throw msg; } }

const float  LoadNum = 32.0;  // dispProgress()を呼ぶ回数

ResourceLoader *ResourceLoader::getInstance()
{
    static ResourceLoader instance;  // 唯一のインスタンス
    return &instance;
}

void ResourceLoader::dispProgress()
{
    static int currentLoadNum = 0;
    DxLib::ProcessMessage();
    DxLib::ClearDrawScreen();
    currentLoadNum++;
    DxLib::DrawFormatString(WindWidth / 2 - 60, WindHeight / 2 + 160,
        GetColor(255, 255, 255), "Now Loading....");
    DxLib::DrawFormatString(WindWidth / 2 - 20, WindHeight / 2 + 200,
        GetColor(255, 255, 255), "%.0f％", currentLoadNum / LoadNum * 100);
    DxLib::ScreenFlip();
}

// データのロード
int ResourceLoader::load()
{
    
    int ret = 0;

    try {
        // マップチップ
        ret = DxLib::LoadDivGraph("img/mapchip/0/0.png",
            MapchipNum, 4, 1, 32, 32, hdlImgMapchip_);
        THROWS_EX(ret, "img/mapchip/0/0.png");
        dispProgress();

        // キャラクター画像
        ret = DxLib::LoadDivGraph("img/char/1.png",
            ImgCharDivNum, 4, 4, 32, 48, hdlImgChar_[0]);
        THROWS_EX(ret, "img/mapchip/0/1.png");
        dispProgress();

        ret = DxLib::LoadDivGraph("img/char/2.png",
            ImgCharDivNum, 4, 4, 32, 48, hdlImgChar_[1]);
        THROWS_EX(ret, "img/mapchip/0/2.png");
        dispProgress();

        ret = DxLib::LoadDivGraph("img/char/3.png",
            ImgCharDivNum, 4, 4, 32, 48, hdlImgChar_[2]);
        THROWS_EX(ret, "img/mapchip/0/3.png");
        dispProgress();
    
        ret = DxLib::LoadDivGraph("img/char/4.png",
            ImgCharDivNum, 4, 4, 32, 48, hdlImgChar_[3]);
        THROWS_EX(ret, "img/mapchip/0/4.png");
        dispProgress();
    
        // 背景の読込
        hdlImgBackGround_[0] = DxLib::LoadGraph("img/back/0.png");
        THROWS_EX(hdlImgBackGround_[0], "img/back/0.png");
        dispProgress();
        hdlImgBackGround_[10] = DxLib::LoadGraph("img/battle/10.png");
        THROWS_EX(hdlImgBackGround_[10], "img/battle/10.png");
        dispProgress();
        hdlImgBackGround_[11] = DxLib::LoadGraph("img/battle/11.png");
        THROWS_EX(hdlImgBackGround_[11], "img/battle/11.png");
        dispProgress();
    
        // モンスター画像の読込み
        hdlImgMonster_[0] = DxLib::LoadGraph("img/monster/0.png");
        THROWS_EX(hdlImgMonster_[0], "img/monster/0.png");
        dispProgress();
        //hdlImgMonster_[1] = DxLib::LoadGraph("img/monster/1.png");
        //dispProgress();

        // アイコン
        //ret = DxLib::LoadDivGraph("img/icon/0.png", ImgIconDivNum, 1, 5, 97, 17, hdlImgMainIcon_);
        //THROWS_EX(ret, "img/icon/0.png");
        //dispProgress();

        //ret = DxLib::LoadDivGraph("img/icon/1.png", ImgIconDivNum, 1, 5, 157, 17, hdlImgIcon_[0]);
        //THROWS_EX(ret, "img/icon/1.png");
        //dispProgress();

        //ret = DxLib::LoadDivGraph("img/icon/2.png", ImgIconDivNum, 1, 5, 157, 17, hdlImgIcon_[1]);
        //THROWS_EX(ret, "img/icon/2.png");
        //dispProgress();
        //
        //ret = DxLib::LoadDivGraph("img/icon/3.png", ImgIconDivNum, 1, 5, 157, 17, hdlImgIcon_[2]);
        //THROWS_EX(ret, "img/icon/3.png");
        //dispProgress();

        //ret = DxLib::LoadDivGraph("img/icon/4.png", ImgIconDivNum, 1, 5, 157, 17, hdlImgIcon_[3]);
        //THROWS_EX(ret, "img/icon/4.png");
        //dispProgress();

        //ret = DxLib::LoadDivGraph("img/icon/5.png", ImgIconDivNum, 1, 5, 157, 17, hdlImgIcon_[4]);
        //THROWS_EX(ret, "img/icon/5.png");
        //dispProgress();

        // 数字
        ret = DxLib::LoadDivGraph("img/num/0.png", ImgNumberDivNum, 10, 1, 16, 10, hdlImgNumber_[0]);
        THROWS_EX(ret, "img/num/0.png");
        dispProgress();

        //ret = DxLib::LoadDivGraph("img/num/1.png", 11, 11, 1, 15, 19, hdlImgNumber_[1]);
        //THROWS_EX(ret, "img/num/1.png");
        //dispProgress();

        ret = DxLib::LoadDivGraph("img/num/2.png", ImgNumberDivNum, 10, 1, 16, 18, hdlImgNumber_[2]);
        THROWS_EX(ret, "img/num/2.png");
        dispProgress();

        ret = DxLib::LoadDivGraph("img/num/3.png", ImgNumberDivNum, 10, 1, 16, 18, hdlImgNumber_[3]);
        THROWS_EX(ret, "img/num/3.png");
        dispProgress();

        // アニメーション画像
        ret = DxLib::LoadDivGraph("img/animation/0.png", 25, 5, 5, 96, 96, hdlImgAnimation_[0]);
        THROWS_EX(ret, "img/animation/0.png");
        dispProgress();

        ret = DxLib::LoadDivGraph("img/animation/1.png", 40, 8, 5, 96, 96, hdlImgAnimation_[1]);
        THROWS_EX(ret, "img/animation/1.png");
        dispProgress();

        // その他画像
        // 呪文詠唱フキダシ
        hdlImgEtc_[0] = DxLib::LoadGraph("img/animation/2.png");
        THROWS_EX(hdlImgEtc_[0], "img/animation/2.png");
        dispProgress();
        // メッセージボード
        hdlImgEtc_[1] = DxLib::LoadGraph("img/battle/0.png");
        THROWS_EX(hdlImgEtc_[1], "img/battle/0.png");
        dispProgress();
        // サクラ
        hdlImgEtc_[2] = DxLib::LoadGraph("img/battle/2.png");
        THROWS_EX(hdlImgEtc_[2], "img/battle/2.png");
        dispProgress();

        // カーソル画像
        hdlImgCursor_ = DxLib::LoadGraph("img/icon/100.png");
        THROWS_EX(hdlImgCursor_, "img/icon/100.png");
        dispProgress();

        // サウンドの読込み
        // マップ
        hdlMapSound_[0] = DxLib::LoadSoundMem("sound/map/0.ogg");
        THROWS_EX(hdlMapSound_[0], "sound/map/0.ogg");
        dispProgress();
        // バトル
        hdlBattleSound_[0] = DxLib::LoadSoundMem("sound/battle/0.ogg");
        THROWS_EX(hdlBattleSound_[0], "sound/battle/0.ogg");
        dispProgress();
        // SE
        hdlSoundEffect_[0] = DxLib::LoadSoundMem("sound/SE/0.ogg");
        THROWS_EX(hdlSoundEffect_[0], "sound/SE/0.ogg");
        dispProgress();
        hdlSoundEffect_[1] = LoadSoundMem("sound/SE/1.ogg");
        THROWS_EX(hdlSoundEffect_[1], "sound/SE/1.ogg");
        dispProgress();
        hdlSoundEffect_[2] = LoadSoundMem("sound/SE/2.ogg");
        THROWS_EX(hdlSoundEffect_[2], "sound/SE/2.ogg");
        dispProgress();
        hdlSoundEffect_[3] = LoadSoundMem("sound/SE/3.ogg");
        THROWS_EX(hdlSoundEffect_[3], "sound/SE/3.ogg");
        dispProgress();
        hdlSoundEffect_[4] = LoadSoundMem("sound/SE/4.ogg");
        THROWS_EX(hdlSoundEffect_[4], "sound/SE/4.ogg");
        dispProgress();
        hdlSoundEffect_[5] = LoadSoundMem("sound/SE/5.ogg");
        THROWS_EX(hdlSoundEffect_[5], "sound/SE/5.ogg");
        dispProgress();
        hdlSoundEffect_[6] = LoadSoundMem("sound/SE/6.ogg");
        THROWS_EX(hdlSoundEffect_[6], "sound/SE/6.ogg");
        dispProgress();
        hdlSoundEffect_[100] = LoadSoundMem("sound/SE/100.ogg");
        THROWS_EX(hdlSoundEffect_[100], "sound/SE/100.ogg");
        dispProgress();
		hdlSoundEffect_[101] = LoadSoundMem("sound/SE/101.ogg");
		THROWS_EX(hdlSoundEffect_[101], "sound/SE/101.ogg");
		dispProgress();
        hdlSoundEffect_[200] = LoadSoundMem("sound/SE/200.ogg");
        THROWS_EX(hdlSoundEffect_[200], "sound/SE/200.ogg");
        dispProgress();
        hdlSoundEffect_[201] = LoadSoundMem("sound/SE/201.ogg");
        THROWS_EX(hdlSoundEffect_[201], "sound/SE/201.ogg");
        dispProgress();
        hdlSoundEffect_[202] = LoadSoundMem("sound/SE/202.ogg");
        THROWS_EX(hdlSoundEffect_[202], "sound/SE/202.ogg");
        dispProgress();
        hdlSoundEffect_[203] = LoadSoundMem("sound/SE/203.ogg");
        THROWS_EX(hdlSoundEffect_[203], "sound/SE/203.ogg");
        dispProgress();
        hdlSoundEffect_[204] = LoadSoundMem("sound/SE/204.ogg");
        THROWS_EX(hdlSoundEffect_[204], "sound/SE/204.ogg");
        dispProgress();
        hdlSoundEffect_[301] = LoadSoundMem("sound/SE/301.ogg");
        THROWS_EX(hdlSoundEffect_[301], "sound/SE/301.ogg");
        dispProgress();
        hdlSoundEffect_[302] = LoadSoundMem("sound/SE/302.ogg");
        THROWS_EX(hdlSoundEffect_[302], "sound/SE/302.ogg");
        dispProgress();
        hdlSoundEffect_[303] = LoadSoundMem("sound/SE/303.ogg");
        THROWS_EX(hdlSoundEffect_[303], "sound/SE/303.ogg");
        dispProgress();
        hdlSoundEffect_[304] = LoadSoundMem("sound/SE/304.ogg");
        THROWS_EX(hdlSoundEffect_[304], "sound/SE/304.ogg");
        dispProgress();

        // 等幅フォントのConsolas
        hdlFont_[0] = CreateFontToHandle("Consolas", 18, 3, DX_FONTTYPE_ANTIALIASING_EDGE);  // 日本語不可
        hdlFont_[1] = CreateFontToHandle("メイリオ", 12, 2, DX_FONTTYPE_ANTIALIASING_EDGE);

    }
    catch ( const char* ex )
    {
        DxLib::printfDx("ERROR LoadFailed %s", ex);
    }

    return ret;
}

void ResourceLoader::captureImgMap()
{
    for (int i = 0; i < DivisionNum; i++)
    {
        for (int j = 0; j < DivisionNum; j++)
        {
            // 16分割画像格納データを作る。
            hdlImgStartBattle_[i * DivisionNum + j]
                = DxLib::MakeGraph(
                WindWidth / DivisionNum,
                WindHeight / DivisionNum
                );
            DxLib::GetDrawScreenGraph(
                0 + (WindWidth / DivisionNum) * j,
                0 + (WindHeight / DivisionNum) * i,
                0 + (WindWidth / DivisionNum) * (j + 1),
                0 + (WindHeight / DivisionNum) * (i + 1),
                hdlImgStartBattle_[i * DivisionNum + j]
                );
        }
    }
}

int  ResourceLoader::getHdlImgChar(int kind, int pos) const
{
    if ( kind >= 0 && kind <= 3 )
    {
        if ( pos >= 0 && pos <= 15 )
        {
            if ( hdlImgChar_[kind][pos] < 10 )
            {
                DxLib::printfDx( "INVALID HANDLE %d %d\n", kind, pos);
            }
            return hdlImgChar_[kind][pos];
        }
    }
    DxLib::printfDx("ERROR getHdlImgChar()\n");
    return 0;
}

int  ResourceLoader::getHdlImgMapchip(int kind) const
{
    return hdlImgMapchip_[kind];
}

int  ResourceLoader::getHdlImgBackGround(int kind) const
{
    return hdlImgBackGround_[kind];
}

int  ResourceLoader::getHdlImgMonster(int kind) const
{
    return hdlImgMonster_[kind];
}

int  ResourceLoader::getHdlMapSound(int kind) const
{
    return hdlMapSound_[kind];
}

int  ResourceLoader::getHdlBattleSound(int kind) const
{
    return hdlBattleSound_[kind];
}

int  ResourceLoader::getHdlSoundEffect(int kind) const
{
    return hdlSoundEffect_[kind];
}

int  ResourceLoader::getHdlImgStartBattle(int kind) const
{
    return hdlImgStartBattle_[kind];
}

void ResourceLoader::setImageStartBattle(int idx, int width, int height)
{
    hdlImgStartBattle_[idx] = DxLib::MakeGraph(width, height);
}

int ResourceLoader::getHdlImgMainIcon(int number) const
{
    return hdlImgMainIcon_[number];
}

int ResourceLoader::getHdlImgIcon(int kind, int pos) const
{
    return hdlImgIcon_[kind][pos];
}

int ResourceLoader::getHdlImgNumber(int kind, int pos) const
{
    return hdlImgNumber_[kind][pos];
}

int ResourceLoader::getHdlImgAnimation(int kind, int pos) const
{
    return hdlImgAnimation_[kind][pos];
}

int  ResourceLoader::getHdlImgEtc(int kind) const
{
    return hdlImgEtc_[kind];
}

int  ResourceLoader::getHdlImgCursor() const
{
    return hdlImgCursor_;
}

int  ResourceLoader::getHdlFont(int kind) const
{
    return hdlFont_[kind];
}

int ResourceLoader::getHdlNullImg() const
{
    return 0;
}