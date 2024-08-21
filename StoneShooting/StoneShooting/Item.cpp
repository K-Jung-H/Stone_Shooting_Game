#include "stdafx.h"
#include "Item.h"
#include "Mesh.h"

CMesh* Item::outer_Mesh = NULL;
CMesh* Item::inner_Mesh = NULL;

CMaterial* material_color_item_outer = NULL;
CMaterial* material_color_item_inner_red = NULL;
CMaterial* material_color_item_inner_green = NULL;
CMaterial* material_color_item_inner_blue = NULL;
CMaterial* material_color_item_inner_gray = NULL;
CMaterial* material_color_item_inner_purple = NULL;
CMaterial* material_color_item_inner_icy = NULL;
CMaterial* material_color_item_inner_orange = NULL;


void Item::Prepare_Item(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (outer_Mesh == NULL && inner_Mesh == NULL)
	{
		outer_Mesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 13.0f, 13.0f, 13.0f);
		inner_Mesh = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, 8.0f, 20, 20, 1.0f);
	}

	CMaterialColors yellow_color = {
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f),
		XMFLOAT4(0.3f, 0.3f, 0.0f, 1.0f)
	};

	CMaterialColors red_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f),
	XMFLOAT4(0.3f, 0.0f, 0.0f, 1.0f)
	};

	CMaterialColors green_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f),
	XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f),
	XMFLOAT4(0.0f, 0.3f, 0.0f, 1.0f)
	};

	CMaterialColors blue_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.3f, 1.0f),
	XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f),
	XMFLOAT4(0.0f, 0.0f, 0.3f, 1.0f)
	};

	CMaterialColors gray_color = {
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 30.0f),
		XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)
	};

	CMaterialColors purple_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 30.0f),
	XMFLOAT4(0.15f, 0.1f, 0.8f, 1.0f)
	};

	CMaterialColors icy_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 30.0f),
	XMFLOAT4(0.05f, 0.7f, 1.0f, 1.0f),
	};

	CMaterialColors orange_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(2.0f, 0.5f, 0.0f, 1.0f),
	XMFLOAT4(0.1f, 0.1f, 0.1f, 30.0f),
	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f)
	};

	material_color_item_inner_red = new CMaterial(&red_color);
	material_color_item_inner_orange = new CMaterial(&orange_color);
	material_color_item_outer = new CMaterial(&yellow_color);
	material_color_item_inner_green = new CMaterial(&green_color);
	material_color_item_inner_icy = new CMaterial(&icy_color);
	material_color_item_inner_blue = new CMaterial(&blue_color);
	material_color_item_inner_purple = new CMaterial(&purple_color);
	material_color_item_inner_gray = new CMaterial(&gray_color);
}

Item::Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Item_Type i_type)
	: CRotatingObject(pd3dDevice, pd3dCommandList)
{
	o_type = Object_Type::Item;
	item_type = i_type;

	outer_frame = new CRotatingObject(pd3dDevice, pd3dCommandList);	
	outer_frame->SetMesh(outer_Mesh);
	outer_frame->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	outer_frame->SetRotationSpeed(30.0f);
	outer_frame->SetMaterial(material_color_item_outer);
	


	inner_frame = new CGameObject(pd3dDevice, pd3dCommandList);
	inner_frame->SetMesh(inner_Mesh);

	switch (item_type)
	{
	case Item_Type::Double_Power:
		inner_frame->SetMaterial(material_color_item_inner_green);
		break;

	case Item_Type::Fire_Shot:
		inner_frame->SetMaterial(material_color_item_inner_orange);
		break;

	case Item_Type::Ghost:
		inner_frame->SetMaterial(material_color_item_inner_gray);
		break;

	case Item_Type::Max_Power:
		inner_frame->SetMaterial(material_color_item_inner_purple);
		break;

	case Item_Type::Frozen_Time:
		inner_frame->SetMaterial(material_color_item_inner_icy);
		break;

	case Item_Type::Taunt:
		inner_frame->SetMaterial(material_color_item_inner_red);
		break;

	case Item_Type::ETC:
		inner_frame->SetMaterial(material_color_item_outer);
		break;

	default:
		break;
	}

	Add_Child(inner_frame);
	Add_Child(outer_frame);


}

Item::~Item()
{
}

void Item::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	CGameObject::Render(pd3dCommandList, pCamera, pShader);
}

void Item::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{

	if (outer_frame)
	{
		outer_frame->Animate(fTimeElapsed, pxmf4x4Parent);
	}

	if (inner_frame)
	{
		inner_frame->Animate(fTimeElapsed, pxmf4x4Parent);
	}

	// 자식 객체가 있다면, 해당 객체들 업데이트
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);

	outer_frame->m_pMesh->m_xmBoundingBox.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));


}


//===============================================================================

void Item_Manager::Add_Stone_Item_Applied(StoneObject* stone)
{
	switch (stone->used_item)
	{	
	case Item_Type::Taunt:
		Taunt_obj.push_back(stone);
		break;

	case Item_Type::Frozen_Time:
		Frozen_Time_obj.push_back(stone);
		break;

	case Item_Type::Ghost:
		Ghost_obj.push_back(stone);
		break;
	case Item_Type::Fire_Shot:
		Fire_Shot_obj = stone;
		break;
	case Item_Type::Double_Power:
		Double_Power_obj = stone;
		break;

	case Item_Type::Max_Power:
		Max_Power_obj = stone;
		break;

	default:
		break;
	}

}

std::vector<StoneObject*>* Item_Manager::Get_Stone_List(Item_Type type)
{
	switch (type)
	{
	case Item_Type::Taunt:
		return &Taunt_obj;
		break;

	case Item_Type::Frozen_Time:
		return &Frozen_Time_obj;
		break;

	case Item_Type::Ghost:
		return &Ghost_obj;
		break;

	default:
		break;
	}

	return NULL;
}

StoneObject* Item_Manager::Get_Stone(Item_Type type)
{
	switch (type)
	{
	case Item_Type::Fire_Shot:
		return Fire_Shot_obj;
		break;

	case Item_Type::Double_Power:
		return Double_Power_obj;
		break;

	case Item_Type::Max_Power:
		return Max_Power_obj;
		break;

	default:
		break;
	}

	return NULL;
}

void Item_Manager::Set_Clear(Item_Type type)
{
	switch (type)
	{
	case Item_Type::Fire_Shot:

		Fire_Shot_obj = NULL;
		break;
	case Item_Type::Double_Power:

		Double_Power_obj = NULL;
		break;
	case Item_Type::Max_Power:
		Max_Power_obj = NULL;
		break;

	case Item_Type::Taunt:
		Taunt_obj.clear();
		break;

	case Item_Type::Frozen_Time:
		Frozen_Time_obj.clear();
		break;

	case Item_Type::Ghost:
		Ghost_obj.clear();
		break;

	case Item_Type::ETC:
	case Item_Type::None:
	default:
		break;
	}
}