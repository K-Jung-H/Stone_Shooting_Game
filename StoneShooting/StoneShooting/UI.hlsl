cbuffer ScaleBuffer : register(b0)
{
    float scale; // 1~100 사이의 값
    int fixType; // 1~4의 값으로 고정되는 면을 결정
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
};


// 정점 셰이더의 입력을 위한 구조체를 선언
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

// 정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


VS_OUTPUT BAR_UI_VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // scale 값의 범위를 0.0에서 1.0으로 변환
    float scaledValue = scale / 600.0f;

    float3 scaledPosition = input.position;

    // fixType에 따라 고정되는 면 변경
    if (fixType == 1)  // 상단 고정
    {
        if (scaledPosition.y < 0.0f)
        {
            scaledPosition.y = scaledPosition.y - 2 * (scaledPosition.y * (1.0f - scaledValue));
        }
    }
    else if (fixType == 2)  // 하단 고정
    {
        if (scaledPosition.y > 0.0f)
        {
            scaledPosition.y = scaledPosition.y - 2 * (scaledPosition.y * (1.0f - scaledValue));
        }
    }
    else if (fixType == 3)  // 좌측 고정
    {
        if (scaledPosition.x > 0.0f)
        {
            scaledPosition.x = scaledPosition.x - 2 * (scaledPosition.x * (1.0f - scaledValue));
        }
    }
    else if (fixType == 4)  // 우측 고정
    {
        if (scaledPosition.x < 0.0f)
        {
            scaledPosition.x = scaledPosition.x - 2 * (scaledPosition.x * (1.0f - scaledValue));
        }
    }
    else // fixType == 0
    {
        // None
    }

    // 클립 공간으로 변환
    output.position = mul(mul(float4(scaledPosition, 1.0f), gmtxGameObject), gmtxProjection);
    output.color = input.color;
    return (output);
}

float4 BAR_UI_PS(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}