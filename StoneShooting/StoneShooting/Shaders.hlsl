//플레이어 객체의 데이터를 위한 상수 버퍼
cbuffer cbPlayerInfo : register(b0)
{
    matrix gmtxPlayerWorld : packoffset(c0);
};

//카메라 객체의 데이터를 위한 상수 버퍼
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};
 
//게임 객체의 데이터를 위한 상수 버퍼
cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
};

cbuffer Outline : register(b5)
{
    float4 line_color : packoffset(c0);
    float OutlineThickness : packoffset(c1);
    float padding[3] : packoffset(c2);
};


//========================================================================
// 플레이어를 그리는 셰이더 

#include "Light.hlsl"

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다. 
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


VS_OUTPUT VSPlayer(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}

float4 PSPlayer(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}

//========================================================================


// 객체를 그릴 시, 정점 조명을 사용
#define _WITH_VERTEX_LIGHTING

//정점 쉐이더의 입력 정점 구조
struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

//정점 쉐이더의 출력 정점 구조
struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#else
    float3 normalW : NORMAL;
#endif
};

//정점 쉐이더 함수
VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    float3 normalW = mul(input.normal, (float3x3) gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.color = Lighting(output.positionW, normalize(normalW));
#else
    output.normalW = normalW;
#endif
    return (output);
}

//픽셀 쉐이더 함수
float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    return (input.color);
#else
    float3 normalW = normalize(input.normalW);
    float4 color = Lighting(input.positionW, normalW);
    return(color);
#endif
}
//========================================================================

// 윤곽선 정점 셰이더
VS_LIGHTING_OUTPUT VSOutline(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;
    
    // 노멀 벡터를 따라 윤곽선을 확장
    float4 expandedPosition = float4(input.position + input.normal * OutlineThickness, 1.0f);
    
    // 확장된 위치를 변환
    output.position = mul(mul(mul(expandedPosition, gmtxGameObject), gmtxView), gmtxProjection);
    output.color = line_color;
    return output;
}

// 픽셀 셰이더
float4 PSOutline(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
    // 윤곽선 색상 설정
    return input.color;
}


//========================================================================

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

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

    return (output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
   
    return (cColor);
}
