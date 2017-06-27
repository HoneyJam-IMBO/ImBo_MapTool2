struct VS_INPUT {
	uint3 index : INSUINT_C_A;
};

VS_INPUT main(VS_INPUT input)
{
	return input;
}