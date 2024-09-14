cbuffer cb_UI_Transform : register(b0)
{
    float4x4 transformMatrix; // Transform ���
}

cbuffer Index_Buffer : register(b1) 
{
    int sprite_index; 
};

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


float2 Loading_Sprite_UV(int n)
{
    float2 uv;
    int index = sprite_index;
    
    // �ε����� ���� ���� ��ȣ�� �Է� �޾Ƽ� ��ȯ�ؾ� ��
    if (n== 0)
    {
        uv.x = (sprite_index % 12) * 0.083333;
        uv.y = (sprite_index / 12) * 0.25;
    }
    else if (n == 1)
    {
        uv.x = (sprite_index % 12) * 0.083333 + 0.083333;
        uv.y = (sprite_index / 12) * 0.25;
    }
    else if (n == 2)
    {
        uv.x = (sprite_index % 12) * 0.083333;
        uv.y = (sprite_index / 12) * 0.25 + 0.25;
    }
    else if (n == 3)
    {
        uv.x = (sprite_index % 12) * 0.083333 + 0.083333;
        uv.y = (sprite_index / 12) * 0.25 + 0.25;
    }
    
    return uv;
}

VS_TEXTURED_OUTPUT VSTextured_UI(uint nVertexID : SV_VertexID)
{
    VS_TEXTURED_OUTPUT output;

    // Vertex ID�� ���� �簢���� ��ġ�� �����մϴ�.
    if (nVertexID == 0)
    {
        output.position = float4(-1.0, 1.0, 0.0, 1.0); // �»��
        output.uv = Loading_Sprite_UV(0);

    }
    else if (nVertexID == 1)
    {
        output.position = float4(1.0, 1.0, 0.0, 1.0); // ����
        output.uv = Loading_Sprite_UV(1);
    }
    else if (nVertexID == 2)
    {
        output.position = float4(-1.0, -1.0, 0.0, 1.0); // ���ϴ�
        output.uv = Loading_Sprite_UV(2);
    }
    else if (nVertexID == 3)
    {
        output.position = float4(1.0, -1.0, 0.0, 1.0); // ���ϴ�
        output.uv = Loading_Sprite_UV(3);
    }
    // Transform ����� ����Ͽ� ��ġ ��ȯ
    output.position = mul(output.position, transformMatrix);
    return output;
}

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

float4 PSTextured_UI(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    // UV ��ǥ�� ���� �ؽ��� ���ø�
    float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
    return cColor;
}
