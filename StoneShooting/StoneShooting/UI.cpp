#include "stdafx.h"
#include "UI.h"

//=================================================================================
// UICamera 생성자
UICamera::UICamera(RECT& monitor_area) : CCamera()
{
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
// BAR_UI 생성자
BAR_UI::BAR_UI(RECT& monitor_area) : UICamera(monitor_area)
{
	Max_Width = monitor_area.right - monitor_area.left;
	Max_Height = monitor_area.bottom - monitor_area.top;
}

BAR_UI::~BAR_UI()
{
}

int BAR_UI::Update(float fTimeElapsed, bool power_charge)
{
	if (!Active)
		return 0;

	DebugOutput(std::to_string(Degree));

	if (power_charge)
	{
		if (Degree_increase) {
			if (Degree < 400) 
				Degree += 3;			
			else {
				// 400에 도달하면 증가 상태를 false로 설정
				Degree_increase = false;  
			}
		}
		else {
			if (Degree > 300)
				Degree -= 3;
			else if (Degree > 200)
				Degree -= 2;
			else if (Degree > 100)
				Degree -= 1;
			else
				Degree = 100;  // Degree 100 이하 방지
		}
		SetScissorRect(Monitor_Area.right - Degree/2, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);
	}
	else
		SetScissorRect(Monitor_Area.left, 0, Monitor_Area.right, FRAME_BUFFER_HEIGHT);

	if( 380 <= Degree && Degree <= 400)
		return 500;

	if (Degree_increase == false && Degree == 100)
	{
		return 100;
	}

	return Degree;
}

void BAR_UI::Reset()
{
	Degree = 0;
	Degree_increase = true;
}