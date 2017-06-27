struct VS_INPUT {
	uint3 index : INSUINT_C_A;
};

cbuffer ViewProjection {
	float4x4 gmtxViewProjection : register(b0);
};

cbuffer GSVertexBuffer {
	float4 Vertex[1000] : register(b1);
};
struct GSOutput{
	float4 pos : SV_POSITION;
};

[maxvertexcount(3)]
void main(
	point VS_INPUT input[1],
	inout TriangleStream< GSOutput > output
)
{
	uint3 index = input[0].index;
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = Vertex[index.x];
		element.pos = mul(element.pos, gmtxViewProjection);
		output.Append(element);
		element.pos = Vertex[index.y];
		element.pos = mul(element.pos, gmtxViewProjection);
		output.Append(element);
		element.pos = Vertex[index.z];
		element.pos = mul(element.pos, gmtxViewProjection);
		output.Append(element);
	}
}