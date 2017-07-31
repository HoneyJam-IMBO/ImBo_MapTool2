
cbuffer gWaterColor: register(b0)
{
	float4 WaterColor			: packoffset(c0);
}

float4 main() : SV_TARGET
{
	return WaterColor;
}