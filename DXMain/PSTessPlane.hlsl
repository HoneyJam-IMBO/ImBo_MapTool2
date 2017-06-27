#include "PackGbuffer.hlsli"
//#include "GetWorldNormal.hlsli"

Texture2D<float4> HDRTex : register(t0);
Texture2D<float> DepthTex : register(t1);
Texture2D NormalTex : register(t2);
//Texture2D<float4> Color_Depth : register(t3);
TextureCube gtxtSkyBox : register(t13);

texture2D gtxtBase : register(t3);

texture2D gtxtNormal : register(t5);

SamplerState gssWRAP_LINEAR : register(s0);
SamplerState gssWRAP_POINT : register(s1);
SamplerState gssCLAMP_LINEAR : register(s2);
SamplerState gssCLAMP_POINT : register(s3);
SamplerState gssSHADOW : register(s4);

cbuffer SSReflectionPSConstants : register(b0)
{
	float4x4 ProjMatrix			: packoffset(c0);
	float ViewAngleThreshold : packoffset(c4.x);
	float EdgeDistThreshold : packoffset(c4.y);
	float minDepthBias : packoffset(c4.z);
	float maxDepthBias : packoffset(c4.w);
	float4 PerspectiveValues	: packoffset(c5);
	float ReflectionScale : packoffset(c6);
	float PixelSize : packoffset(c6.y);// 2.f / 768.0f;
	int nNumSteps : packoffset(c6.z);// 1024 / 2;

}
cbuffer gMaterialInfo : register(b3) {
	float4 gMaterialColor : packoffset(c0);
	float gSpecExp : packoffset(c1.x);
	float gSpecIntensity : packoffset(c1.y);
}

struct Material {
	float3 normal;
	float4 diffuseColor;
	float specExp;
	float specIntensity;
};

struct DS_OUT {
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 texCoord : TEXCOORD0;
	float4 ViewPosition	: TEXCOORD1;
	float3 ViewNormal	: TEXCOORD2;
	float3 csPos		: TEXCOORD3;
};
float ConvertZToLinearDepth(float depth)
{
	float linearDepth = PerspectiveValues.z / (depth + PerspectiveValues.w);
	return linearDepth;
}
float3 CalcViewPos(float2 csPos, float depth)
{
	float3 position;

	position.xy = csPos.xy * PerspectiveValues.xy * depth;
	position.z = depth;

	return position;
}

// 클립 공간의 픽셀 크기
// HDR 너비와 높이의 최대 값 선택


float4 main(DS_OUT In) : SV_TARGET
{
	// Pixel position and normal in view space
	float3 vsPos = In.ViewPosition.xyz;
	float3 vsNorm = normalize(In.ViewNormal);
	float4 sv_pos = mul(float4(vsPos, 1.f), ProjMatrix);
	float3 csPos = sv_pos.xyz / sv_pos.w;
	float2 UV = csPos.xy;
	UV = UV* float2(0.5, -0.5) + 0.5;

	// Calculate the camera to pixel direction
	float3 eyeToPixel = normalize(vsPos);

	// Calculate the reflected view direction
	float3 vsReflect = reflect(eyeToPixel,  vsNorm);

	// The initial reflection color for the pixel
	//float4 reflectColor = float4(0.0, 0.0, 0.0, 0.0);
	float4 reflectColor = gtxtSkyBox.Sample(gssWRAP_LINEAR, vsReflect);
	reflectColor.w = 0.4 * ReflectionScale;

	// Don't bother with reflected vector above the threshold vector
	if (vsReflect.z >= ViewAngleThreshold )
	{
		// Fade the reflection as the view angles gets close to the threshold
		float viewAngleThresholdInv = 1.0 - ViewAngleThreshold;
		float viewAngleFade = saturate(3.0 * (vsReflect.z - ViewAngleThreshold) / viewAngleThresholdInv);

		// Transform the View Space Reflection to clip-space
		float3 vsPosReflect = vsPos + vsReflect;
		float3 csPosReflect = mul(float4(vsPosReflect, 1.0), ProjMatrix).xyz / vsPosReflect.z;
		float3 csReflect = csPosReflect - csPos;

		// Resize Screen Space Reflection to an appropriate length.
		float reflectScale = PixelSize / length(csReflect.xy);
		csReflect *= reflectScale;

		// Calculate the first sampling position in screen-space
		float2 ssSampPos = (csPos + csReflect).xy;
		ssSampPos = ssSampPos * float2(0.5, -0.5) + 0.5;

		// Find each iteration step in screen-space
		float2 ssStep = csReflect.xy * float2(0.5, -0.5);

		// Build a plane laying on the reflection vector
		// Use the eye to pixel direction to build the tangent vector
		float4 rayPlane;
		float3 vRight = cross(eyeToPixel, vsReflect);
		rayPlane.xyz = normalize(cross(vsReflect, vRight));
		rayPlane.w = dot(rayPlane.xyz, vsPos);

		// Iterate over the HDR texture searching for intersection
		for (int nCurStep = 0; nCurStep < nNumSteps; nCurStep++)
		{
			// Sample from depth buffer
			float curDepth = DepthTex.SampleLevel(gssCLAMP_POINT, ssSampPos, 0.0).x;
			if (curDepth > 0.999) break;

			float curDepthLin = ConvertZToLinearDepth(curDepth);//NormalTex.SampleLevel(gssCLAMP_POINT, ssSampPos, 0.0).w * 3000.f;//
			float3 curPos = CalcViewPos(csPos.xy + csReflect.xy * ((float)nCurStep + 1.0), curDepthLin);

			// Find the intersection between the ray and the scene
			// The intersection happens between two positions on the oposite sides of the plane
			float d = rayPlane.w - dot(rayPlane.xyz, curPos);
			if (d >= minDepthBias && d < maxDepthBias)
			{
				// Calculate the actual position on the ray for the given depth value
				float3 vsFinalPos = vsPos + (vsReflect / abs(vsReflect.z)) * abs(curDepthLin - vsPos.z + minDepthBias);
				float2 csFinalPos = vsFinalPos.xy / PerspectiveValues.xy / vsFinalPos.z;
				ssSampPos = csFinalPos.xy * float2(0.5, -0.5) + 0.5;

				// Get the HDR value at the current screen space location
				reflectColor.xyz = HDRTex.SampleLevel(gssCLAMP_POINT, ssSampPos, 0.0).xyz;

				// Fade out samples as they get close to the texture edges
				float edgeFade = saturate(distance(ssSampPos, float2(0.5, 0.5)) * 2.0 - EdgeDistThreshold);

				// Calculate the fade value
				reflectColor.w = min(viewAngleFade, 1.0 - edgeFade * edgeFade);

				// Apply the reflection sacle
				reflectColor.w *= ReflectionScale;

				// Advance past the final iteration to break the loop
				nCurStep = nNumSteps;
			}

			// Advance to the next sample
			ssSampPos += ssStep;
		}
	}

	return reflectColor;
}
