#pragma once
#include "GameObject.h"
#include "Particle.h"
extern CMaterial* material_color_item_outer;
extern CMaterial* material_color_item_inner_red;
extern CMaterial* material_color_item_inner_green;
extern CMaterial* material_color_item_inner_blue;

class Item : public CRotatingObject
{
public:
	CRotatingObject* outer_frame = NULL;
	CGameObject* inner_frame = NULL;

	bool is_ground = false;

public:
	Item_Type item_type = Item_Type::ETC;
	static CMesh* outer_Mesh;
	static CMesh* inner_Mesh;
	static void Prepare_Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Item_Type type = Item_Type::ETC);
	virtual ~Item();

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);

	virtual BoundingOrientedBox Get_Collider();
};


struct Stone_Item_Info
{
	StoneObject*	stone		= NULL;
	Particle*		particle	= NULL;
	int				turn		= 0;

	Stone_Item_Info(StoneObject* s, Particle* p, int t) { stone = s; particle = p; turn = t; };
};


class Item_Manager
{
	Stone_Item_Info* Double_Power_obj = NULL;
	Stone_Item_Info* Max_Power_obj = NULL;
	Stone_Item_Info* Fire_Shot_obj = NULL;

	std::vector<Stone_Item_Info*> Frozen_Time_obj;
	std::vector<Stone_Item_Info*> Ghost_obj;
	std::vector<Stone_Item_Info*> Taunt_obj;

	BoundingSphere Snow_Area;

	void Update_Frozen_Time(float fTimeElapsed);
	void Update_Ghost(float fTimeElapsed);
	void Check_Stone_Frozen_Time_Effect(std::vector<StoneObject*>* stones);

public:
	LIGHT* Frozen_Light = NULL;

	Item_Manager();
	~Item_Manager();

	void Add_Stone_Item_Applied(StoneObject* stone);

	Stone_Item_Info* Get_Stone(Item_Type type);
	std::vector<Stone_Item_Info*>* Get_Stone_Iist(Item_Type type);
	int Get_Active_Stone_Num(Item_Type type);

	void Set_Clear(Item_Type type);
	
	// 판 위의 아이템에 영향받는지 판별
	void Check_Stone_Item_Effect(std::vector<StoneObject*>* stones);

	void Animate(float fTimeElapsed);
	void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};