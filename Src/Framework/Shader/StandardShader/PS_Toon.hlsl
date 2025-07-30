#include "inc_KdStandardShader.hlsli"
#include "../inc_KdCommon.hlsli"

// モデル描画用テクスチャ
Texture2D g_baseTex : register(t0);
Texture2D g_metalRoughTex : register(t1);
Texture2D g_emissiveTex : register(t2);
Texture2D g_normalTex : register(t3);

// 特殊処理用テクスチャ
Texture2D g_dirShadowMap : register(t10);
Texture2D g_dissolveTex : register(t11);
Texture2D g_environmentTex : register(t12);

// サンプラ
SamplerState g_ss : register(s0);
SamplerComparisonState g_ssCmp : register(s1);

// ToonRamp
static const float3 ToonRamp[2] =
{
	float3(0.8, 0.8, 0.8), // 影色
    float3(0.8, 0.8, 0.8) // 明色
};

// 影の閾値
static const float shadowThreshold = 0.05;

float4 main(VSOutput In) : SV_Target0
{
    // ベースカラー取得
	float4 baseColor = g_baseTex.Sample(g_ss, In.UV) * g_BaseColor * In.Color;

    // 法線取得
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;
	wN = wN * 2.0 - 1.0;
	row_major float3x3 mTBN = { normalize(In.wT), normalize(In.wB), normalize(In.wN) };
	wN = mul(wN, mTBN);
	wN = normalize(wN);

    // ライト方向
	float3 lightDir = -g_DL_Dir;

    // 拡散光（Lambert）
	float NdotL = dot(wN, lightDir);
	NdotL = saturate(NdotL);

    // 段階化（2段階でセルルック）
	float3 toonColor;
	if (NdotL < shadowThreshold)
		toonColor = baseColor.rgb * ToonRamp[0];
	else
		toonColor = baseColor.rgb * ToonRamp[1];

    // 環境光加算（控えめに）
	toonColor += g_AmbientLight.rgb * baseColor.rgb * baseColor.a * 1.0;

    // --- 輪郭線（エッジ）検出 ---
	float3 viewDir = normalize(g_CamPos - In.wPos);
	float edge = dot(wN, viewDir);
	float edgeWidth = 0.2;
	float edgeFactor = smoothstep(0.0, edgeWidth, abs(edge));
	float3 edgeColor = float3(0.05, 0.05, 0.08);
	toonColor = lerp(edgeColor, toonColor, edgeFactor);

    // 出力
	return float4(toonColor, baseColor.a);
}
