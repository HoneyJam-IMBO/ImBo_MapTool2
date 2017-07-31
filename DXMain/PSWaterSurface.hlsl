

Texture2D           gtxtDepthTexture	: register(t0);
Texture2D           gtxtColor 			: register(t1);
Texture2D           gtxtNormal_Depth	: register(t2);
Texture2D           gtxtSpecInt_Pow	 	: register(t3);
Texture2D           gtxtReflection	 	: register(t4);//water ref
Texture2D           gtxtWaterBaseBump	 	: register(t5);//water base bump
Texture2D           gtxtWaterDetailBump	 	: register(t6);//water detail bump

SamplerState gssWRAP_LINEAR : register(s0);
SamplerState gssWRAP_POINT : register(s1);
SamplerState gssCLAMP_LINEAR : register(s2);
SamplerState gssCLAMP_POINT : register(s3);

cbuffer SSReflectionPSConstants : register(b0)
{
	float4x4 ViewProjMatrix			: packoffset(c0);
	float4x4 RefViewProjMatrix		: packoffset(c4);
	//프라뉄?
	///////////////////
	float3 eyeVector : packoffset(c8);
	uint fresnelMode : packoffset(c8.w);
	///////////////////
	float xDrawfactor : packoffset(c9.x);
	float fPowFactor : packoffset(c9.y);
	//wind
	float2 windDir : packoffset(c9.z);
	///////////////////
	///////////////////
	float windForce : packoffset(c10.x);
	float progress : packoffset(c10.y);
	float BumpMapBaseUVScale : packoffset(c10.z);
	float BumpMapDetailUVScale : packoffset(c10.w);
	/////////////////////
	//specular
	float specIntensity : packoffset(c11.x);
	float3 DirToLight: packoffset(c11.y);
	////////////////////
	float specExp : packoffset(c12.x);
	float3 DirLightColor : packoffset(c12.y);

	//wind2
	float2 windDir2 : packoffset(c13.x);
	///////////////////
	///////////////////
	float windForce2 : packoffset(c13.z);
	float xWaveHeight : packoffset(c13.w);
}
struct DS_OUT {
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normalW	: TEXCOORD1;
};

float4 main(DS_OUT input) : SV_TARGET
{
	//reflection uv
	float4 ReflectionUVD = mul(float4(input.positionW, 1.0f), RefViewProjMatrix);
	float2 ReflectionMapSamplingPos = ReflectionUVD.xy / ReflectionUVD.w;
	ReflectionMapSamplingPos.xy = 0.5 * ReflectionMapSamplingPos.xy + 0.5;
	ReflectionMapSamplingPos.y = 1 - ReflectionMapSamplingPos.y;

	//refraction uv
	float4 RefractionUVD = mul(float4(input.positionW, 1.0f), ViewProjMatrix);
	float2 RefractionMapSamplingPos = RefractionUVD.xy / RefractionUVD.w;
	RefractionMapSamplingPos.xy = 0.5 * RefractionMapSamplingPos.xy + 0.5;
	RefractionMapSamplingPos.y = 1 - RefractionMapSamplingPos.y;

	//범프
	//float3 vNormal = float3(0, 1, 0);
	float2 BaseBumpMapSamplingPos = progress * windForce * windDir;
	float2 BaseBumpMapUV = input.texCoord * BumpMapBaseUVScale + BaseBumpMapSamplingPos;
	float2 DetailBumpMapSamplingPos = progress * windForce2 * windDir2;
	float2 DetailBumpMapUV = input.texCoord * BumpMapDetailUVScale + DetailBumpMapSamplingPos;
	float3 vBaseNormal = gtxtWaterBaseBump.Sample(gssWRAP_LINEAR, BaseBumpMapUV).xyz; // 0 - 1
	float3 vDetailNormal = gtxtWaterDetailBump.Sample(gssWRAP_LINEAR, DetailBumpMapUV).xyz; // 0 - 1

	float p = 0.5f;
	float3 vNormal = vBaseNormal * (1 - p) + vDetailNormal * p;

	float2 perturbation = xWaveHeight * (vNormal.rg - 0.5f);
	ReflectionMapSamplingPos += perturbation;
	RefractionMapSamplingPos += perturbation;

	vNormal = vNormal * 2.f - 1.f;

	//vBaseNormal = vBaseNormal* 2.f - 1.f; //0-1, 0-2, -1~1
	//vDetailNormal = vDetailNormal * 2.f - 1.f;
	float3 T = float3(-1, 0, 0);
	float3 B = float3(0, 0, 1);
	float3 N = float3(0, 1, 0);

	float3x3 TBN = float3x3(T, B, N);
	vNormal = mul(vNormal, TBN);
	//vBaseNormal = mul(vBaseNormal, TBN);
	//vDetailNormal = mul(vDetailNormal, TBN);


	//return float4(1,0,0,1);
	//return float4(vNormal, 1.f);

	float3 vToEye = normalize(eyeVector - input.positionW);
	float fresnelTerm = 0.f;
	if (fresnelMode == 0) {
		fresnelTerm = dot(vToEye, vNormal);
		fresnelTerm = 1 - fresnelTerm * 1.3f;
	}
	else if (fresnelMode == 1) {
		fresnelTerm = 0.02f + 0.97f * pow((1 - dot(vToEye, vNormal)), fPowFactor);
	}
	else if (fresnelMode == 2) {
		float fAngle = 1 + dot(vToEye, vNormal);
		fAngle = pow(fAngle, fPowFactor);
		fresnelTerm = 1 / fAngle;
	}
	fresnelTerm = saturate(fresnelTerm * xDrawfactor);
	

	//반사
	float4 reflectiveColor = gtxtReflection.Sample(gssWRAP_LINEAR, ReflectionMapSamplingPos);
	//굴절
	float4 refractiveColor = gtxtColor.Sample(gssWRAP_LINEAR, RefractionMapSamplingPos);

	// 결합 된 색상을 생성
	float4 combinedColor = refractiveColor * (1 - fresnelTerm) + reflectiveColor * (fresnelTerm);
	float alpha = max(0.25, fresnelTerm);
	combinedColor = float4(combinedColor.xyz, alpha);

	//블린 스펙큘러
	float3 HalfWay = normalize(vToEye + DirToLight.xyz);
	float NDotH = saturate(dot(HalfWay, vNormal));
	combinedColor.xyz += DirLightColor * pow(NDotH, specExp) * specIntensity;

	return combinedColor;
}