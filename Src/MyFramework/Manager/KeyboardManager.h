#pragma once

class KeyboardManager
{
public:
    // シングルトンインスタンスを取得（他のクラスでインスタンス作成できなくする）
    static KeyboardManager& GetInstance()
    {
        static KeyboardManager instance;
        return instance;
    }

    // キーボードの状態を更新
    void Update()
    {
        for (int i = 0; i < 256; ++i)
        {
            mOldKeyState[i] = mNowKeyState[i];
            mNowKeyState[i] = GetAsyncKeyState(i) & 0x8000;
        }
    }

    // 特定のキーが押されているかチェック
    bool IsKeyPressed(int key) const
    {
        return mNowKeyState[key];
    }

    // 特定のキーが押された瞬間をチェック
    bool IsKeyJustPressed(int key) const
    {
        return mNowKeyState[key] && !mOldKeyState[key];
    }

    // 特定のキーが離された瞬間をチェック
    bool IsKeyJustReleased(int key) const
    {
        return !mNowKeyState[key] && mOldKeyState[key];
    }

private:
    // コンストラクタをプライベートにしてシングルトンにする
    KeyboardManager() : mNowKeyState{}, mOldKeyState{} {}

	// GetAsyncKeyStateがSHORT型の配列を使用するため、256キー分の状態を保持
    std::array<SHORT, 256> mNowKeyState;
    std::array<SHORT, 256> mOldKeyState;
};
