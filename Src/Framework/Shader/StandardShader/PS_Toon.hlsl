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

// 段階数（例: 3段階）
static const int ToonSteps = 3;
static const float3 ToonRamp[ToonSteps] =
{
	float3(0.2, 0.2, 0.3), // 影色
    float3(0.7, 0.7, 0.9), // 中間色
    float3(1.0, 1.0, 1.0) // 明色
};

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

    // 段階化（セルルック）
	int stepIdx = (int) floor(NdotL * ToonSteps);
	stepIdx = clamp(stepIdx, 0, ToonSteps - 1);

	float3 toonColor = baseColor.rgb * ToonRamp[stepIdx];

    // --- セルルックスペキュラー（ハイライト） ---
	float3 viewDir = normalize(g_CamPos - In.wPos);
	float3 halfDir = normalize(lightDir + viewDir);
	float NdotH = dot(wN, halfDir);
	NdotH = saturate(NdotH);

    // ハイライトを段階化
	float highlight = step(1.0, NdotH); // 0.95以上でハイライト
	toonColor += highlight * float3(1.0, 1.0, 0.8); // 黄色っぽいハイライト

    // 環境光加算
	toonColor += g_AmbientLight.rgb * baseColor.rgb * baseColor.a;

    // --- 輪郭線（エッジ）検出 ---
	float edge = dot(wN, viewDir);
	float edgeFactor = smoothstep(0.0, 0.2, abs(edge)); // 0.2未満でエッジ
	float3 edgeColor = float3(0, 0, 0);
	toonColor = lerp(edgeColor, toonColor, edgeFactor);

    // 出力
	return float4(toonColor, baseColor.a);
}
