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
	XMFLOAT4(0.0f, 0.0f, 0.1f, 1.0f),
	XMFLOAT4(0.1f, 0.1f, 0.1f, 30.0f),
	XMFLOAT4(1.0f, 0.3f, 0.0f, 1.0f)
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
	outer_frame->AddMesh(outer_Mesh);
	outer_frame->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	outer_frame->SetRotationSpeed(30.0f);
	outer_frame->SetMaterial(material_color_item_outer);
	


	inner_frame = new CGameObject(pd3dDevice, pd3dCommandList);
	inner_frame->AddMesh(inner_Mesh);

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

	SetMovingDirection(XMFLOAT3(0.0f, -1.0f, 0.0f));
	SetMovingSpeed(30.0f);

	default_collider = outer_frame->mesh_list[0]->m_xmBoundingBox;

	Add_Child(inner_frame);
	Add_Child(outer_frame);
}

Item::~Item()
{
}

void Item::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	if(Is_Visible_Collider(pCamera))
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

	if (!is_ground)
	{
		Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);
		XMFLOAT3 pos = GetPosition();
		if (pos.y <= 10.0f)
		{
			SetPosition(pos.x, 10.0f, pos.z);
			is_ground = true;
		}
	}

	// �ڽ� ��ü�� �ִٸ�, �ش� ��ü�� ������Ʈ
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);

}

BoundingOrientedBox Item::Get_Collider()
{
	BoundingOrientedBox xmBoundingBox;
	default_collider.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&xmBoundingBox.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmBoundingBox.Orientation)));

	return xmBoundingBox;
}

//===============================================================================

Item_Manager::Item_Manager()
{
	Snow_Area = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), Snow_Area_Radius);

	Boom_Area = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), Boom_Area_Radius);
}

Item_Manager::~Item_Manager()
{
	delete Double_Power_obj;
	delete Max_Power_obj;
	delete Fire_Shot_obj;

	for (Stone_Item_Info* item_info_ptr : Frozen_Time_obj)
		delete item_info_ptr;

	for (Stone_Item_Info* item_info_ptr : Ghost_obj)
		delete item_info_ptr;

	for (Stone_Item_Info* item_info_ptr : Taunt_obj)
		delete item_info_ptr;

}

void Item_Manager::Add_Stone_Item_Applied(StoneObject* stone)
{
	Stone_Item_Info* stone_info = new Stone_Item_Info{ stone, NULL, 0 };
	switch (stone->used_item)
	{
	case Item_Type::Taunt:
		Taunt_obj.push_back(stone_info);
		break;

	case Item_Type::Frozen_Time:
	{
		auto empty_space = std::find_if(Frozen_Time_obj.begin(), Frozen_Time_obj.end(), [](Stone_Item_Info* existing_stone)
			{return (existing_stone->stone == NULL && existing_stone->particle == NULL); });

		// �����̳ʿ� ��Ȱ��ȭ �� �� ������ �ִٸ� ��ü
		if (empty_space != Frozen_Time_obj.end())
		{
			// ������ �����Ͱ� ����Ű�� �޸𸮸� ����
			delete* empty_space;

			// �� �����ͷ� ��ü
			*empty_space = stone_info; 
		}
		else
			Frozen_Time_obj.push_back(stone_info);
	}
	break;

	case Item_Type::Ghost:
		{
			auto empty_space = std::find_if(Ghost_obj.begin(), Ghost_obj.end(), [](Stone_Item_Info* existing_stone)
				{return (existing_stone->stone == NULL && existing_stone->particle == NULL); });

			// �����̳ʿ� ��Ȱ��ȭ �� �� ������ �ִٸ� ��ü
			if (empty_space != Ghost_obj.end())
			{
				// ������ �����Ͱ� ����Ű�� �޸𸮸� ����
				delete* empty_space;

				// �� �����ͷ� ��ü
				*empty_space = stone_info;
			}
			else
				Ghost_obj.push_back(stone_info);
		}
		break;

	case Item_Type::Fire_Shot:
		Fire_Shot_obj = stone_info;
		break;
	case Item_Type::Double_Power:
		Double_Power_obj = stone_info;
		break;

	case Item_Type::Max_Power:
		Max_Power_obj = stone_info;
		break;

	default:
		break;
	}

}

