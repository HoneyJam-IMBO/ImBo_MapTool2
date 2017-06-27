cbuffer SSReflectionVSConstants : register(b0)
{
	float4x4 WorldViewProjection	: packoffset(c0);
	float4x4 WorldView				: packoffset(c4);
}

struct VS_OUTPUT
{
	float4 Position		: SV_Position;
	float4 ViewPosition	: TEXCOORD0;
	float3 ViewNormal	: TEXCOORD1;
	float3 csPos		: TEXCOORD2;
};


VS_OUTPUT main(float4 pos : POSITION, float3 norm : NORMAL)
{
	VS_OUTPUT Output;

	// Transform to projected space
	Output.Position = mul(pos, WorldViewProjection);

	// Transform the position and normal to view space
	Output.ViewPosition = mul(pos, WorldView);
	Output.ViewNormal = mul(norm, (float3x3)WorldView);

	// Convert the projected position to clip-space
	Output.csPos = Output.Position.xyz / Output.Position.w;

	return Output;
}
