#pragma once

class KeyboardManager
{
public:
    // �V���O���g���C���X�^���X���擾�i���̃N���X�ŃC���X�^���X�쐬�ł��Ȃ�����j
    static KeyboardManager& GetInstance()
    {
        static KeyboardManager instance;
        return instance;
    }

    // �L�[�{�[�h�̏�Ԃ��X�V
    void Update()
    {
        for (int i = 0; i < 256; ++i)
        {
            mOldKeyState[i] = mNowKeyState[i];
            mNowKeyState[i] = GetAsyncKeyState(i) & 0x8000;
        }
    }

    // ����̃L�[��������Ă��邩�`�F�b�N
    bool IsKeyPressed(int key) const
    {
        return mNowKeyState[key];
    }

    // ����̃L�[�������ꂽ�u�Ԃ��`�F�b�N
    bool IsKeyJustPressed(int key) const
    {
        return mNowKeyState[key] && !mOldKeyState[key];
    }

    // ����̃L�[�������ꂽ�u�Ԃ��`�F�b�N
    bool IsKeyJustReleased(int key) const
    {
        return !mNowKeyState[key] && mOldKeyState[key];
    }

private:
    // �R���X�g���N�^���v���C�x�[�g�ɂ��ăV���O���g���ɂ���
    KeyboardManager() : mNowKeyState{}, mOldKeyState{} {}

	// GetAsyncKeyState��SHORT�^�̔z����g�p���邽�߁A256�L�[���̏�Ԃ�ێ�
    std::array<SHORT, 256> mNowKeyState;
    std::array<SHORT, 256> mOldKeyState;
};
