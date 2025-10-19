#pragma once
#include"../../../../../MyFramework/State/StateBase/StateBase.h"
#include"../PlayerConfig.h"
#include"../Player.h"
#include"../../Enemy/Enemy.h"
class BossEnemy;
class PlayerStateBase : public StateBase
{
public:
	PlayerStateBase() = default;
	~PlayerStateBase() override = default;

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	// 刀の位置を右手に追従するように更新
	virtual void UpdateKatanaPos();

	// 刀と鞘の位置が左手に追従するように更新
	virtual void UpdateUnsheathed();

	void SetPlayer(Player* player) { m_player = player; }

protected:
	Player* m_player = nullptr;

	Math::Vector3 prevRootTranslation = Math::Vector3::Zero;
	Math::Vector3 currentRootTranslation = Math::Vector3::Zero;
	Math::Vector3 m_attackDirection = Math::Vector3::Zero;

	bool m_isKeyPressing = false;

	float m_time = 0.0f;

	bool m_LButtonkeyInput = false;

	bool m_EButtonkeyInput = false;

	float m_animeTime = 0.0f;
	float m_maxAnimeTime = 0.0f;

	std::weak_ptr<BossEnemy> m_bossEnemy;

	Math::Vector3 m_cameraTargetOffset = { 0.0f,1.0f,-2.5f };

	Math::Vector3 m_cameraBossTargetOffset = { 0.0f,1.0f,-5.5f };

	std::shared_ptr<KdSoundInstance> m_runSound = nullptr;


};