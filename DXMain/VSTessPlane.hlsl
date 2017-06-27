struct VS_INPUT {
	float3 position : POSITION;
	float4x4 mtxWorld : INSWORLDMTX;
};
struct VS_OUTPUT {
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD;
};

static const float2 arrUV[4] = {
	float2(0, 1),
	float2(1, 1),
	float2(0, 0),
	float2(1, 0)
};

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.positionW = mul(float4(input.position, 1.0f), input.mtxWorld).xyz;
	output.normalW = mul(float3(0, 1, 0), (float3x3)input.mtxWorld).xyz;
	output.texCoord = arrUV[vertexID];

	return output;
}