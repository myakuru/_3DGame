#include "Katana.h"
#include"../../../main.h"
#include"../../../Scene/SceneManager.h"

// TypeIDの定義と初期化
const uint32_t Katana::TypeID = KdGameObject::GenerateTypeID();

void Katana::Init()
{
	WeaponBase::Init();
	m_trailPolygon = std::make_shared<KdTrailPolygon>();
	m_trailPolygon2 = std::make_shared<KdTrailPolygon>();
	m_trailPolygon3 = std::make_shared<KdTrailPolygon>();

	m_trailPolygon->ClearPoints();
	m_trailPolygon2->ClearPoints();
	m_trailPolygon3->ClearPoints();

	m_trailPolygon->SetLength(100);
	m_trailPolygon2->SetLength(100);
	m_trailPolygon3->SetLength(100);

	m_trailModel->Load("Asset/Models/Goast/nani.gltf");

	m_trailTex->Load("Asset/Textures/nailGlow.png");
	m_trailTex2->Load("Asset/Textures/NA_Basic hit_006.png");
	m_trailTex3->Load("Asset/Textures/nailGlow.png");

	m_trailPolygon->SetMaterial(m_trailTex);
	m_trailPolygon2->SetMaterial(m_trailTex);
	m_trailPolygon3->SetMaterial(m_trailTex3);

	m_trailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_trailPolygon2->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_trailPolygon3->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
}

void Katana::Update()
{
	// 剣の行列計算（元の処理そのまま）
	if (m_swordHandData.m_weaponTranslationMatrix != Math::Matrix::Identity)
	{
		UpdateHand();
	}
	else
	{
		m_swordData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.y),
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.x),
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.z)
		);

		Math::Vector3 playerHipPos = m_swordData.m_weaponTranslationMatrix.Translation();
		m_swordData.m_weaponTranslationMatrix.Translation(m_katanaOffset + playerHipPos);
		m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);
		m_swordData.m_weaponMatrix = m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponRotationMatrix * m_swordData.m_weaponTranslationMatrix * m_swordData.m_playerWorldMatrix;
		m_trailOffset = m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponRotationMatrix * m_swordData.m_playerWorldMatrix;
	}

	// 軌跡の先端位置を計算
	Math::Vector3 tip1, tip2, tip3;
	if (m_swordHandData.m_weaponTranslationMatrix != Math::Matrix::Identity)
	{
		tip1 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up() * 1.0f;
		tip2 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up() * 1.0f;
		tip3 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up() * 1.0f;
	}
	else
	{
		//tip1 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up();
		//tip2 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up();
		//tip3 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up();

		// 軌跡をクリア
		m_trailPolygon->ClearPoints();
		m_trailPolygon2->ClearPoints();
		m_trailPolygon3->ClearPoints();
	}

	Math::Matrix finalMat = Math::Matrix::CreateTranslation(tip1);
	Math::Matrix finalScale = Math::Matrix::CreateScale(0.1f, 0.1f, 0.1f);	

	m_trailPolygon->AddPoint(finalMat);

}

void Katana::DrawLit()
{
	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_trailModel);
}

void Katana::DrawUnLit()
{
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
}

void Katana::DrawBright()
{
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 0.0f,1.0f,1.0f,0.5f });
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon2, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon3, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
}

void Katana::UpdateHand()
{
	m_swordHandData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.z)
	);

	m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);

	m_swordHandData.m_weaponTranslationMatrix.Translation(m_katanaOffset + m_swordHandData.m_weaponTranslationMatrix.Translation());
	m_swordData.m_weaponMatrix = m_swordData.m_weaponScaleMatrix * m_swordHandData.m_weaponRotationMatrix *m_swordHandData.m_weaponTranslationMatrix * m_swordData.m_playerWorldMatrix; 

}

void Katana::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();

	ImGui::Text("Katana Inspector");

	ImGui::Text(U8("刀の角度を変更"));
	ImGui::DragFloat3("deg", &m_swordData.m_weaponDeg.x, 0.1f);
	ImGui::Text(U8("刀の位置を変更"));
	ImGui::DragFloat3("offset", &m_katanaOffset.x, 0.01f);
	ImGui::Text(U8("刀のスケールを変更"));
	ImGui::DragFloat3("scale", &m_swordData.m_scale.x, 0.01f);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Hand Katana"))
	{
		ImGui::Text(U8("手持ちの刀の角度を変更"));
		ImGui::DragFloat3("handDeg", &m_swordHandData.m_weaponDeg.x, 0.1f);
	}

	UpdateHand();
}
