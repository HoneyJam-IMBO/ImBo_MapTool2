//SamplerComparisonState gssSHADOW : register(s4);
SamplerState gssSHADOW : register(s4);
Texture2D<float> ShadowMap : register(t5);
Texture2DArray<float> StaticShadowMapArray : register(t9);

#define MAX_SPACE_NUM 64

cbuffer CalcDirShadowBuffer : register(b4) {
	float bias : packoffset(c0.x);
	float bias_offset : packoffset(c0.y);
	float kernelhalf : packoffset(c0.z);
	float dist_sum : packoffset(c0.w);
	matrix gmtxLightVP : packoffset(c1);
};
cbuffer CalcStaticShadowBuffer : register(b9) {
	matrix gmtxStaticLightVP[MAX_SPACE_NUM] : packoffset(c0);
};
cbuffer SpaceInfo : register(b11) {
	float gOneSpaceSizeRcp;//하나의 공간의 크기
	float gOneSideSpaceNum;//한의 사이드에 있는 공간 수
	uint pad[2];
};

uint CalcShadowmapIndex(float3 positionW) {
	int indexX = positionW.x * gOneSpaceSizeRcp;
	int indexZ = positionW.z * gOneSpaceSizeRcp;

	uint index = indexX * gOneSideSpaceNum + indexZ;

	return index;
}

//shadow calc
float2 texOffset(int u, int v){
	return float2(u * 1.0f / 4096, v * 1.0f / 4096);
}
float ShaderPCF(float3 position, float cosTheta) {
	//uint shadowmap_index = CalcShadowmapIndex(position);
	//uint shadowmap_index = 0;
	float4 posShadowMap = mul(float4(position, 1.f), gmtxLightVP);
	float3 UVD = posShadowMap.xyz / posShadowMap.w;

	UVD.xy = 0.5 * UVD.xy + 0.5;
	UVD.y = 1 - UVD.y;

	float LightDepth = ShadowMap.Sample(gssSHADOW, UVD.xy).x;
	//float LightDepth = StaticShadowMapArray.Sample(gssSHADOW, float3(UVD.xy, shadowmap_index)).x;
	float fCamposDepth = (UVD.z);

	float value = bias;//d
	float offset = bias_offset;//d
	float bias = value*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
	bias = clamp(bias, value - offset, value + offset);

	
	float sum = 0.f;
	float fxlsize = 1 / 4096.f;
	float x, y;
	//bias = 0.04f;

	if (LightDepth < fCamposDepth - bias) {
		[unroll(9)]
		for (y = -kernelhalf; y <= kernelhalf; y += 1.f){
			[unroll(9)]
			for (x = -kernelhalf; x <= kernelhalf; x += 1.f){
				float2 pcfUV = UVD + float2(x * fxlsize, y * fxlsize);
				//LightDepth = StaticShadowMapArray.Sample(gssSHADOW, float3(pcfUV, shadowmap_index)).x;
				LightDepth = ShadowMap.Sample(gssSHADOW, pcfUV).x;

				if (LightDepth < fCamposDepth - bias){
					sum += dist_sum;
				}
				//n++;
				//if (n > 81) break;
			}
		}
	}
	return 1 - sum;
}
//shadow calc