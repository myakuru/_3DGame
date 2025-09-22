#pragma once

// アニメーションキー(クォータニオン
struct KdAnimKeyQuaternion
{
	float				m_time = 0;		// 時間
	Math::Quaternion	m_quat;			// クォータニオンデータ
};

// アニメーションキー(ベクトル
struct KdAnimKeyVector3
{
	float				m_time = 0;		// 時間
	Math::Vector3		m_vec;			// 3Dベクトルデータ
};

//============================
// アニメーションデータ
//============================
struct KdAnimationData
{
	// アニメーション名
	std::string		m_name;
	// アニメの長さ
	float			m_maxLength = 0;

	// １ノードのアニメーションデータ
	struct Node
	{
		int			m_nodeOffset = -1;	// 対象モデルノードのOffset値

		// 各チャンネル
		std::vector<KdAnimKeyVector3>		m_translations;	// 位置キーリスト
		std::vector<KdAnimKeyQuaternion>	m_rotations;	// 回転キーリスト
		std::vector<KdAnimKeyVector3>		m_scales;		// 拡縮キーリスト

		void Interpolate(Math::Matrix& rDst, float time);
		bool InterpolateTranslations(Math::Vector3& result, float time);
		bool InterpolateRotations(Math::Quaternion& result, float time);
		bool InterpolateScales(Math::Vector3& result, float time);
	};

	// 全ノード用アニメーションデータ
	std::vector<Node>	m_nodes;
};

class KdAnimator
{
public:

	inline void SetAnimation(const std::shared_ptr<KdAnimationData>& rData, bool isLoop = true)
	{
		m_spAnimation = rData;
		m_isLoop = isLoop;
		m_time = 0.0f;
	}

	// アニメーションが終了してる？
	bool IsAnimationEnd() const
	{
		if (m_spAnimation == nullptr) { return true; }
		if (m_time >= m_spAnimation->m_maxLength) { return true; }

		return false;
	}

	float GetTime() const { return m_time; }
	void SetTime(float time) { m_blendTime = time; }

	// アニメーションの更新
	void AdvanceTime(std::vector<KdModelWork::Node>& rNodes, float speed = 1.0f);

	// アニメーションの補間再生1:再生アニメーション、2:補間時間、3:次のアニメーションがループするか、4:最大ループ回数(-1で無限ループ)
	void AnimationBlend(const std::shared_ptr<KdAnimationData>& nextAnim, float duration, bool nextIsLoop = true, int maxLoopCount = -1);

	bool GetRootMotion(
		const std::shared_ptr<KdAnimationData>& animData,
		const std::vector<KdModelData::Node>& modelNodes,
		const std::string& rootBoneName,
		float time,
		Math::Vector3& outTranslation
	) const;

private:

	std::shared_ptr<KdAnimationData>	m_spAnimation = nullptr;	// 再生するアニメーションデータ

	std::shared_ptr<KdAnimationData> m_spNextAnimation = nullptr;	// 次に再生するアニメーションデータ

	float m_blendTime = 0.0f;      // 補間経過時間
	float m_blendDuration = 0.0f;  // 補間全体時間
	bool m_isBlending = false;

	float m_time = 0.0f;

	bool m_isLoop = false;
	bool m_nextIsLoop = false;

	int m_loopCount = 0;         // 現在のループ回数
	int m_maxLoopCount = -1;     // 最大ループ回数（-1なら無限ループ）

	bool m_blendCaptured = false;

	// ブレンドポーズ
	std::vector<Math::Matrix> m_blendBasePoses;
	UINT idx;
};
