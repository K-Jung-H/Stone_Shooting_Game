#pragma once
#include "GameObject.h"

extern CMaterial* material_color_item_outer;
extern CMaterial* material_color_item_inner_red;
extern CMaterial* material_color_item_inner_green;
extern CMaterial* material_color_item_inner_blue;

class Item : public CRotatingObject
{
public:
	CRotatingObject* outer_frame = NULL;
	CGameObject* inner_frame = NULL;

public:
	Item_Type item_type = Item_Type::ETC;
	static CMesh* outer_Mesh;
	static CMesh* inner_Mesh;
	static void Prepare_Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Item_Type type = Item_Type::ETC);
	virtual ~Item();

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
};


/*
	Taunt,
	Fire_Shot,
	Double_Power,
	Frozen_Time,
	Max_Power,
	Ghost,
	ETC,
	None,

*/
class Item_Manager
{
	StoneObject* Double_Power_obj = NULL;
	StoneObject* Max_Power_obj = NULL;
	StoneObject* Fire_Shot_obj = NULL;

	std::vector<StoneObject*> Frozen_Time_obj;
	std::vector<StoneObject*> Ghost_obj;
	std::vector<StoneObject*> Taunt_obj;

public:
	int Double_Power_count = 0;
	LIGHT* Frozen_Light = NULL;


	void Add_Stone_Item_Applied(StoneObject* stone);
	std::vector<StoneObject*>* Get_Stone_List(Item_Type type);

	StoneObject* Get_Stone(Item_Type type);
	void Set_Clear(Item_Type type);
	
	void Animate(float fTimeElapsed);
};