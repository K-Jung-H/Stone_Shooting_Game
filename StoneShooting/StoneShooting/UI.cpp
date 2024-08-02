#include "stdafx.h"
#include "UI.h"

//=================================================================================
// UICamera 생성자
UICamera::UICamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area) : CCamera()
{
	//UI의 영역값
	Monitor_Area = monitor_area;
	float ui_width = monitor_area.right - monitor_area.left;
	float ui_height = monitor_area.bottom - monitor_area.top; 


	// UI의 뷰포트
	// 뷰포트 설정: 화면에 UI를 배치할 공간 설정 [UI 그려질 위치 및 크기] 
	SetViewport(monitor_area.left, monitor_area.top, ui_width, ui_height, 0.0f, 1.0f);
	
	// UI의 시저 렉트
	// 시저 렉트 설정: 모니터 기준으로 렌더링 될 영역 [영역 밖은 렌더링 X]
	m_d3dScissorRect = monitor_area;

	// UI의 직교 투영 행렬 설정
	// UI의 최대 넓이 길이: ui_width
	// UI의 최대 높이 길이: ui_height
	SetOrthographicProjection(ui_width, ui_height, 0.0f, 100.0f);

	CCamera::Create_Shader_Resource(pd3dDevice, pd3dCommandList);

}

int UICamera::Update(float fTimeElapsed, bool sign)
{
	return 0;
}

void UICamera::Reset()
{
}

// UI 렌더링을 위한 직교 투영 행렬 설정
void UICamera::SetOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ)
{
	m_xmf4x4Projection = Matrix4x4::OrthographicLH(viewWidth, viewHeight, nearZ, farZ);
}

//=================================================================================

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area) : UICamera(pd3dDevice, pd3dCommandList, monitor_area)
{

}
void UI::AnimateObjects(float fTimeElapsed)
{
	for (CGameObject* game_obj : ui_object)
		game_obj->Animate(fTimeElapsed);
}

void UI::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pShader)
{
	SetViewportsAndScissorRects(pd3dCommandList);
//	pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);
	Update_Shader_Resource(pd3dCommandList);  // UI를 그리는 카메라는 내용이 변경될 일 없음
	CCamera::Update_Shader_Resource(pd3dCommandList);

	for (CGameObject* game_obj : ui_object)
		game_obj->Render(pd3dCommandList, this, pShader);

}

int UI::Update(float fTimeElapsed, bool sign)
{
	return 0;
}

void UI::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void UI::Reset()
{
}

//=================================================================================
// BAR_UI 생성자
BAR_UI::BAR_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area, int stick_side)
	: UI(pd3dDevice, pd3dCommandList, monitor_area)
{
	Max_Width = monitor_area.right - monitor_area.left;
	Max_Height = monitor_area.bottom - monitor_area.top;
	//Degree = 100.0f;
	sticked_side = stick_side;
	Create_BarInfo_Buffer(pd3dDevice, pd3dCommandList);
}

BAR_UI::~BAR_UI()
{
}

int BAR_UI::Update(float fTimeElapsed, bool power_charge)
{
	if (!Active)
		return 0;

	//DebugOutput(std::to_string(Degree));

	if (power_charge)
	{
		if (Degree_increase) {
			if (Degree < 600) 
				Degree += 1;			
			else {
				// 400에 도달하면 증가 상태를 false로 설정
				Degree_increase = false;  
			}
		}
		else {
			if (Degree > 300)
				Degree -= 1;
			else if (Degree > 200)
				Degree -= 2;
			else if (Degree > 100)
				Degree -= 3;
			else
				Degree = 100;  // Degree 100 이하 방지
		}
		//if (Right_Start)
		//	SetScissorRect(Monitor_Area.right - Degree/2, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);
		//else
		//	SetScissorRect(Monitor_Area.left, 0, Monitor_Area.left + Degree / 2, FRAME_BUFFER_HEIGHT);
		SetScissorRect(Monitor_Area.left, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);
	}
	else
		SetScissorRect(Monitor_Area.left, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);

	if( 400 <= Degree && Degree <= 600)
		return 600;

	if (Degree_increase == false && Degree == 100)
	{
		return 100;
	}

	return Degree * 1.5;
}


void BAR_UI::Create_BarInfo_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 게임 재질 버퍼 생성 및 매핑
	UINT ncbElementBytes = ((sizeof(CB_BAR_UI_INFO) + 255) & ~255); //256의 배수
	Bar_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	Bar_Constant_Buffer->Map(0, NULL, (void**)&Mapped_Bar_info);
}

void BAR_UI::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CB_BAR_UI_INFO* mapped_bar_info = (CB_BAR_UI_INFO*)Mapped_Bar_info;
	::memcpy(&mapped_bar_info->fixType, &sticked_side, sizeof(int));
	::memcpy(&mapped_bar_info->scale, &Degree, sizeof(float));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = Bar_Constant_Buffer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbGameObjectGpuVirtualAddress);
}

void BAR_UI::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pShader)
{
	SetViewportsAndScissorRects(pd3dCommandList);
	//pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);
	Update_Shader_Resource(pd3dCommandList);  // UI의 정보 업데이트
	CCamera::Update_Shader_Resource(pd3dCommandList);

	ui_object[0]->CGameObject::Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::GameObject_info);
	ui_object[0]->m_pMesh->Render(pd3dCommandList);

	//for (CGameObject* ui_obj : ui_object)
	//	ui_obj->Render(pd3dCommandList, this, pShader);

}

void BAR_UI::Reset()
{
	Degree = 0;
	Degree_increase = true;
}