#pragma once
#include <array>
#include <chrono>

class KeyboardManager
{
public:
	static KeyboardManager& GetInstance()
	{
		static KeyboardManager instance;
		return instance;
	}

	// 毎フレーム呼び出し
	void Update(float deltaTime)
	{
		for (int i = 0; i < 256; ++i)
		{
			mOldKeyState[i] = mNowKeyState[i];
			mNowKeyState[i] = GetAsyncKeyState(i) & 0x8000;

			if (IsKeyJustPressed(i))
			{
				mKeyPressDuration[i] = 0.0f;
			}
			else if (IsKeyPressed(i))
			{
				mKeyPressDuration[i] += deltaTime;
			}
			// IsKeyJustReleasedのフレームではリセットしない
			else if (!mNowKeyState[i] && !mOldKeyState[i])
			{
				mKeyPressDuration[i] = 0.0f;
			}
		}
	}

	bool IsKeyPressed(int key) const
	{
		return mNowKeyState[key];
	}

	bool IsKeyJustPressed(int key) const
	{
		return mNowKeyState[key] && !mOldKeyState[key];
	}

	bool IsKeyJustReleased(int key) const
	{
		return !mNowKeyState[key] && mOldKeyState[key];
	}

	// 押した時間を取得
	float GetKeyPressDuration(int key) const
	{
		return mKeyPressDuration[key];
	}

private:
	KeyboardManager() : mNowKeyState{}, mOldKeyState{}, mKeyPressDuration{} {}

	std::array<SHORT, 256> mNowKeyState;
	std::array<SHORT, 256> mOldKeyState;
	std::array<float, 256> mKeyPressDuration;
};