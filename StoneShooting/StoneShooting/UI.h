#pragma once
#include "Camera.h"
#include "Shader.h"
#include "GameObject.h"

struct CB_UI_INFO
{
	float scale; // 1~100 ������ ��
	int fixType; // �����Ǵ� �� : 1~4 == ��,��,��,��
};

class UI_Object : public CGameObject
{
protected:
	ID3D12Resource* UI_Constant_Buffer = NULL;
	CB_UI_INFO* Mapped_UI_info = NULL;

public:
	UI_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~UI_Object();

	void Create_UI_Info_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_UI_Info(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
};

class BAR_UI_Object : public UI_Object
{
public:
	int sticked_side = 0;
	float Degree = 0;
	bool Degree_increase = true;

public:

	BAR_UI_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int stick_type = 3);
	virtual ~BAR_UI_Object();

	virtual void Update_BAR_UI_Info(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Animate(float fElapsedTime, bool power);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);

	float Get_Degree();
};

//================================================================================
// UI ��ü���� �����ϰ� �׸��� �� ����ϴ� ��ü
class UICamera : public CCamera
{
public:
	RECT Monitor_Area;
	bool Active = false;

	UICamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	virtual ~UICamera() {}

	virtual void Update(float fTimeElapsed, bool sign);
	virtual void Reset();
	void SetOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ);
};

class UI : public UICamera
{
public:
	std::vector<UI_Object*>ui_object;

	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	virtual ~UI() {}

	virtual void Update(float fTimeElapsed, bool sign);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);

	virtual void Reset();

};

class BAR_UI : public UI
{
private:
	BAR_UI_Object* bar_obj = NULL;
	UI_Object* endline_obj = NULL;

public:
	BAR_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	~BAR_UI();
	virtual void Update(float fTimeElapsed, bool power_charge);
	virtual void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);

	void Set_Bar_obj(BAR_UI_Object* bar);
	float Get_Degree();
	void Reset();
};