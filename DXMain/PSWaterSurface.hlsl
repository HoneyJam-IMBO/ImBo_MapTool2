

Texture2D           gtxtDepthTexture	: register(t0);
Texture2D           gtxtColor 			: register(t1);
Texture2D           gtxtNormal_Depth	: register(t2);
Texture2D           gtxtSpecInt_Pow	 	: register(t3);
Texture2D           gtxtReflection	 	: register(t4);//water ref

SamplerState        gssSamplerState				: register(s0);


cbuffer SSReflectionPSConstants : register(b0)
{
	float4x4 ViewProjMatrix			: packoffset(c0);
	float4x4 RefViewProjMatrix		: packoffset(c4);
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

	float4 cColor = gtxtReflection.Sample(gssSamplerState, ReflectionMapSamplingPos);
	return cColor;
}