Stone_Item_Info* Item_Manager::Get_Stone(Item_Type type)
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
}
std::vector<Stone_Item_Info*>* Item_Manager::Get_Stone_Iist(Item_Type type)
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

int Item_Manager::Get_Active_Stone_Num(Item_Type type)
{
	switch (type)
	{
	case Item_Type::Taunt:
		return std::count_if(Taunt_obj.begin(), Taunt_obj.end(), [](Stone_Item_Info* stone_info) {return (stone_info->stone != NULL); });
		break;

	case Item_Type::Frozen_Time:
		return std::count_if(Frozen_Time_obj.begin(), Frozen_Time_obj.end(), [](Stone_Item_Info* stone_info) {return (stone_info != NULL); });
		break;
		
	case Item_Type::Ghost:
		return std::count_if(Ghost_obj.begin(), Ghost_obj.end(), [](Stone_Item_Info* stone_info) {return (stone_info->stone != NULL); });
		break;

	default:
		break;
	}

	return 0;
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

void Item_Manager::Update_Frozen_Time(float fTimeElapsed)
{
	if (Frozen_Light == NULL)
		return;

	bool Exist_Frozen_Stone = std::any_of(Frozen_Time_obj.begin(), Frozen_Time_obj.end(), [](Stone_Item_Info* info) {return (info->stone != NULL && info->stone->active); });
	if (Exist_Frozen_Stone)
	{
		
		if (Frozen_Light->m_fRange < 300.0f)
		{
			Frozen_Light->m_bEnable = true;
			Frozen_Light->m_fRange += 4.0f;
			Frozen_Light->m_xmf4Ambient.x -= 0.25f;
			Frozen_Light->m_xmf4Diffuse.x -= 0.25f;
		}
	}
	else
	{
		if (Frozen_Light->m_fRange > 30.0f)
		{
			Frozen_Light->m_xmf4Ambient.x += 0.25f;
			Frozen_Light->m_xmf4Diffuse.x += 0.25f;
			Frozen_Light->m_fRange -= 4.0f;
		}
		else
		{		
			Frozen_Light->m_bEnable = false;
			Frozen_Light->m_xmf4Ambient = XMFLOAT4(-0.3f, 0.1f, 1.0f, 1.0f);
			Frozen_Light->m_xmf4Diffuse = XMFLOAT4(-0.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	for (Stone_Item_Info* info : Frozen_Time_obj)
	{
		// ���� ���� ������
		if (info->turn == 3)
		{
			delete info->particle;
			info->particle = NULL;
			info->stone = NULL;
		}
		else 
		{
			// ���� ���Ҵµ�, ����� ���� Ż���ߴٸ�
			if (info->stone == NULL || info->stone->active == false)
			{
				if (info->particle != NULL)
				{
					delete info->particle;
					info->particle = NULL;
					info->stone = NULL;
				}
			}
		}
		
		// ���� ���ǹ��� ����ߴٸ�
		if (info->particle != NULL)
			info->particle->Animate(fTimeElapsed);
	}

}
void Item_Manager::Update_Ghost(float fTimeElapsed)
{
	bool Exist_Ghost_Stone = std::any_of(Ghost_obj.begin(), Ghost_obj.end(), [](Stone_Item_Info* info) {return (info->stone != NULL && info->stone->active); });
	if (Exist_Ghost_Stone)
	{
		for (Stone_Item_Info* info : Ghost_obj)
		{
			// ���� ���� ������
			if (info->turn == 2)
			{
				info->stone->used_item = Item_Type::None;
				info->stone = NULL;
			}
			else
			{
				// ���� ���Ҵµ�, ����� ���� Ż���ߴٸ�
				if (info->stone == NULL || info->stone->active == false)
					info->stone = NULL;
			}
		}
	}



}
void Item_Manager::Update_Fire_Shot(float fTimeElapsed)
{
	if (Fire_Shot_obj != NULL)
		if(Fire_Shot_obj->particle!= NULL)
			Fire_Shot_obj->particle->Animate(fTimeElapsed);
}
void Item_Manager::Check_Stone_Fire_Shot_Effect(std::vector<StoneObject*>* stonelist)
{
	Stone_Item_Info* fire_stone_info = Get_Stone(Item_Type::Fire_Shot);

	std::vector<StoneObject*> stones_in_boom_area;

	if (fire_stone_info == NULL || 1 <= fire_stone_info->turn)
		return;

	//========================================================

	XMFLOAT3 boom_center = fire_stone_info->stone->GetPosition();

	// ���� ���� �̵�
	BoundingSphere moved_Boom_area = Boom_Area;
	moved_Boom_area.Center = boom_center;

	// ���� ���� �ȿ� �ִ� �� ��ü ����
	for (StoneObject* stone : *stonelist)
	{
		if (moved_Boom_area.Intersects(stone->stone_collider))
			stones_in_boom_area.push_back(stone);
	}

	//========================================================

	// ���� ȿ�� ����
	for (StoneObject* stone_ptr : stones_in_boom_area)
	{
		XMVECTOR stone_pos = XMLoadFloat3(&stone_ptr->GetPosition());
		XMVECTOR boom_center_vec = XMLoadFloat3(&boom_center);

		// ���� �߽����κ��� �������� ���� ���
		XMVECTOR direction = XMVector3Normalize(stone_pos - boom_center_vec);

		// ��� ���� ���
		float explosion_force = 100.0f;  // ���߷� ũ�� (����� ���� ����)
		XMVECTOR explosion_vec = explosion_force * direction;

		// ���� ���� �ӵ��� ���� ���� ����
		XMVECTOR stone_velocity = XMLoadFloat3(&stone_ptr->m_xmf3MovingDirection) * stone_ptr->m_fMovingSpeed;
		XMVECTOR new_velocity = stone_velocity + explosion_vec;

		// ���� �ӵ� �� ���� ����
		XMStoreFloat3(&stone_ptr->m_xmf3MovingDirection, XMVector3Normalize(new_velocity));
		stone_ptr->m_fMovingSpeed = XMVectorGetX(XMVector3Length(new_velocity));
	}

}
void Item_Manager::Check_Stone_Frozen_Time_Effect(std::vector<StoneObject*>* stonelist)
{
	std::vector<StoneObject*> stones_to_apply_snow;
	
	if (Get_Active_Stone_Num(Item_Type::Frozen_Time))
	{
		for (const Stone_Item_Info* frozen_info : Frozen_Time_obj)
		{
			if (frozen_info->particle != NULL)
			{
				XMFLOAT3 frozen_pos = frozen_info->particle->GetPosition();
				frozen_pos.y = 0.0f;

				// ��ġ ��ȭ�� ������ �ӽ� �浹ü ����
				BoundingSphere moved_Snow_Area = Snow_Area;
				moved_Snow_Area.Center = frozen_pos;

				for (StoneObject* stone : *stonelist)
				{
					if (moved_Snow_Area.Intersects(stone->stone_collider))
						stones_to_apply_snow.push_back(stone); // �����ص״ٰ� �������� ������ ��ü�� �ѹ��� �����ϱ�
				}
			}
		}

		std::transform(stonelist->begin(), stonelist->end(), stonelist->begin(), [](StoneObject* stone_ptr) {
			stone_ptr->SetFriction(2.0f);
			return stone_ptr; });
		for (StoneObject* froze_stone : stones_to_apply_snow)
			froze_stone->SetFriction(10.0f);
	}
}

void Item_Manager::Check_Stone_Item_Effect(std::vector<StoneObject*>* stonelist, Item_Type type)
{
	switch (type)
	{
	case Item_Type::Taunt:
		break;

	case Item_Type::Fire_Shot:
		Check_Stone_Fire_Shot_Effect(stonelist);
		break;

	case Item_Type::Double_Power:
		break;

	case Item_Type::Frozen_Time:
		Check_Stone_Frozen_Time_Effect(stonelist);
		break;

	case Item_Type::Max_Power:
		break;

	case Item_Type::Ghost:
		break;

	case Item_Type::ETC:
		break;

	case Item_Type::None:
		break;
	default:
		break;
	}

}

void Item_Manager::Animate(float fTimeElapsed)
{
	Update_Frozen_Time(fTimeElapsed);
	Update_Ghost(fTimeElapsed);
	Update_Fire_Shot(fTimeElapsed);
}

void Item_Manager::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (Stone_Item_Info* info : Frozen_Time_obj)
	{
		if (info->particle != NULL)
			info->particle->Particle_Render(pd3dCommandList, pCamera);
	}

	if(Fire_Shot_obj != NULL)
		if (Fire_Shot_obj->particle != NULL)
			Fire_Shot_obj->particle->Particle_Render(pd3dCommandList, pCamera);
	
}

