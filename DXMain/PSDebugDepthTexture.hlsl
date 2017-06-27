#include "UnpackGBuffer.hlsli"

texture2D gtxtViewer : register(t0);
//Texture2DArray<float4> gtxtViewer : register(t0);

SamplerState gssWRAP_LINEAR : register(s0);
SamplerState gssWRAP_POINT : register(s1);
SamplerState gssCLAMP_LINEAR : register(s2);
SamplerState gssCLAMP_POINT : register(s3);

struct VS_TEXTURED_OUTPUT {
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

float4 main(VS_TEXTURED_OUTPUT input) : SV_Target{
	
	//float cColor = gtxtViewer.SampleLevel(gssCLAMP_POINT, input.texCoord, 0.0).w;
	float4 cColor = gtxtViewer.Sample(gssWRAP_LINEAR, float3(input.texCoord, 0));
	float depth = cColor.r;
	float linearDepth = ConvertDepthToLinear(depth) / 3000;
	return float4(linearDepth, linearDepth, linearDepth, 1.f);
}