#include "../inc_KdCommon.hlsli"
#include "inc_KdPostProcessShader.hlsli"

Texture2D g_inputTex : register(t0);
SamplerState g_ss : register(s0);

cbuffer cbNoise : register(b3)
{
	float g_noiseStrength;
	float g_time;
};

// 太い縦ノイズ帯を生成
float verticalBandNoise(float2 uv, float time)
{
    // 画面を32分割（太さ調整：値を変えると太く/細くなる）
	float band = floor(uv.y * 128.0);
    // 各帯ごとに同じノイズ値
	float noise = frac(sin(band + time * 5.0) * 43758.5453) * 2.0 - 1.0;
	return noise * g_noiseStrength * 0.02;
}

float4 main(VSOutput In) : SV_Target0
{
	float2 uv = In.UV;
	uv.x += verticalBandNoise(uv, g_time);

	float3 color = g_inputTex.Sample(g_ss, uv).rgb;

    // 追加でノイズを加算（好みに応じて）
	float n = verticalBandNoise(uv, g_time) * 10.0;
	color = saturate(color + n);

	return float4(color, 1);
}
