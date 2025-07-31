#include "inc_KdStandardShader.hlsli"
#include "../inc_KdCommon.hlsli"

// モデル描画用テクスチャ
Texture2D g_baseTex : register(t0);

// サンプラ
SamplerState g_ss : register(s0);

// ToonRamp
static const float3 ToonRamp = (0.8, 0.8, 0.8);

float4 main(VSOutput In) : SV_Target0
{
    // ベースカラー取得
	float4 baseColor = g_baseTex.Sample(g_ss, In.UV) * g_BaseColor * In.Color;

	float3 toonColor;

	toonColor = baseColor.rgb * ToonRamp;

    // 環境光加算
	toonColor += g_AmbientLight.rgb * baseColor.rgb * baseColor.a * 1.0;

    // 出力
	return float4(toonColor, baseColor.a);
}
