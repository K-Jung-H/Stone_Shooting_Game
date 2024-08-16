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
 
//게임 객체의 데이터를 위한 상수 버퍼\
cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
};

cbuffer cbOutline : register(b5)
{
    float4 color : packoffset(c0);
    float OutlineThickness : packoffset(c1);
}
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
    // 정점을 확장하여 윤곽선을 생성
    float4 expandedPosition = float4(input.position * (1.0f + OutlineThickness), 1.0f);
    // 월드, 뷰, 투영 행렬을 적용하여 최종 위치 계산
    output.position = mul(mul(expandedPosition, gmtxGameObject), mul(gmtxView, gmtxProjection));
    return output;
}

// 픽셀 셰이더
float4 PSOutline(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
    // 윤곽선 색상 설정 (검은색)
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}