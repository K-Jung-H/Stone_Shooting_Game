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

	Create_Shader_Resource(pd3dDevice, pd3dCommandList);

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
UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, RECT& monitor_area) : UICamera(pd3dDevice, pd3dCommandList, monitor_area)
{
	// UI에 그릴 객체들 + 해당 객체를 그릴 셰이더
	// 셰이더에서 객체 관리
	m_uiShaders = new UIShader[m_n_uiShaders];
	m_uiShaders[0].CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}
void UI::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_n_uiShaders; ++i)
	{
		m_uiShaders[i].AnimateObjects(fTimeElapsed);
	}
}

void UI::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);
	Update_Shader_Resource(pd3dCommandList);

	for (int i = 0; i < m_n_uiShaders; ++i)
	{
		m_uiShaders[i].Render(pd3dCommandList, this);
	}
}

int UI::Update(float fTimeElapsed, bool sign)
{
	return 0;
}

void UI::Reset()
{
}

//=================================================================================
// BAR_UI 생성자
BAR_UI::BAR_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, RECT& monitor_area, bool Right_to_Left)
	: UI(pd3dDevice, pd3dCommandList,pd3dGraphicsRootSignature, monitor_area)
{
	Max_Width = monitor_area.right - monitor_area.left;
	Max_Height = monitor_area.bottom - monitor_area.top;
	Right_Start = Right_to_Left;
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
		if (Right_Start)
			SetScissorRect(Monitor_Area.right - Degree/2, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);
		else
			SetScissorRect(Monitor_Area.left, 0, Monitor_Area.left + Degree / 2, FRAME_BUFFER_HEIGHT);
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

void BAR_UI::Reset()
{
	Degree = 0;
	Degree_increase = true;
}