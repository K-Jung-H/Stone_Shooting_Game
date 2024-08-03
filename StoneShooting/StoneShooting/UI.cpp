#include "stdafx.h"
#include "UI.h"

//=================================================================================
// UICamera ������
UICamera::UICamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area) : CCamera()
{
	//UI�� ������
	Monitor_Area = monitor_area;
	float ui_width = monitor_area.right - monitor_area.left;
	float ui_height = monitor_area.bottom - monitor_area.top; 


	// UI�� ����Ʈ
	// ����Ʈ ����: ȭ�鿡 UI�� ��ġ�� ���� ���� [UI �׷��� ��ġ �� ũ��] 
	SetViewport(monitor_area.left, monitor_area.top, ui_width, ui_height, 0.0f, 1.0f);
	
	// UI�� ���� ��Ʈ
	// ���� ��Ʈ ����: ����� �������� ������ �� ���� [���� ���� ������ X]
	m_d3dScissorRect = monitor_area;

	// UI�� ���� ���� ��� ����
	// UI�� �ִ� ���� ����: ui_width
	// UI�� �ִ� ���� ����: ui_height
	SetOrthographicProjection(ui_width, ui_height, 0.0f, 100.0f);

	CCamera::Create_Shader_Resource(pd3dDevice, pd3dCommandList);

}

void UICamera::Update(float fTimeElapsed, bool sign)
{
}

void UICamera::Reset()
{
}

// UI �������� ���� ���� ���� ��� ����
void UICamera::SetOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ)
{
	m_xmf4x4Projection = Matrix4x4::OrthographicLH(viewWidth, viewHeight, nearZ, farZ);
}

//=================================================================================

UI_Object::UI_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Create_Object_Buffer(pd3dDevice, pd3dCommandList);
	Create_UI_Info_Buffer(pd3dDevice, pd3dCommandList);
	//Update_UI_Info(pd3dCommandList);

}

UI_Object::~UI_Object()
{
}

void UI_Object::Create_UI_Info_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ���� ���� ���� ���� �� ����
	UINT ncbElementBytes = ((sizeof(CB_UI_INFO) + 255) & ~255); //256�� ���
	UI_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	UI_Constant_Buffer->Map(0, NULL, (void**)&Mapped_UI_info);
}

void UI_Object::Update_UI_Info(ID3D12GraphicsCommandList* pd3dCommandList)
{
	int default_side = 4;
	float default_Degree = 10;
	CB_UI_INFO* mapped_bar_info = (CB_UI_INFO*)Mapped_UI_info;
	::memcpy(&mapped_bar_info->fixType, &default_side, sizeof(int));
	::memcpy(&mapped_bar_info->scale, &default_Degree, sizeof(float));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = UI_Constant_Buffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbGameObjectGpuVirtualAddress);
}

void UI_Object::Animate(float fElapsedTime)
{
}

void UI_Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	if (!active)
		return;

	// ���� UI�� �ʿ��� ������ ���� �Լ��� ȣ���ϴ� �������� �̹� ������Ʈ ��
	// Update_UI_Info(pd3dCommandList);

	CGameObject::Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::GameObject_info);

	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList);

	if (m_pSibling)
		m_pSibling->Render(pd3dCommandList, pCamera, pShader);
	if (m_pChild)
		m_pChild->Render(pd3dCommandList, pCamera, pShader);
}

//=================================================================================

BAR_UI_Object::BAR_UI_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int stick_type)
	: UI_Object(pd3dDevice, pd3dCommandList)
{
	sticked_side = stick_type;
	Degree = 90;
	sticked_side = 4;
}

BAR_UI_Object::~BAR_UI_Object()
{
}

void BAR_UI_Object::Update_BAR_UI_Info(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CB_UI_INFO* mapped_bar_info = (CB_UI_INFO*)Mapped_UI_info;
	::memcpy(&mapped_bar_info->fixType, &sticked_side, sizeof(int));
	::memcpy(&mapped_bar_info->scale, &Degree, sizeof(float));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = UI_Constant_Buffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbGameObjectGpuVirtualAddress);
}

void BAR_UI_Object::Animate(float fElapsedTime, bool power)
{
	if (power)
	{
		if (Degree_increase) 
		{
			if (Degree < 600)
				Degree += 1;
			else 
				Degree_increase = false;
		}
		else 
		{
			if (Degree > 300)
				Degree -= 1;
			else if (Degree > 200)
				Degree -= 2;
			else if (Degree > 100)
				Degree -= 3;
			else
				Degree = 100;
		}
	}
}

void BAR_UI_Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	Update_BAR_UI_Info(pd3dCommandList);
	UI_Object::Render(pd3dCommandList, pCamera, pShader);
}

float BAR_UI_Object::Get_Degree()
{
	if (!active)
		return 0;

	if (400 <= Degree && Degree <= 600)
		return 600;

	if (Degree_increase == false && Degree == 100)
		return 100;

	return Degree * 1.5;
}

//=================================================================================

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area) 
	: UICamera(pd3dDevice, pd3dCommandList, monitor_area)
{
}

void UI::AnimateObjects(float fTimeElapsed)
{
	for (UI_Object* game_obj : ui_object)
		game_obj->Animate(fTimeElapsed);
}

void UI::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	SetViewportsAndScissorRects(pd3dCommandList);
	Update_Shader_Resource(pd3dCommandList); 
	CCamera::Update_Shader_Resource(pd3dCommandList); // UI�� �׸��� ī�޶�� ������ ����� �� ����

	for (UI_Object* game_obj : ui_object)
		game_obj->Render(pd3dCommandList, this, pShader);
}

void UI::Update(float fTimeElapsed, bool sign)
{
}

void UI::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void UI::Reset()
{
}

//=================================================================================
// BAR_UI ������
BAR_UI::BAR_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area)
	: UI(pd3dDevice, pd3dCommandList, monitor_area)
{
	Max_Width = monitor_area.right - monitor_area.left;
	Max_Height = monitor_area.bottom - monitor_area.top;
	SetScissorRect(Monitor_Area.left, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);
}

BAR_UI::~BAR_UI()
{
}

void BAR_UI::Update(float fTimeElapsed, bool power_charge)
{
}

void BAR_UI::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader)
{
	SetViewportsAndScissorRects(pd3dCommandList);

	CCamera::Update_Shader_Resource(pd3dCommandList);

	for (UI_Object* ui_obj : ui_object)
	{
		ui_obj->Render(pd3dCommandList, this, pShader);
	}

}

void BAR_UI::Set_Bar_obj(BAR_UI_Object* bar)
{
	if (bar_obj != NULL)
		delete bar_obj;

	bar_obj = bar;
}

float BAR_UI::Get_Degree()
{
	if (bar_obj == NULL)
		return 0;

	return bar_obj->Degree;
}

void BAR_UI::Reset()
{
	if (bar_obj != NULL)
	{
		bar_obj->Degree = 0;
		bar_obj->Degree_increase = true;
	}
}