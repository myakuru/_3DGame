#include "inc_KdStandardShader.hlsli"
#include "../inc_KdCommon.hlsli"

Texture2D g_baseTex : register(t0);
SamplerState g_ss : register(s0);

float4 main(VSOutput In) : SV_Target0
{
    // ベースカラー取得
	float4 baseColor = g_baseTex.Sample(g_ss, In.UV) * g_BaseColor * In.Color;

	float3 gradationColor;
	
    // UV.yでグラデーション
	float gradientRate = In.UV.y;
	gradationColor = lerp(baseColor.rgb, g_GradientColor.rgb, gradientRate);

	return float4(gradationColor, baseColor.a);
}
