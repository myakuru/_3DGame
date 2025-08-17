#pragma once
#include "../CharacterBase.h"
#include"PlayerConfig.h"
class Katana;
class PlayerStateBase;
class Player :public CharaBase
{
public:

	// クラスごとに一意なTypeIDを持たせる
	static const uint32_t TypeID;
	Player() { m_typeID = TypeID; };
	~Player() override = default;

	void Init() override;
	void PreUpdate() override;
	void SkirtUpdate();
	void Update() override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	void StateInit();
	void ChangeState(std::shared_ptr<PlayerStateBase> _state);
	void UpdateMoveDirectionFromInput();

	const Math::Vector3& GetMoveDirection() const { return m_moveDirection; }
	Math::Vector3 GetLastMoveDirection() const { return m_lastMoveDirection; }
	void SetMoveDirection(const Math::Vector3& _moveDirection) { m_moveDirection = _moveDirection; }

	KdModelWork* GetModelWork() { return m_modelWork.get(); }

	const std::weak_ptr<Katana>& GetKatana() const { return m_katana; }

	PlayerConfig& GetPlayerConfig() { return m_playerConfig; }
	const Math::Vector3& GetPosition() const { return m_position; }

private:

	Math::Vector3 m_moveDirection = Math::Vector3::Zero; // 移動方向
	
	Math::Vector3 m_lastMoveDirection = Math::Vector3::Zero;

	std::weak_ptr<Katana>	m_katana;

	struct KdModelWork::Node* m_armatureNode = nullptr; // ヒップノード

	PlayerConfig m_playerConfig;

};