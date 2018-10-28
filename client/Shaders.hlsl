cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
};

cbuffer cbSkinned : register(b5)
{
	// 한 캐릭터 당 최대 뼈대 개수는 96
	matrix gBoneTransform[96];// : packoffset(c0);
};

//Terrain 추가 
struct CB_CAMERA_INFO
{
	matrix		mtxView;
	matrix		mtxProjection;
};

struct CB_GAMEOBJECT_INFO
{
	matrix		mtxWorld;
};
ConstantBuffer<CB_CAMERA_INFO> gcbCameraInfo : register(b1);
ConstantBuffer<CB_GAMEOBJECT_INFO> gcbGameObjectInfo : register(b2);


/////////////////////////



#include "Light.hlsl"

struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);
SamplerState gWrapSamplerState : register(s0);


//Terrian 추가
SamplerState gClampSamplerState : register(s1);


struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);

	return(cColor);
}

#define _WITH_VERTEX_LIGHTING

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
return(cIllumination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	//추가 05.08
	float4 weights : WEIGHTS;
	float4 boneindices: BONEINDICES;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	//셰이더 추가 05.08 
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.weights.x;
	weights[1] = input.weights.y;
	weights[2] = input.weights.z;
	weights[3] = 1 - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i)
	{
		posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransform[input.boneindices[i]]).xyz;
		normalL += weights[i] * mul(input.normal, (float3x3)gBoneTransform[input.boneindices[i]]);
	}


	//==============================================

	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	//output.normalW = mul(normalL, (float3x3) gmtxGameObject);
	//output.uv = input.uv;

	//return output;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	//output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject);
	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{
	//input.normalW = normalize(input.normalW);
	//float4 cIllumination = Lighting(input.positionW, input.normalW);
	//float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv) * cIllumination;
	//
	//return (lerp(cColor, cIllumination, 0.5f));

	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(lerp(cColor, cIllumination, 0.5f));
}
////////////////////////////////////////////////////////// Backgroundobject
struct VS_NONANIMATE_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	//추가 05.08

};

struct VS_TEXTURED_NONANIMATE_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_TEXTURED_NONANIMATE_OUTPUT VSNONANIMATE(VS_NONANIMATE_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	////셰이더 추가 05.08 
	//float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//weights[0] = input.weights.x;
	//weights[1] = input.weights.y;
	//weights[2] = input.weights.z;
	//weights[3] = 1 - weights[0] - weights[1] - weights[2];

	//float3 posL = float3(0.0f, 0.0f, 0.0f);
	//float3 normalL = float3(0.0f, 0.0f, 0.0f);
	//for (int i = 0; i < 4; ++i)
	//{
	//	posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransform[input.boneindices[i]]).xyz;
	//	normalL += weights[i] * mul(input.normal, (float3x3)gBoneTransform[input.boneindices[i]]);
	//}


	//==============================================

	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	//output.normalW = mul(normalL, (float3x3) gmtxGameObject);
	output.uv = input.uv;

	//return output;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject);
	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);



#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSNONANIMATE(VS_TEXTURED_NONANIMATE_OUTPUT input) : SV_TARGET
{
	//input.normalW = normalize(input.normalW);
	//float4 cIllumination = Lighting(input.positionW, input.normalW);
	//float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv) * cIllumination;
	//
	//return (lerp(cColor, cIllumination, 0.5f));

	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(lerp(cColor, cIllumination, 0.5f));
}

