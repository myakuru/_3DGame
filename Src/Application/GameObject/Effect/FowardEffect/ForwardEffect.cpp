#include "ForwardEffect.h"
#include"../../Character/Player/Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"

const uint32_t ForwardEffect::TypeID = KdGameObject::GenerateTypeID();

void ForwardEffect::Init()
{
	m_trailPolygon = std::make_shared<KdTrailPolygon>();
	m_trailPolygon2 = std::make_shared<KdTrailPolygon>();
	m_trailPolygon3 = std::make_shared<KdTrailPolygon>();

	m_trailPolygon->ClearPoints();
	m_trailPolygon2->ClearPoints();
	m_trailPolygon3->ClearPoints();

	m_trailPolygon->SetLength(100);
	m_trailPolygon2->SetLength(100);
	m_trailPolygon3->SetLength(100);

	m_trailTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/NA_ball of light_006.png");

	m_trailPolygon->SetMaterial(m_trailTex);
	m_trailPolygon2->SetMaterial(m_trailTex);
	m_trailPolygon3->SetMaterial(m_trailTex);


	m_trailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_trailPolygon2->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_trailPolygon3->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);

	m_once = false;
}


void ForwardEffect::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_player);
	auto player = m_player.lock();
	if (!player) return;

	// Tキーで軌跡開始
	if (!m_once)
	{
		m_trailProgress = 0.0f;
		m_trailPolygon->ClearPoints();
		m_trailPolygon2->ClearPoints();
		m_trailPolygon3->ClearPoints();

		// プレイヤーの前方ベクトルを取得
		Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(player->GetRotation()));
		forward.Normalize();

		// 開始位置と進行方向（前方×10）を保存
		m_trailStartPos = player->GetPos();
		m_trailDirection = forward * 10.0f;

		m_once = true;
	}

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 軌跡進行中
	if (m_trailProgress < 2.0f)
	{
		m_trailProgress += 0.05f;

		// プレイヤーの前方ベクトル
		Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(player->GetRotation()));
		forward.Normalize();

		// 斜め方向ベクトルを作成
		Math::Matrix rot60 = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(30.0f));
		Math::Matrix rot_60 = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(-30.0f));
		Math::Vector3 forward60 = Math::Vector3::TransformNormal(forward, rot60);
		Math::Vector3 forward_60 = Math::Vector3::TransformNormal(forward, rot_60);

		// 軌跡位置
		Math::Vector3 trailPos = m_trailStartPos + forward * 10.0f * m_trailProgress;
		Math::Vector3 trailPos2 = m_trailStartPos + forward60 * 10.0f * m_trailProgress;
		Math::Vector3 trailPos3 = m_trailStartPos + forward_60 * 10.0f * m_trailProgress;

		// ワールド行列
		Math::Matrix rotMat = Math::Matrix::CreateFromQuaternion(player->GetRotation());
		Math::Matrix scaleMat = Math::Matrix::CreateScale(0.4f);

		Math::Matrix trailMat = scaleMat* rotMat * Math::Matrix::CreateTranslation(trailPos + Math::Vector3{ 0,0.1f,0 });
		Math::Matrix trailMat2 = scaleMat * rotMat * Math::Matrix::CreateTranslation(trailPos2 + Math::Vector3{ 0,0.1f,0 });
		Math::Matrix trailMat3 = scaleMat * rotMat * Math::Matrix::CreateTranslation(trailPos3 + Math::Vector3{ 0,0.1f,0 });

		m_trailPolygon->AddPoint(trailMat);
		m_trailPolygon2->AddPoint(trailMat2);
		m_trailPolygon3->AddPoint(trailMat3);
	}
	
	
	if (m_trailProgress >= 2.0f)
	{

		m_trailPolygon->ClearPoints();
		m_trailPolygon2->ClearPoints();
		m_trailPolygon3->ClearPoints();
	}

	if (m_time >= 1.0f)
	{
		m_isExpired = true;
		m_time = 0.0f;
	}

}

void ForwardEffect::DrawBright()
{
	if (SceneManager::Instance().IsEffectActive())
	{
		KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
		KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon2, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
		KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon3, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
	}
}
