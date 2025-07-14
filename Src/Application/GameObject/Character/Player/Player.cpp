#include "Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../Weapon/Katana/Katana.h"

void Player::Init()
{
	CharaBase::Init();
}

void Player::PreUpdate()
{
	// アニメーション適用後にノード行列を再計算
	m_modelWork->CalcNodeMatrices();

	// 手のワークノードを取得
	auto handWorkNode = m_modelWork->FindWorkNode("ring_01_r");

	if (!handWorkNode) return;

	//手のワークノードを取得して、カタナのワールド変換を設定
	{
		SceneManager::GetInstance().GetObjectWeakPtr(m_katana);

		if (m_katana.expired()) return;

		std::shared_ptr<Katana> katana = m_katana.lock();

		katana->SetKatanaMatrix(handWorkNode->m_worldTransform);
	}
}

void Player::Update()
{
	CharaBase::Update();
	m_animator->AdvanceTime(m_modelWork->WorkNodes(), 100.f);
}

void Player::DrawToon()
{
	CharaBase::DrawToon();
}

void Player::ImGuiInspector()
{
	CharaBase::ImGuiInspector();
}

void Player::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);
}

void Player::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
}
