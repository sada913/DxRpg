#pragma once
#include <climits>
namespace
{
    const int    MetricTimes = 60;            // FPS�v����
    const int    OneFrameMSec = 16;            // 1�t���[���̃~���b(16ms)
    const int    GCountMax = INT_MAX;       // �O���[�o���J�E���^�l�`�w
    const int    KeyKindNum = 256;
}
// ���[�e�B���e�B�N���X
// �V���O���g���N���X
class GameController
{

public:
    enum KeyState {
        NotPressed = 0,
        Pressed = 1,
        PressedNow = 2
    };
    static GameController* getInstance();

    void    calcFps();
    void    graphFps();
    void    controlFps();
    char    getKey(int input) const;
    int     getAllKeyPressed();
    void    increaseGCount();
    int     getGCount() const;

    void    adjustKeyState();

private:
    // �R���X�g���N�^�E�R�s�[�R���X�g���N�^�Ő�����R�s�[���֎~
    GameController() { }
    ~GameController() { }
    GameController(const GameController&) { }
    GameController& operator=(const GameController&) { }

    int     gCount_;
    int     fpsAverage_;                 // ���ςe�o�r
    int     waitTime_;                   // �e�o�r�Ŏg�p����ϐ�
    int     prevTime_;
    int     fps_[MetricTimes];
    char    key_[KeyKindNum];
    char    prevKey_[KeyKindNum];
};
