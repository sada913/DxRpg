#include "StdCommon.h"
#include "GameController.h"
#include "ResourceLoader.h"
#include "Sequence/Parent.h"

// ---------------------------------------------------------------
// エントリポイント
// ---------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); // メモリリーク検出
#endif
    // タイトルをセット
    DxLib::SetWindowTextA("DX RPG");
    // ウィンドウモードにセット＆ＤＸライブラリ開始処理
    if ( DxLib::ChangeWindowMode(TRUE) != DX_CHANGESCREEN_OK
        || DxLib::DxLib_Init() == -1
        || DxLib::SetWindowSizeExtendRate(1.0) == -1 )
    {
        return -1;
    }
    // 裏画面描画にする
    if ( DxLib::SetDrawScreen(DX_SCREEN_BACK) == -1 )
    {
        return -1;
    }
    // ユーティリティクラスのインスタンスを取得
    ResourceLoader   *rl = ResourceLoader::getInstance();
    GameController   *gc = GameController::getInstance();
    Sequence::Parent *parent = Sequence::Parent::getInstance();
    // インスタンス
    if ( rl->load() == -1 )            // 画像読込み
    {
        return -1;
    }
    
    // メッセージループ
    while( DxLib::ProcessMessage()    == 0
        && DxLib::ClearDrawScreen() == 0
        && gc->getAllKeyPressed()    == 0
        && gc->getKey(KEY_INPUT_ESCAPE)    == 0 )
    {
        gc->adjustKeyState(); // キーの入力スピード調整(Map以外)

        parent->update();

        gc->controlFps();        // FPS制御（FPSを60くらいに保つための関数）
#ifdef _DEBUG
        gc->calcFps();           // 現在のFPSを計算
#endif
        gc->increaseGCount();    // 内部カウンタを1増加
        gc->graphFps();
        DxLib::ScreenFlip();     // 裏画面を反映
    }
    // ＤＸライブラリ終了処理
    DxLib::DxLib_End();
    
    return 0;
}
