#include "WeaponKatanaScabbard.h"

const uint32_t WeaponKatanaScabbard::TypeID = KdGameObject::GenerateTypeID();

void WeaponKatanaScabbard::Update()
{
	WeaponBase::Update();
	UpdateMatrix();
}

void WeaponKatanaScabbard::UpdateMatrix()
{
	m_swordHandData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.z)
	);

	m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);

	m_swordHandData.m_weaponTranslationMatrix.Translation(m_swordHandData.m_weaponTranslationMatrix.Translation());
	Math::Matrix transOffsetMat = Math::Matrix::CreateTranslation(m_katanaOffset);
	m_swordData.m_weaponMatrix = transOffsetMat * m_swordData.m_weaponScaleMatrix * m_swordHandData.m_weaponRotationMatrix * m_swordHandData.m_weaponTranslationMatrix * m_swordData.m_playerWorldMatrix;
}

void WeaponKatanaScabbard::ImGuiInspector()
{
	WeaponBase::ImGuiInspector();

	ImGui::Text("Katana Inspector");

	ImGui::Text(U8("刀の角度を変更"));
	ImGui::DragFloat3("deg", &m_swordData.m_weaponDeg.x, 0.1f);
	ImGui::Text(U8("刀の位置を変更"));
	ImGui::DragFloat3("offset", &m_katanaOffset.x, 0.1f);
	ImGui::Text(U8("刀のスケールを変更"));
	ImGui::DragFloat3("scale", &m_swordData.m_scale.x, 0.01f);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Hand Katana"))
	{
		ImGui::Text(U8("手持ちの刀の角度を変更"));
		ImGui::DragFloat3("handDeg", &m_swordHandData.m_weaponDeg.x, 0.1f);
	}

	UpdateMatrix();
}
