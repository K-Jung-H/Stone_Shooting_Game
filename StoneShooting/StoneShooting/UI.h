#pragma once
#include "Camera.h"
#include "Shader.h"
#include "GameObject.h"

struct CB_UI_INFO
{
	float scale; // 1~100 사이의 값
	int fixType; // 고정되는 면 : 1~4 == 상,하,좌,우
};

class UI_Object : public CGameObject
{
protected:
	ID3D12Resource* UI_Constant_Buffer = NULL;
	CB_UI_INFO* Mapped_UI_info = NULL;

	UI_Type type = UI_Type::Standard;

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

	bool Charging = false;

public:

	BAR_UI_Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int stick_type = 3);
	virtual ~BAR_UI_Object();

	virtual void Update_BAR_UI_Info(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);

	float Get_Degree();
	void Set_Charging_Mode(bool charge);
};

//================================================================================
// UI 객체들을 관리하고 그리는 걸 담당하는 객체
class UICamera : public CCamera
{
public:
	RECT Monitor_Area;
	bool Active = false;

	UICamera(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	virtual ~UICamera() {}

	virtual void Reset();
	void SetOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ);
};

class UI : public UICamera
{
public:
	std::vector<UI_Object*>ui_object;

	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	virtual ~UI() {}

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
	float Max_Width;
	float Max_Height;
public:
	BAR_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	~BAR_UI();

	virtual void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);

	void Set_Bar_obj(BAR_UI_Object* bar);
	BAR_UI_Object* Get_Bar_obj();

	void Set_Bar_Charge_Mode(bool charge);

	float Get_Degree();
	void Reset();
};

class Inventory_UI : public UI
{
private:
	bool visualize = false;
	bool Drag_up = false;

	XMFLOAT3 Drag_Direction = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float Drag_Speed = 180.0f;

	UI_Object* inventory_board_obj = NULL;
public:
	Inventory_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, RECT& monitor_area);
	~Inventory_UI();
	
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader);

	void Set_Inventory_board_obj(UI_Object* obj) { inventory_board_obj = obj; }
	UI_Object* Get_Inventory_board_obj() { return inventory_board_obj; }
	void Set_Visualize(bool Bool);

	void Reset();
};