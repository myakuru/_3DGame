#include "inc_KdStandardShader.hlsli"
#include "../inc_KdCommon.hlsli"

// モデル描画用テクスチャ
Texture2D g_baseTex : register(t0);			// ベースカラーテクスチャ
Texture2D g_metalRoughTex : register(t1);	// メタリック/ラフネステクスチャ
Texture2D g_emissiveTex : register(t2);		// 発光テクスチャ
Texture2D g_normalTex : register(t3);		// 法線マップ

// 特殊処理用テクスチャ
Texture2D g_dirShadowMap : register(t10);	// 平行光シャドウマップ
Texture2D g_dissolveTex : register(t11);	// ディゾルブマップ
Texture2D g_environmentTex : register(t12); // 反射景マップ

// サンプラ
SamplerState g_ss : register(s0);				// 通常のテクスチャ描画用
SamplerComparisonState g_ssCmp : register(s1);	// 補間用比較機能付き

// ================================
// GGX ベースのPBRユーティリティ
// ================================
static const float PI = 3.1415926535;

float DistributionGGX(float3 N, float3 H, float a)
{
	// a = roughness^2（アルファ）
	float a2 = a * a;
	float NdotH = saturate(dot(N, H));
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return a2 / max(denom, 1e-4);
}

float GeometrySchlickGGX(float NdotV, float k)
{
	return NdotV / max(NdotV * (1.0 - k) + k, 1e-4);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	// k は直接光向けの近似
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Schlick-GGX

	float NdotV = saturate(dot(N, V));
	float NdotL = saturate(dot(N, L));
	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);
	return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// 既存：BlinnPhongは使用しないが残しておいても可
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
	float3 H = normalize(-lightDir + vCam);
	float NdotH = saturate(dot(normal, H)); // カメラの角度差(0～1)
	float spec = pow(NdotH, specPower);

	// 正規化Blinn-Phong
	return spec * ((specPower + 2) / (2 * 3.1415926535));
}

