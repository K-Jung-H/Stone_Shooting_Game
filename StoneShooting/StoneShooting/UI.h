#pragma once
#include "Camera.h"
#include "Shader.h"
#include "GameObject.h"

struct CB_BAR_UI_INFO
{
	float scale; // 1~100 사이의 값
	int fixType; // 고정되는 면 : 1~4 == 상,하,좌,우
};

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
	std::vector<CGameObject*>ui_object;

	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	virtual ~UI() {}

	virtual int Update(float fTimeElapsed, bool sign);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pShader);

	virtual void Reset();

};

class BAR_UI : public UI
{
private:
	ID3D12Resource* Bar_Constant_Buffer = NULL;
	CB_BAR_UI_INFO* Mapped_Bar_info = NULL;

public:
	BAR_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area, int stick_side = 3);
	~BAR_UI();
	virtual int Update(float fTimeElapsed, bool power_charge);
	void Create_BarInfo_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pShader);
	void Reset();
protected:
	int Max_Width;
	int Max_Height;

	int sticked_side = 1;

	float Degree = 0;
	bool Degree_increase = true;
};