VS_TEXTURED_NONANIMATE_OUTPUT VSNONANIMATESHADOW(VS_NONANIMATE_INPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

	////셰이더 추가 05.08 
	//float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//weights[0] = input.weights.x;
	//weights[1] = input.weights.y;
	//weights[2] = input.weights.z;
	//weights[3] = 1 - weights[0] - weights[1] - weights[2];

	//float3 posL = float3(0.0f, 0.0f, 0.0f);
	//float3 normalL = float3(0.0f, 0.0f, 0.0f);
	//for (int i = 0; i < 4; ++i)
	//{
	//	posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransform[input.boneindices[i]]).xyz;
	//	normalL += weights[i] * mul(input.normal, (float3x3)gBoneTransform[input.boneindices[i]]);
	//}


	//==============================================

	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	//output.normalW = mul(normalL, (float3x3) gmtxGameObject);
    output.uv = input.uv;

	//return output;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject);
	//output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);



#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}

float4 PSNONANIMATESHADOW(VS_TEXTURED_NONANIMATE_OUTPUT input) : SV_TARGET
{
	//input.normalW = normalize(input.normalW);
	//float4 cIllumination = Lighting(input.positionW, input.normalW);
	//float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv) * cIllumination;
	//
	//return (lerp(cColor, cIllumination, 0.5f));

    float4 cColor = (1.0f, 1.0f, 1.0f, 0.08f);
#ifdef _WITH_VERTEX_LIGHTING
    float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
    return (lerp(cColor, cIllumination, 0.5f));




}




////////////////////////////////////////////////////////////////////Terrian 추가  11.26문제 Terrian shader 인식 못함. 코드 풀면 다른 것도 못읽음 레지스터 지정문제인듯 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
Texture2D gtxtTerrainBaseTexture : register(t6);
Texture2D gtxtTerrainDetailTexture : register(t7);

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

#ifdef _WITH_CONSTANT_BUFFER_SYNTAX
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gvCameraPosition);
#else
	//output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);  기존 터레인 코드 이거임 근데 else일때도 ifdef때와 같이해줌 
	// 플레이어 시선따라 터레인 움직여서       gmtxGameObject), gmtxView), gmtxProjection); 
	//output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gvCameraPosition);
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
#endif
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gWrapSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gWrapSamplerState, input.uv1);
	float4 cColor = input.color * saturate((cBaseTexColor * 0.6f) + (cDetailTexColor * 0.6f));

	return(cColor);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
Texture2D gtxtSkyBox : register(t8);

float4 PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyBox.Sample(gClampSamplerState, input.uv);

	return(cColor);
}

////////////////
struct VS_WATER_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_WATER_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_WATER_TERRAIN_OUTPUT VSWaterTerrain(VS_WATER_TERRAIN_INPUT input)
{
	VS_WATER_TERRAIN_OUTPUT output;

#ifdef _WITH_CONSTANT_BUFFER_SYNTAX
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
#else
	//output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);  기존 터레인 코드 이거임 근데 else일때도 ifdef때와 같이해줌 
	// 플레이어 시선따라 터레인 움직여서       gmtxGameObject), gmtxView), gmtxProjection); 
	//output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gvCameraPosition);
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
#endif
	output.color = input.color;

	output.uv0 = input.uv0;

	output.uv1 = input.uv1;
	return(output);
}

float4 PSWaterTerrain(VS_WATER_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gWrapSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gWrapSamplerState, input.uv1);
	float4 cColor = input.color * saturate((cBaseTexColor * 0.6f) + (cDetailTexColor * 0.6f));

	return(cColor);
}


//==========//
//Effect
//=================================================================
VS_TEXTURED_NONANIMATE_OUTPUT VS_EFFECT(VS_NONANIMATE_INPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    output.uv = input.uv;

	//return output;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
	//output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}

////
float4 PS_EFFECT(VS_TEXTURED_NONANIMATE_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
#ifdef _WITH_VERTEX_LIGHTING
    float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
    return (lerp(cColor, cIllumination, 0.5f));
}


//==========//
//Effect
//=================================================================
VS_TEXTURED_LIGHTING_OUTPUT VS_Shadow(VS_TEXTURED_LIGHTING_INPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    //input.position.x = float(input.position.x / tan(0.785398));
   // input.position.y = float(input.position.y / tan(0.785398));
    //input.position.z = float(input.position.z / tan(0.785398));

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.weights.x;
    weights[1] = input.weights.y;
    weights[2] = input.weights.z;
    weights[3] = 1 - weights[0] - weights[1] - weights[2];

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransform[input.boneindices[i]]).xyz;
        normalL += weights[i] * mul(input.normal, (float3x3) gBoneTransform[input.boneindices[i]]);
    }
    

	//return output;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
    //output.positionW.y = 5;

    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    


#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}

float4 PS_Shadow(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{

    float4 cColor = (1.0f, 1.0f, 1.0f, 0.08f);
#ifdef _WITH_VERTEX_LIGHTING
    float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
    return (lerp(cColor, cIllumination, 0.5f));
}

