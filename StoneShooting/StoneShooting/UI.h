#pragma once
#include "Camera.h"
#include "Shader.h"
#include "GameObject.h"

class UICamera : public CCamera
{
public:
	RECT Monitor_Area;
	bool Active = false;

	UICamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	virtual ~UICamera() {}

	virtual int Update(float fTimeElapsed, bool sign);
	virtual void Reset();
	void SetOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ);
};

class UI : public UICamera
{
public:
	UIShader* m_uiShaders = NULL;
	int m_n_uiShaders = 1;

	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, RECT& monitor_area);
	virtual ~UI() {}

	virtual int Update(float fTimeElapsed, bool sign);

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void Reset();

};

class BAR_UI : public UI
{
public:
	BAR_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, RECT& monitor_area, bool Right_Left = true);
	~BAR_UI();
	virtual int Update(float fTimeElapsed, bool power_charge);
	void Reset();
protected:
	int Max_Width;
	int Max_Height;

	bool Right_Start = true;

	int Degree = 0;
	bool Degree_increase = true;
};