//================================
// ピクセルシェーダ
//================================
float4 main(VSOutput In) : SV_Target0
{
	// ディゾルブによる描画スキップ
	float discardValue = g_dissolveTex.Sample(g_ss, In.UV).r;
	if (discardValue < g_dissolveValue)
	{
		discard;
	}
	
	//------------------------------------------
	// 材質色
	//------------------------------------------
	float4 baseSample = g_baseTex.Sample(g_ss, In.UV);
	float4 baseColor = baseSample * g_BaseColor * In.Color;
	
	// Alphaテスト
	if( baseColor.a < 0.05f )
	{
		discard;
	}
	
	// カメラへの方向
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam); // カメラ - ピクセル距離
	vCam = normalize(vCam);

	// 法線マップから法線ベクトル取得
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;

	// UV座標（0～1）から 射影座標（-1～1）へ変換
	wN = wN * 2.0 - 1.0;
	
	{
		// 3種の法線から法線行列を作成
		row_major float3x3 mTBN =
		{
			normalize(In.wT),
			normalize(In.wB),
			normalize(In.wN),
		};
	
		// 法線ベクトルをこのピクセル空間へ変換
		wN = mul(wN, mTBN);
	}

	// 法線正規化
	wN = normalize(wN);

	// マテリアル（Metal-Rough）
	float4 mr = g_metalRoughTex.Sample(g_ss, In.UV);
	float metallic  = saturate(mr.b * g_Metallic);
	float roughness = saturate(mr.g * g_Roughness);
	roughness = max(roughness, 0.04);   // 破綻防止の下限
	float alpha = roughness * roughness;

	float3 albedo = saturate(baseColor.rgb);
	float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

	//------------------------------------------
	// ライティング
	//------------------------------------------
	float3 outColor = 0;
	
	//-------------------------------
	// シャドウマッピング(影判定)
	//-------------------------------
	float shadow = 1;

	// ピクセルの3D座標から、DepthMapFromLight空間へ変換
	float4 liPos = mul(float4(In.wPos, 1), g_DL_mLightVP);
	liPos.xyz /= liPos.w;

	// 深度マップの範囲内？
	if (abs(liPos.x) <= 1 && abs(liPos.y) <= 1 && liPos.z <= 1)
	{
		// 射影座標 -> UV座標へ変換
		float2 uv = liPos.xy * float2(1, -1) * 0.5 + 0.5;
		// ライトカメラからの距離
		float NdL = saturate(dot(normalize(In.wN), -normalize(g_DL_Dir)));
		float slopeBias = lerp(0.0005, 0.0035, 1 - NdL);
		float z = liPos.z - slopeBias;
		
		// 画像のサイズからテクセルサイズを求める
		float w, h;
		g_dirShadowMap.GetDimensions(w, h);
		float tw = 1.0 / w;
		float th = 1.0 / h;
	
		// uvの周辺3x3も判定し、平均値を求める
		shadow = 0;
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				shadow += g_dirShadowMap.SampleCmpLevelZero(g_ssCmp, uv + float2(x * tw, y * th), z);
			}
		}
		shadow *= 0.11;
	}
		
	//-------------------------
	// 平行光（GGX）
	//-------------------------
	{
		float3 N = wN;
		float3 V = vCam;
		float3 L = normalize(-g_DL_Dir); // ライト→ピクセル方向の逆（ピクセルからライト方向）
		float3 H = normalize(V + L);

		float NdotL = saturate(dot(N, L));
		float NdotV = saturate(dot(N, V));

		if (NdotL > 0 && NdotV > 0)
		{
			float  D  = DistributionGGX(N, H, alpha);
			float  G  = GeometrySmith(N, V, L, roughness);
			float3 F  = FresnelSchlick(saturate(dot(H, V)), F0);

			float3 kS = F;
			float3 kD = (1.0 - kS) * (1.0 - metallic);

			float3 diffuse  = kD * albedo / PI;
			float3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 1e-4);

			float3 direct = (diffuse + specular) * g_DL_Color * NdotL;
			outColor += direct * shadow * baseColor.a;
		}
	}

	// 全体の明度：環境光に1が設定されている場合は影響なし
	float totalBrightness = g_AmbientLight.a;

	//-------------------------
	// 点光（GGX）
	//-------------------------
	for( int i = 0; i < g_PointLightNum.x; i++ )
	{
		// ピクセルから点光への方向
		float3 L = g_PointLights[ i ].Pos - In.wPos;
		
		// 距離を算出
		float dist = length( L );
		L /= max(dist, 1e-4);

		// 点光の判定以内
		if( dist < g_PointLights[ i ].Radius )
		{
			// 半径をもとに、距離の比率を求める
			float atte = 1.0 - saturate( dist / g_PointLights[ i ].Radius );
			
			// 明度の追加
			totalBrightness += (1 - pow( 1 - atte, 2 )) * g_PointLights[ i ].IsBright;
			
			// 逆２乗の法則
			atte *= atte;

			float3 N = wN;
			float3 V = vCam;
			float3 H = normalize(V + L);
			float  NdotL = saturate(dot(N, L));
			float  NdotV = saturate(dot(N, V));

			if (NdotL > 0 && NdotV > 0)
			{
				float  D  = DistributionGGX(N, H, alpha);
				float  G  = GeometrySmith(N, V, L, roughness);
				float3 F  = FresnelSchlick(saturate(dot(H, V)), F0);

				float3 kS = F;
				float3 kD = (1.0 - kS) * (1.0 - metallic);

				float3 diffuse  = kD * albedo / PI;
				float3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 1e-4);

				float3 direct = (diffuse + specular) * g_PointLights[i].Color * NdotL;
				outColor += direct * atte * baseColor.a;
			}
		}
	}

	// 金属は拡散環境光が弱いので (1 - metallic) を適用して“白っぽさ”を軽減
	outColor += g_AmbientLight.rgb * baseColor.rgb * baseColor.a * (1.0 - metallic);
	
	// 自己発光色の適応
	if (g_OnlyEmissie)
	{
		outColor = g_emissiveTex.Sample(g_ss, In.UV).rgb * g_Emissive * In.Color.rgb;
	}
	else
	{
		outColor += g_emissiveTex.Sample(g_ss, In.UV).rgb * g_Emissive * In.Color.rgb;
	}
	
	//------------------------------------------
	// 高さフォグ
	//------------------------------------------
	if (g_HeightFogEnable && g_FogEnable)
	{
		if (In.wPos.y < g_HeightFogTopValue)
		{
			float distRate = length(In.wPos - g_CamPos);
			distRate = saturate(distRate / g_HeightFogDistance);
			distRate = pow(distRate, 2.0);
			
			float heightRange = g_HeightFogTopValue - g_HeightFogBottomValue;
			float heightRate = 1 - saturate((In.wPos.y - g_HeightFogBottomValue) / heightRange);
			
			float fogRate = heightRate * distRate;
			outColor = lerp(outColor, g_HeightFogColor, fogRate);
		}
	}
	
	//------------------------------------------
	// 距離フォグ
	//------------------------------------------
	if (g_DistanceFogEnable && g_FogEnable)
	{
		// フォグ 1(近い)～0(遠い)
		float f = saturate(1.0 / exp(camDist * g_DistanceFogDensity));
		
		// 適用
		outColor = lerp(g_DistanceFogColor, outColor, f);
	}
	
	// ディゾルブ輪郭発光
	if (g_dissolveValue > 0)
	{
		// 閾値とマスク値の差分で、縁を検出
		if (abs(discardValue - g_dissolveValue) < g_dissolveEdgeRange)
		{
			// 輪郭に発光色追加
			outColor += g_dissolveEmissive;
		}
	}
	
	totalBrightness = saturate(totalBrightness);
	// 過度な減衰を避ける（50%だけ反映）
	outColor *= lerp(1.0, totalBrightness, 0.5);
	
	//------------------------------------------
	// 出力
	//------------------------------------------
	return float4(outColor, baseColor.a);
}
