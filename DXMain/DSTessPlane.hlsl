
SamplerState gssWRAP_LINEAR : register(s0);
SamplerState gssWRAP_POINT : register(s1);
SamplerState gssCLAMP_LINEAR : register(s2);
SamplerState gssCLAMP_POINT : register(s3);

cbuffer SSReflectionVSConstants : register(b0){
	float4x4 Projection			: packoffset(c0);
	float4x4 View				: packoffset(c4);
}


//#define TEXTURE_SIZE 256
//cbuffer HightMapBuffer:register(b1) {
//	uint gHightValue[TEXTURE_SIZE * TEXTURE_SIZE];
//}



// 제어점을 출력합니다.
struct TERRAIN_HS_OUTPUT
{
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD;
};
struct DS_OUT {
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 texCoord : TEXCOORD0;
	float4 ViewPosition	: TEXCOORD1;
	float3 ViewNormal	: TEXCOORD2;
	float3 csPos		: TEXCOORD3;
};
// 패치 상수 데이터를 출력합니다.
struct TERRAIN_HS_CONSTANT
{
	float EdgeTessFactor[4]			: SV_TessFactor; // 예를 들어 쿼드 도메인에 대해 [4]입니다.
	float InsideTessFactor[2]		: SV_InsideTessFactor; // 예를 들어 쿼드 도메인에 대해 Inside[2]입니다.
														   // TODO: 다른 내용을 변경/추가합니다.
};

[domain("quad")]
DS_OUT main(TERRAIN_HS_CONSTANT input, float2 uv : SV_DomainLocation, OutputPatch<TERRAIN_HS_OUTPUT, 4> quad)
{
	DS_OUT output;

	output.positionW = lerp(lerp(quad[0].positionW, quad[1].positionW, uv.x), lerp(quad[2].positionW, quad[3].positionW, uv.x), uv.y);
	float3 normalW = lerp(lerp(quad[0].normalW, quad[1].normalW, uv.x), lerp(quad[2].normalW, quad[3].normalW, uv.x), uv.y);
	normalW = normalize(normalW);
	output.texCoord = lerp(lerp(quad[0].texCoord, quad[1].texCoord, uv.x), lerp(quad[2].texCoord, quad[3].texCoord, uv.x), uv.y);
	// Transform the position and normal to view space
	output.ViewPosition = mul(float4(output.positionW, 1.0f), View);
	output.ViewNormal = mul(normalW, (float3x3)View);

	//calc _view projection
	output.position = mul(output.ViewPosition, Projection);
	output.csPos = output.position.xyz / output.position.w;
	// Convert the projected position to clip-space

	return output;
}
