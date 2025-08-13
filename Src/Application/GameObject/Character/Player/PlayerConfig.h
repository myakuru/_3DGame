#pragma once

struct PlayerState_AttackParameter
{
	float m_dashTimer = 0.0f; // ダッシュのタイマー
};
struct PlayerState_Attack1Parameter
{
	float m_dashTimer = 0.0f; // ダッシュのタイマー
};

struct PlayerState_Attack2Parameter
{
	float m_dashTimer = 0.0f; // ダッシュのタイマー
};

class PlayerConfig
{
public:
	PlayerConfig() = default;
	~PlayerConfig() = default;


	void InGuiInspector()
	{
		{
			ImGui::Text(U8("アタックステート"));
			ImGui::DragFloat(U8("DashTime"), &m_playerState_AttackParam.m_dashTimer, 0.01f);

		}
		
		ImGui::Separator();
		{
			ImGui::Text(U8("アタック1ステート"));
			ImGui::DragFloat(U8("DashTime1"), &m_playerState_Attack1Param.m_dashTimer, 0.01f);
		}
	
		ImGui::Separator();
		{
			ImGui::Text(U8("アタック2ステート"));
			ImGui::DragFloat(U8("DashTime2"), &m_playerState_Attack2Param.m_dashTimer, 0.01f);
		}
	}

	void JsonInput(const nlohmann::json& _json)
	{
		if (_json.contains("DashTime"))
		{
			m_playerState_AttackParam.m_dashTimer = _json["DashTime"].get<float>();
			m_playerState_Attack1Param.m_dashTimer = _json["DashTime1"].get<float>();
			m_playerState_Attack2Param.m_dashTimer = _json["DashTime2"].get<float>();
		}
	}

	void JsonSave(nlohmann::json& _json) const
	{
		_json["DashTime"] = m_playerState_AttackParam.m_dashTimer;
		_json["DashTime1"] = m_playerState_Attack1Param.m_dashTimer;
		_json["DashTime2"] = m_playerState_Attack2Param.m_dashTimer;
	}

	PlayerState_AttackParameter& GetAttackParam() { return m_playerState_AttackParam; }
	PlayerState_Attack1Parameter& GetAttack1Param() { return m_playerState_Attack1Param; }
	PlayerState_Attack2Parameter& GetAttack2Param() { return m_playerState_Attack2Param; }

private:

	PlayerState_AttackParameter m_playerState_AttackParam; // 攻撃ステートのパラメータ
	PlayerState_Attack1Parameter m_playerState_Attack1Param; // 攻撃1ステートのパラメータ
	PlayerState_Attack2Parameter m_playerState_Attack2Param; // 攻撃2ステートのパラメータ

};