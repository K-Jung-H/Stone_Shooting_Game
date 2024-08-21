#include "stdafx.h"
#include "Scene.h"
#include "Player.h"

/*
추가 할  내용:
우클릭으로 선택한 돌에 윤곽선이 생겼으면 좋겠음

보드에서 우클릭으로 돌을 골라 해당 돌이 사용할 아이템을 미리 정하도록 하고, 우클릭은 시점 변화가 안 일어나야 함
			+
돌 시점에서, 특정 키를 누르면 현재 돌이 사용할 아이템 선택 할 수 있도록 하기

돌 객체에 현재 돌에 적용된 아이템을 변수로 저장하도록 할 것


셰이더마다 타입을 추가하여 구별하는 중

윤곽선 셰이더가 연결된 재질은 active가 변경되지 않도록 조건을 부분마다 추가하는 중

현재, 마우스로 선택한 돌의 색상이 바뀐 후,
다른 돌로 갈아타면, 이전 돌의 색상이 안바뀌는 중-> 원인 찾기!!!!!!!!!!

//=============================================

-------------------추가 희망 사항-------------

Scene에 player1로 빼놓은 객체를 최대한 활용하면 좋을 듯

특정 연산에서는 해당 객체에 저장된 돌들만 사용해서 연산 하도록.


*/
//=========================================================================================
CMaterial* CScene::material_color_white_stone = NULL;
CMaterial* CScene::material_color_black_stone = NULL;

CMaterial* CScene::material_color_player_selected = NULL;
CMaterial* CScene::material_color_com_selected = NULL;

CMaterial* CScene::material_color_white_particle = NULL;
CMaterial* CScene::material_color_black_particle = NULL;

CMaterial* CScene::material_color_board = NULL;
CMaterial* CScene::material_color_none = NULL; 


CScene::CScene()
{
}
CScene::~CScene()
{
}

void CScene::Build_Lights_and_Materials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(0.0f, 50.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[2].m_fRange = 30.0f;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.2, 0.2, 0.2, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(0.0f, 30.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Attenuation = XMFLOAT3(0.5f, 0.001f, 0.0001f);
	m_pLights->m_pLights[2].m_fFalloff = 0.5f;
	m_pLights->m_pLights[2].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[2].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 50.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(-0.3f, 0.1f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(-0.0f, 0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 1.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(0.0f, 5.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.005f);

	

	//=====================================================
	CMaterialColors white_stone_color = {
		XMFLOAT4(0.5f, 0.5, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0, 1.0f, 20.0f),
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f) 
	};

	CMaterialColors black_stone_color = {
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 20.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
	};

	CMaterialColors white_particle_color = {
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f)
	};

	CMaterialColors black_particle_color = {
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f),
		XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f)
	};

	CMaterialColors player_selected_color = {
		 XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f),
		 XMFLOAT4(0.1f, 0.3f, 0.1f, 1.0f),
		 XMFLOAT4(0.1f, 0.1f, 0.1f, 100.0f),
		 XMFLOAT4(0.0f, 0.2f, 0.0f, 1.0f)
	};

	CMaterialColors com_selected_color = {
		XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.3f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 100.0f),
		XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f)
	};

	CMaterialColors board_color = {
		XMFLOAT4(2.0f, 1.5f, 0.2f, 1.0f),
		XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f),
		XMFLOAT4(0.4f, 0.4f, 0.4f, 20.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f)
	};

	CMaterialColors none_color = {
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	};

	material_color_white_stone = new CMaterial(&white_stone_color);
	material_color_black_stone = new CMaterial(&black_stone_color);

	material_color_white_particle = new CMaterial(&white_particle_color);
	material_color_black_particle = new CMaterial(&black_particle_color);

	material_color_player_selected = new CMaterial(&player_selected_color);
	material_color_com_selected = new CMaterial(&com_selected_color);
	material_color_board = new CMaterial(&board_color);
	
	material_color_none = new CMaterial(&none_color);
}


void CScene::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CScene::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	Update_Lights(pd3dCommandList);
}

void CScene::Update_Lights(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 매핑한 주소에 조명 정보 전달(복사)
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));

	// 조명 리소스에 대한 상수 버퍼 뷰를 쉐이더 변수에 연결(바인딩) 
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress);
}

void CScene::Release_Shader_Resource()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Explosion_Particle::Prepare_Particle(pd3dDevice, pd3dCommandList);
	Charge_Particle::Prepare_Particle(pd3dDevice, pd3dCommandList);
	Firework_Particle::Prepare_Particle(pd3dDevice, pd3dCommandList);
	Snow_Particle::Prepare_Particle(pd3dDevice, pd3dCommandList);
	Item::Prepare_Item(pd3dDevice, pd3dCommandList);
	Build_Lights_and_Materials();

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	UI_GraphicsRootSignature = Create_UI_GraphicsRootSignature(pd3dDevice);

	Object_GraphicsRootSignature_ptr = m_pd3dGraphicsRootSignature;
	UI_GraphicsRootSignature_ptr = UI_GraphicsRootSignature;

	// 조명 및 재질 리소스 생성
	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
	
	BuildObjects(pd3dDevice, pd3dCommandList);
	BuildUIs(pd3dDevice, pd3dCommandList);


	// 미리 생성하여 준비
	item_manager = new Item_Manager();
	item_manager->Frozen_Light = &m_pLights->m_pLights[3];

	Charge_Effect = new Charge_Particle(pd3dDevice, pd3dCommandList, 50.0f, 5.0f, material_color_white_particle, Particle_Type::Charge);
	Charge_Effect->SetMaterial(material_color_none,true);
	Charge_Effect->AddMaterial(material_color_black_particle);
	Charge_Effect->active = true;
	Charge_Effect->m_ppMaterials[0].second = true;
	Charge_Effect->m_ppMaterials[1].second = true;
	Setting_Item(pd3dDevice, pd3dCommandList, XMFLOAT3(30.0f, 10.0f, 0.0f), Item_Type::Max_Power);
	Setting_Item(pd3dDevice, pd3dCommandList, XMFLOAT3(-30.0f, 10.0f, 0.0f), Item_Type::Frozen_Time);

	Snow_Effect = new Snow_Particle(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0f, 50.0f, 0.0f), 100.0f, material_color_white_stone, Particle_Type::Snow);

	//Setting_Particle(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0f, 50.0f, 0.0f), material_color_white_stone, Particle_Type::Snow);

}

void CScene::Create_Board(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float Board_Width, float Board_Depth)
{
	float Board_Half_Width = Board_Width / 2;
	float Board_Half_Depth = Board_Depth / 2;

	CPlaneMeshIlluminated* pboard_mesh = new CPlaneMeshIlluminated(pd3dDevice, pd3dCommandList, Board_Half_Width * 2.0f, Board_Half_Depth * 2.0f, 500);

	m_pBoards = new CBoardObject(pd3dDevice, pd3dCommandList);
	m_pBoards->SetPosition(0.0f, 0.0f, 0.0f);
	m_pBoards->SetMesh(pboard_mesh);
	m_pBoards->SetMaterial(material_color_none);
	m_pBoards->AddMaterial(material_color_board, true);
	m_pBoards->m_xmOOBB = m_pBoards->m_pMesh->m_xmBoundingBox; // 시작할 때 한번만 하면 됨


	//-------------------------------------------------------

	CGameObject* board_line = NULL;
	CPlaneMeshIlluminated* pboard_line_mesh = new CPlaneMeshIlluminated(pd3dDevice, pd3dCommandList, Board_Half_Width / 100.0f, Board_Half_Depth, 1);
	
	std::vector<XMFLOAT3> horizontal_line_pos;
	horizontal_line_pos.push_back(XMFLOAT3(-50.0f, 0.3f, 0.0f));
	horizontal_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, 0.0f));
	horizontal_line_pos.push_back(XMFLOAT3(50.0f, 0.3f, 0.0f));

	for (const XMFLOAT3& pos : horizontal_line_pos)
	{
		board_line = new CGameObject(pd3dDevice, pd3dCommandList);
		board_line->SetPosition(XMFLOAT3(pos));
		board_line->SetScale(1.0f, 1.0f, 2.0f);
		board_line->SetMesh(pboard_line_mesh);
		board_line->o_type = Object_Type::Board;
		board_line->SetMaterial(material_color_black_particle);

		m_pBoards->Add_Child(board_line);
	}

	std::vector<XMFLOAT3> vertical_line_pos;
	XMFLOAT3 vertical_angle = XMFLOAT3{ 0.0f, 1.0f, 0.0f };
	vertical_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, -100.0f));
	vertical_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, 0.0f));
	vertical_line_pos.push_back(XMFLOAT3(0.0f, 0.3f, 100.0f));

	for (const XMFLOAT3& pos : vertical_line_pos)
	{
		board_line = new CGameObject(pd3dDevice, pd3dCommandList);
		board_line->SetPosition(XMFLOAT3(pos));
		board_line->SetScale(2.0f/3.0f,1.0f,1.0f);
		board_line->Rotate(&vertical_angle, 90.0f);
		board_line->SetMesh(pboard_line_mesh);
		board_line->o_type = Object_Type::Board;
		board_line->SetMaterial(material_color_black_particle);

		m_pBoards->Add_Child(board_line);
	}

}

void CScene::Setting_Stone(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMesh* mesh, XMFLOAT3 pos, bool player_team)
{
	StoneObject* pStoneObject = NULL;

	pStoneObject = new StoneObject(pd3dDevice, pd3dCommandList);
	pStoneObject->SetMesh(mesh);
	pStoneObject->SetPosition(pos.x, pos.y, pos.z);
	pStoneObject->SetFriction(2);										// Default

	pStoneObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));			// Default
	pStoneObject->SetRotationSpeed(0.0f);								// Default

	pStoneObject->SetMaterial(material_color_none);
	if (player_team)
	{
		pStoneObject->AddMaterial(material_color_white_stone, true);
		pStoneObject->AddMaterial(material_color_player_selected, false);
		pStoneObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));	// Default
	}
	else
	{
		pStoneObject->AddMaterial(material_color_black_stone, true);
		pStoneObject->AddMaterial(material_color_com_selected, false);
		pStoneObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));	// Default
	}

	pStoneObject->SetMovingSpeed(0.0f);									// Default

	pStoneObject->player_team = player_team;
	if (player_team)
		player1.stone_list.push_back(pStoneObject);
	else
		computer.stone_list.push_back(pStoneObject);

	GameObject_Stone.push_back(pStoneObject);
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//===========================================================
	// 게임 객체
	Object_Shader = new CObjectsShader[N_Object_Shader];
	Object_Shader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	// 윤곽선
	Outline_Shader = new OutlineShader[N_Outline_Shader];
	Outline_Shader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	((OutlineShader*)&Outline_Shader[0])->Create_Outline_Buffer(pd3dDevice, pd3dCommandList);
	//===========================================================
	material_color_none->SetShader(&Outline_Shader[0]);

	Create_Board(pd3dDevice, pd3dCommandList, 200, 600);

	//===========================================================

	CSphereMeshIlluminated* StoneMesh = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, 8.0f, 20, 20, 0.5f);
	std::vector<XMFLOAT3> w_stone_pos_list;
	
	w_stone_pos_list.push_back({ -50, 5, 100 });
	w_stone_pos_list.push_back({ 0, 5, 100 });
	w_stone_pos_list.push_back({ 50, 5, 100 });
	w_stone_pos_list.push_back({ -25, 5, 150 });
	w_stone_pos_list.push_back({ 0, 5, 150 });
	w_stone_pos_list.push_back({ 25, 5, 150 });

	for(XMFLOAT3& w_stone_pos : w_stone_pos_list)
		Setting_Stone(pd3dDevice, pd3dCommandList, StoneMesh, w_stone_pos, true);

	//-------------------------------------------------------------------

	std::vector<XMFLOAT3> b_stone_pos_list;

	b_stone_pos_list.push_back({ -50, 5, -100 });
	b_stone_pos_list.push_back({ 0, 5, -100 });
	b_stone_pos_list.push_back({50, 5, -100});
	b_stone_pos_list.push_back({ -25, 5, -150 });
	b_stone_pos_list.push_back({ 0, 5, -150 });
	b_stone_pos_list.push_back({ 25, 5, -150 });

	for (XMFLOAT3& b_stone_pos : b_stone_pos_list)
		Setting_Stone(pd3dDevice, pd3dCommandList, StoneMesh, b_stone_pos, false);


	//===========================================================


}

void CScene::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// UI 객체 // UIShader 
	UI_Shader = new UIShader[N_UI_Shader];
	UI_Shader[0].CreateShader(pd3dDevice, UI_GraphicsRootSignature);

	//=======================================================================
	// UI Area
	D3D12_RECT power_ui_area_1 = { 600, 0, 800, 90 };

	D3D12_RECT power_ui_area_2 = { 0, 0, 200, 90 };
	
	//=======================================================================

	// UI Mesh
	CMesh* ui_power_mesh = new UIMesh(pd3dDevice, pd3dCommandList, 200.0f, 90.0f, 1.0f, XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f));
	
	CMesh* ui_endline_mesh = new UIMesh(pd3dDevice, pd3dCommandList, 5.0f, 90.0f, 1.0f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), false);

	//=======================================================================

	// UI's Object
	UI_Object* ui_power_bar = new BAR_UI_Object(pd3dDevice, pd3dCommandList, 4);
	ui_power_bar->SetMesh(ui_power_mesh);
	ui_power_bar->active = true;

	UI_Object* ui_endline =  new UI_Object(pd3dDevice, pd3dCommandList);
	ui_endline->SetMesh(ui_endline_mesh);
	ui_endline->SetPosition(-100.0f, 0.0f, 0.0f);
	ui_endline->active = true;

	//-----------------------------------

	UI_Object* ui_power_bar_com = new BAR_UI_Object(pd3dDevice, pd3dCommandList, 3);
	ui_power_bar_com->SetMesh(ui_power_mesh);
	ui_power_bar_com->active = true;

	UI_Object* ui_endline_com = new UI_Object(pd3dDevice, pd3dCommandList);
	ui_endline_com->SetMesh(ui_endline_mesh);
	ui_endline_com->SetPosition(100.0f, 0.0f, 0.0f);
	ui_endline_com->active = true;

	//=======================================================================

	// UI Manager
	UI* Power_UI_player = new BAR_UI(pd3dDevice, pd3dCommandList, power_ui_area_1);
	ui_player_power = Power_UI_player;
	
	((BAR_UI*)Power_UI_player)->Set_Bar_obj((BAR_UI_Object*)ui_power_bar);
	Power_UI_player->ui_object.push_back(ui_power_bar);
	Power_UI_player->ui_object.push_back(ui_endline);
	Power_UI_player->Active = true;
	UI_list.push_back(Power_UI_player);

	//-----------------------------------
	
	UI* Power_UI_com = new BAR_UI(pd3dDevice, pd3dCommandList, power_ui_area_2);
	ui_com_power = Power_UI_com;

	((BAR_UI*)Power_UI_com)->Set_Bar_obj((BAR_UI_Object*)ui_power_bar_com);
	Power_UI_com->ui_object.push_back(ui_power_bar_com);
	Power_UI_com->ui_object.push_back(ui_endline_com);
	Power_UI_com->Active = true;
	UI_list.push_back(Power_UI_com);

	//=======================================================================

	D3D12_RECT inventory_ui_area = { 0, 500, 800, 600 };

	Inventory_UI* Inventory_ui = Create_Inventory_UI(pd3dDevice, pd3dCommandList, inventory_ui_area); 
	player_inventory = Inventory_ui;

	UI_list.push_back(Inventory_ui);

	ui_num = UI_list.size();

}

Inventory_UI* CScene::Create_Inventory_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_RECT area)
{
	float inventory_width = area.right - area.left;
	float inventory_height = area.bottom - area.top;
	float start_pos_x = -(inventory_width / 2);

	if (inventory_width < 100.0f)
		inventory_width = 100;


	float icon_Box_size = inventory_width / (8 * 2); // 아이템 8개, 아이템 간 간격
	float icon_distance = icon_Box_size; // 아이템 아이콘별 간격


	// 인벤토리의 아이템 아이콘 크기 설정
	float min_width = 100;
	float max_width = 800;
	float min_scale = 0.5f; // 100일 때의 scale 값
	float max_scale = 3.0f; // 800일 때의 scale 값

	// 선형 보간 계산
	float icon_scale = (inventory_width - min_width) / (max_width - min_width) * (max_scale - min_scale) + min_scale;

	//------------------------------------------------------------------------------

	CMesh* ui_inventory_mesh = new UIMesh(pd3dDevice, pd3dCommandList, inventory_width, inventory_height, 1.0f, XMFLOAT4(1.0f, 0.3f, 0.8f, 1.0f), false); // 1.0f, 1.0f, 1.0f

	//------------------------------------------------------------------------------
	Inventory_UI* Inventory_ui = new Inventory_UI(pd3dDevice, pd3dCommandList, area);
	
	UI_Object* ui_inventory_obj = new UI_Object(pd3dDevice, pd3dCommandList);
	ui_inventory_obj->SetMesh(ui_inventory_mesh);
	ui_inventory_obj->SetPosition(0.0f, 0.0f, 30.0f);
	ui_inventory_obj->active = true;
	
	((Inventory_UI*)Inventory_ui)->Set_Inventory_board_obj(ui_inventory_obj);
	Inventory_ui->ui_object.push_back(ui_inventory_obj);

	//------------------------------------------------------------------------------

	std::vector <Item_Type> item_type_list
	{ 
		Item_Type::Taunt,
		Item_Type::Fire_Shot,
		Item_Type::Double_Power, 
		Item_Type::ETC,
		Item_Type::ETC,
		Item_Type::Frozen_Time,
		Item_Type::Ghost, 
		Item_Type::Max_Power
	};

	std::vector<std::pair<Item_Type, XMFLOAT3>> icon_info;

	for (int i = 0; i < item_type_list.size(); ++i)
		icon_info.push_back(std::make_pair(item_type_list[i], XMFLOAT3(start_pos_x + icon_Box_size * (i + 1) + (icon_distance * i), 10.0f, 0.0f)));




	for (std::pair < Item_Type, XMFLOAT3> info : icon_info)
	{
		Item* item = new Item(pd3dDevice, pd3dCommandList, info.first);

		item->SetActive(true);
		item->outer_frame->Set_MaterialShader(&Object_Shader[0], 0);
		item->outer_frame->SetMaterial(material_color_none,true);
		item->outer_frame->m_ppMaterials[1].second = true;

		item->inner_frame->Set_MaterialShader(&Object_Shader[0], 0);
		item->inner_frame->SetMaterial(material_color_none, true);
		item->inner_frame->m_ppMaterials[1].second = true;

		item->SetScale(icon_scale, icon_scale, icon_scale);
		item->SetPosition(info.second);
		ui_inventory_obj->Add_Child(item);

		Inventory_ui->item_list.push_back(std::make_pair(info.first, 0));
	}

	for (std::pair<Item_Type, XMFLOAT3> info : icon_info)
	{
		D2D1_RECT_F temp{ inventory_width / 2 + info.second.x - icon_Box_size, 550, inventory_width / 2 + info.second.x + icon_Box_size, 600 };
		Inventory_ui->text_area.push_back(temp);
	}

	Inventory_ui->Active = true;

	return Inventory_ui;
}
void CScene::ReleaseObjects()
{
	Release_Shader_Resource();

	if (m_pLights)
		delete m_pLights;

	//if (m_pMaterials)
	//	delete m_pMaterials;


	if (m_pd3dGraphicsRootSignature) 
		m_pd3dGraphicsRootSignature->Release();

	if (GameObject_Stone.size())
		GameObject_Stone.clear();

	if (Object_Shader)
		delete[] Object_Shader;

	/*for (int j = 0; j < m_n_uiShaders; ++j)
	{
		m_uiShaders[j].ReleaseShaderVariables();
		m_uiShaders[j].ReleaseObjects();
	}*/



	//if (m_uiShaders)
	//	delete[] m_uiShaders;
}

void CScene::ReleaseUploadBuffers()
{
	for (int j = 0; j < N_Object_Shader; ++j)
		Object_Shader[j].ReleaseUploadBuffers();
}


ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[6];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; //GameObject
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 5; // Outline
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].Constants.Num32BitValues = 8;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	
	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	
	if (pd3dSignatureBlob) 
		pd3dSignatureBlob->Release();
	
	if (pd3dErrorBlob) 
		pd3dErrorBlob->Release();
	
	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::Create_UI_GraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; // GameObject_pos
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; // UI_INFO
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;

	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob)
		pd3dSignatureBlob->Release();

	if (pd3dErrorBlob)
		pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}


void CScene::CreateGraphicsPipelineState(ID3D12Device* pd3dDevice)
{
	//정점 셰이더와 픽셀 셰이더를 생성한다.
	ID3DBlob* pd3dVertexShaderBlob = NULL;
	ID3DBlob* pd3dPixelShaderBlob = NULL;
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "VSMain", "vs_5_1", nCompileFlags, 0, &pd3dVertexShaderBlob, NULL);
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "PSMain", "ps_5_1", nCompileFlags, 0, &pd3dPixelShaderBlob, NULL);
	//래스터라이저 상태를 설정한다.
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	//블렌드 상태를 설정한다.
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//그래픽 파이프라인 상태를 설정한다.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS.pShaderBytecode = pd3dVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pd3dVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pd3dPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pd3dPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState = d3dRasterizerDesc;
	d3dPipelineStateDesc.BlendState = d3dBlendDesc;
	d3dPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3dPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3dPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	d3dPipelineStateDesc.InputLayout.NumElements = 0;
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.SampleDesc.Quality = 0;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}


void CScene::Defend_Overlap()
{
	int objs_N = GameObject_Stone.size();

	// 충돌 체크 및 overlaped 상태 설정
	for (int i = 0; i < objs_N; ++i)
	{
		auto* stone_1 = static_cast<StoneObject*>(GameObject_Stone[i]);
		if (!stone_1->active || stone_1->m_fMovingSpeed > 1.0f)
			continue;

		for (int j = i + 1; j < objs_N; ++j)
		{
			auto* stone_2 = static_cast<StoneObject*>(GameObject_Stone[j]);
			if (!stone_2->active || stone_2->m_fMovingSpeed > 1.0f)
				continue;

			if (stone_1->m_xmOOSP.Intersects(stone_2->m_xmOOSP) != DISJOINT)
			{
				// 객체를 overlaped 상태로 설정
				if (stone_1->Overlaped == NULL)
				{
					stone_1->Overlaped = stone_2;
					stone_2->Overlaped = stone_1;
				}
			}
		}
	}

	// 충돌 처리
	for (int i = 0; i < objs_N; ++i)
	{
		auto* stone_1 = static_cast<StoneObject*>(GameObject_Stone[i]);
		if (stone_1->Overlaped)
		{
			auto* stone_2 = stone_1->Overlaped;

			// 충돌 방향 계산
			XMFLOAT3 Diff_Pos = XMFLOAT3(
				stone_2->GetPosition().x - stone_1->GetPosition().x,
				stone_2->GetPosition().y - stone_1->GetPosition().y,
				stone_2->GetPosition().z - stone_1->GetPosition().z);
			XMVECTOR Diff_Vec = XMLoadFloat3(&Diff_Pos);
			Diff_Vec = XMVector3Normalize(Diff_Vec);

			// 새 이동 방향 설정
			XMVECTOR New_Speed1 = -Diff_Vec;
			XMVECTOR New_Speed2 = Diff_Vec;

			stone_1->m_xmf3MovingDirection = XMFLOAT3(XMVectorGetX(New_Speed1), XMVectorGetY(New_Speed1), XMVectorGetZ(New_Speed1));
			stone_2->m_xmf3MovingDirection = XMFLOAT3(XMVectorGetX(New_Speed2), XMVectorGetY(New_Speed2), XMVectorGetZ(New_Speed2));

			stone_1->m_fMovingSpeed = 5.0f; // 밀어내는 속도
			stone_2->m_fMovingSpeed = 5.0f; // 밀어내는 속도
		}
	}

	// overlaped 상태 정리
	for (int i = 0; i < objs_N; ++i)
	{
		auto* stone_1 = static_cast<StoneObject*>(GameObject_Stone[i]);
		if (stone_1->Overlaped && stone_1->m_xmOOSP.Intersects(stone_1->Overlaped->m_xmOOSP) == DISJOINT)
		{
			stone_1->Overlaped->Overlaped = NULL;
			stone_1->Overlaped = NULL;
		}
	}
}

void CScene::Remove_Unnecessary_Objects()
{
	// 자식 객체만 사라져야 하는 일이 발생하는 경우 오류 발생 중
	//auto unactive_stone_range = std::remove_if(GameObject_Stone.begin(), GameObject_Stone.end(), [](CGameObject* stone) {
	//	if (!stone->active)
	//	{
	//		delete stone;
	//		return true;
	//	}
	//	return false; });
	//GameObject_Stone.erase(unactive_stone_range, GameObject_Stone.end());
}



void CScene::Check_Stones_Collisions()
{
	// CShader 벡터의 참조를 임시 변수에 저장
	int objs_N = GameObject_Stone.size();

	// 충돌 객체 초기화
	for (CGameObject* stone_ptr : GameObject_Stone)
		stone_ptr->m_pObjectCollided = NULL;

	// 충돌 체크 및 충돌 객체 설정
	for (int i = 0; i < objs_N; ++i)
	{
		if (GameObject_Stone[i]->active)
		{
			for (int j = (i + 1); j < objs_N; ++j)
			{
				if (GameObject_Stone[j]->active)
				{
					if (GameObject_Stone[i]->m_xmOOSP.Intersects(GameObject_Stone[j]->m_xmOOSP))
					{
						GameObject_Stone[i]->m_pObjectCollided = GameObject_Stone[j];
						GameObject_Stone[j]->m_pObjectCollided = GameObject_Stone[i];
					}
				}
			}
		}
	}

	// 충돌 처리
	for (int i = 0; i < objs_N; i++)
	{
		if (GameObject_Stone[i]->m_pObjectCollided)
		{
			auto* bumped_stone1 = GameObject_Stone[i];
			auto* bumped_stone2 = bumped_stone1->m_pObjectCollided;

			XMFLOAT3 pos1 = bumped_stone1->GetPosition();
			XMFLOAT3 pos2 = bumped_stone2->GetPosition();

			XMVECTOR vel1 = XMLoadFloat3(&bumped_stone1->m_xmf3MovingDirection) * bumped_stone1->m_fMovingSpeed;
			XMVECTOR vel2 = XMLoadFloat3(&bumped_stone2->m_xmf3MovingDirection) * bumped_stone2->m_fMovingSpeed;

			// 충돌 방향 계산
			XMFLOAT3 Diff_Pos = XMFLOAT3(pos2.x - pos1.x, pos2.y - pos1.y, pos2.z - pos1.z);
			XMVECTOR Diff_Vec = XMLoadFloat3(&Diff_Pos);
			Diff_Vec = XMVector3Normalize(Diff_Vec); // 충돌 방향 정규화

			// 속도 차이 계산
			XMVECTOR vel_Diff_Vec = vel2 - vel1;

			// 충격량 계산
			float Power = XMVectorGetX(XMVector3Dot(vel_Diff_Vec, Diff_Vec));
			XMVECTOR Bump_Vec = Power * Diff_Vec;


			XMVECTOR New_Speed1 = vel1 + Bump_Vec;
			XMVECTOR New_Speed2 = vel2 - Bump_Vec;


			XMVECTOR Final_Vel1 = (New_Speed1 + vel2) / 2.0f;
			XMVECTOR Final_Vel2 = (New_Speed2 + vel1) / 2.0f;

			XMFLOAT3 Final_Vel1_f, Final_Vel2_f;
			XMStoreFloat3(&Final_Vel1_f, Final_Vel1);
			XMStoreFloat3(&Final_Vel2_f, Final_Vel2);

			// 속도 업뎃
			XMVECTOR Final_Vel1_v = XMLoadFloat3(&Final_Vel1_f);
			XMVECTOR Final_Vel2_v = XMLoadFloat3(&Final_Vel2_f);

			Final_Vel1_v = XMVector3Normalize(Final_Vel1_v);
			Final_Vel2_v = XMVector3Normalize(Final_Vel2_v);

			XMStoreFloat3(&bumped_stone1->m_xmf3MovingDirection, Final_Vel1_v);
			XMStoreFloat3(&bumped_stone2->m_xmf3MovingDirection, Final_Vel2_v);

			bumped_stone1->m_fMovingSpeed = sqrt(Final_Vel1_f.x * Final_Vel1_f.x + Final_Vel1_f.y * Final_Vel1_f.y + Final_Vel1_f.z * Final_Vel1_f.z);
			bumped_stone2->m_fMovingSpeed = sqrt(Final_Vel2_f.x * Final_Vel2_f.x + Final_Vel2_f.y * Final_Vel2_f.y + Final_Vel2_f.z * Final_Vel2_f.z);


			bumped_stone1->m_pObjectCollided = NULL;
			bumped_stone2->m_pObjectCollided = NULL;
		}
	}
}
void CScene::Check_Board_and_Stone_Collisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 충돌 객체 초기화
	for (CGameObject* stone_ptr : GameObject_Stone)
	{
		if (!stone_ptr->active)
			continue;
		ContainmentType containType = m_pBoards->m_xmOOBB.Contains(stone_ptr->m_xmOOSP);
		switch (containType)
		{
		case DISJOINT:
		{
			stone_ptr->SetMovingSpeed(0.0f);
			stone_ptr->GetPosition();

			if (stone_ptr->active == true)
			{
				if (stone_ptr->player_team)
					Setting_Particle(pd3dDevice, pd3dCommandList, stone_ptr->GetPosition(), material_color_white_stone, Particle_Type::Explosion);
				else if (!stone_ptr->player_team)
					Setting_Particle(pd3dDevice, pd3dCommandList, stone_ptr->GetPosition(), material_color_black_stone, Particle_Type::Explosion);
			}
			stone_ptr->active = false;
		}
		break;

		case INTERSECTS:
		case CONTAINS:
			break;
		}
	}
}


void CScene::Check_Item_and_Stone_Collisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (Item* item : Game_Items)
	{
		if (!item->active)
			continue;

		for (CGameObject* stone_ptr : player1.stone_list)
		{
			if (!stone_ptr->active)
				continue;

			if (stone_ptr->m_xmOOSP.Intersects(item->m_xmOOBB))
			{
				player1.Item_Inventory[item->item_type] += 1;
				Setting_Particle(pd3dDevice, pd3dCommandList, item->GetPosition(), material_color_black_particle, Particle_Type::Firework);
				item->SetActive(false);

				if (player_inventory != NULL)
				{
					for (std::pair<Item_Type, int> &info : player_inventory->item_list)
					{
						if (info.first == item->item_type)
							info.second += 1;					
					}
				}
			}
		}
	}
}


bool CScene::Change_Turn()
{
	if (item_manager->Get_Stone(Item_Type::Double_Power))
	{
		Player_Shot = false;
		if (item_manager->Double_Power_count < 1)
		{
			item_manager->Double_Power_count++;	
			m_pPlayer->SetPosition(player1.select_Stone->GetPosition());

			// 두번째 차례이므로, 다시 카메라 변경
			Set_MainCamera(m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.005f));
			return false;
		}
		else
		{
			item_manager->Double_Power_count = 0;
			item_manager->Set_Clear(Item_Type::Double_Power);
		}
	}


	if (Player_Turn)
	{
		Player_Turn = false;
		Com_Turn = true;
		Com_Shot = false;

		if (player1.select_Stone != NULL && player1.select_Stone->active)
			player1.select_Stone->ChangeMaterial(1);

		player1.select_Stone = NULL;
		player1.selected_Item_Type = Item_Type::None;
		Charge_Effect->ChangeMaterial(2);
	}
	else if (Com_Turn)
	{
		Player_Turn = true;
		Player_Shot = false;
		Com_Turn = false;
		
		if (computer.select_Stone->active)
		{
			computer.select_Stone->ChangeMaterial(1);
			computer.select_Stone = NULL;
			computer.target_Stone = NULL;
		
		}
		Charge_Effect->ChangeMaterial(1);
	}
	Mark_selected_stone();
	Charge_Effect->Reset();

	return true;
}

bool CScene::Check_Turn()
{
	if (!Player_Shot && Player_Turn)
		return false;

	if (!Com_Shot && Com_Turn)
		return false;

	int stop_stone_n = 0;

	for (const CGameObject* obj_ptr : GameObject_Stone)
	{
		const StoneObject* stone_ptr = static_cast<const StoneObject*>(obj_ptr);
		if (0.1f >= stone_ptr->m_fMovingSpeed || !stone_ptr->active)
			stop_stone_n += 1;
		else
			stone_ptr = stone_ptr;
	}

	if (stop_stone_n == GameObject_Stone.size())
		return true;
	else
		return false;
	
}

bool CScene::Check_GameOver()
{
	int dead_White_Stone = 0;
	int dead_Black_Stone = 0;
	for (CGameObject* obj_ptr : GameObject_Stone)
	{
		if (obj_ptr->active == false)
		{
			if (obj_ptr->player_team)
				dead_White_Stone += 1;
			else
				dead_Black_Stone += 1;
		}
	}

	if (dead_White_Stone == 6 || dead_Black_Stone == 6)
		return true;
	else
		return false;
}

void CScene::Setting_Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, Item_Type type)
{
	Item* item = NULL;

	switch (type)
	{
	case Item_Type::Double_Power:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Double_Power);
	break;

	case Item_Type::Ghost:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Ghost);
	break;

	case Item_Type::Taunt:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Taunt);
		break;

	case Item_Type::Fire_Shot:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Fire_Shot);
		break;

	case Item_Type::Frozen_Time:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Frozen_Time);
		break;

	case Item_Type::Max_Power:
		item = new Item(pd3dDevice, pd3dCommandList, Item_Type::Max_Power);
		break;

	case Item_Type::ETC:
	case Item_Type::None:
	default:
		break;
	}

	if (item != NULL)
	{
		item->SetActive(true);
		item->SetPosition(pos);
		item->outer_frame->Set_MaterialShader(&Object_Shader[0], 0);
		item->outer_frame->SetMaterial(material_color_none, true);
		item->outer_frame->m_ppMaterials[1].second = true;

		item->inner_frame->Set_MaterialShader(&Object_Shader[0], 0);
		item->inner_frame->SetMaterial(material_color_none, true);
		item->inner_frame->m_ppMaterials[1].second = true;

		Game_Items.push_back(item);
	}
}

void CScene::Setting_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, CMaterial* material, Particle_Type particle_type)
{
	bool Done = false;
	for (Particle* particle : m_particle)
	{
		if (particle->p_type == particle_type && particle->active == false)
		{
			particle->SetPosition(pos);
			particle->SetActive(true);
			particle->SetMaterial(material);
			Done = true;
			break;
		}
	}

	if (Done == false)
	{
		Particle* particle = NULL;
		switch (particle_type)
		{

		case Particle_Type::Explosion:
		{
			particle = new Explosion_Particle(pd3dDevice, pd3dCommandList, material, Particle_Type::Explosion);
			particle->SetActive(true);
			particle->SetPosition(pos);
			m_particle.push_back(particle);
		}
		break;

		case Particle_Type::Charge:
		{
			particle = new Charge_Particle(pd3dDevice, pd3dCommandList, 50.0f, 5.0f, material, Particle_Type::Charge);
			particle->SetActive(true);
			particle->SetPosition(pos);
			((Charge_Particle*)particle)->Set_Center_Position(pos);
			m_particle.push_back(particle);
		}
		break;

		case Particle_Type::Firework:
		{
			particle = new Firework_Particle(pd3dDevice, pd3dCommandList, 3.0f, material, Particle_Type::Firework);
			particle->SetActive(true);
			particle->SetPosition(pos);

			particle->SetMaterial(material_color_item_inner_red);
			particle->AddMaterial(material_color_item_outer);
			particle->AddMaterial(material_color_item_inner_blue);
			particle->AddMaterial(material_color_item_inner_green);

			m_particle.push_back(particle);
			
		}
		break;

		case Particle_Type::Snow:
			particle = new Snow_Particle(pd3dDevice, pd3dCommandList, pos, 250.0f, material, Particle_Type::Snow);
			particle->SetActive(true);
			particle->SetPosition(pos);
			((Snow_Particle*)particle)->Set_Center(pos);
			m_particle.push_back(particle);
			break;

		case Particle_Type::None:
		default:
			break;
		}
	}

}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (CGameObject* stone_obj : GameObject_Stone)
		stone_obj->Animate(fTimeElapsed, NULL);


	for (Item* item : Game_Items)
		item->Animate(fTimeElapsed, NULL);

	for (Particle* particle : m_particle)
		particle->Animate(fTimeElapsed);

	for (UI* ui_ptr : UI_list)
		ui_ptr->AnimateObjects(fTimeElapsed);

	//if (m_pBoards)
	//	m_pBoards->Animate(fTimeElapsed, NULL);

	if (Charge_Effect)
		Charge_Effect->Animate(fTimeElapsed);

	if (Snow_Effect->active)
		Snow_Effect->Animate(fTimeElapsed);

	if (m_pLights)
	{
		m_pLights->m_pLights[2].m_bEnable = true;
		m_pLights->m_pLights[2].m_xmf3Position.y = 30.0f;

		if (player1.select_Stone != NULL)
			m_pLights->m_pLights[2].m_xmf3Position = player1.select_Stone->GetPosition();
		else if (computer.select_Stone)
			m_pLights->m_pLights[2].m_xmf3Position = computer.select_Stone->GetPosition();
		else
			m_pLights->m_pLights[2].m_bEnable = false;

		//if (player1.select_Stone != NULL)
		//{
		//	Frozen_Light->m_xmf3Position = player1.select_Stone->GetPosition();

		//}
		//if (Com_Turn)
		//{
		//	Frozen_Light->m_fRange += 1;
		//	Frozen_Light->m_xmf4Ambient.x -= 0.1f;
		//	Frozen_Light->m_xmf4Diffuse.x -= 0.1f;
		//}
	}

}

void CScene::Scene_Update(float fTimeElapsed)
{
	if (Player_Turn) // 플레이어 턴
	{
		ui_player_power->Active = true;
		ui_com_power->Active = false;
		
		power_degree = ((BAR_UI*)ui_player_power)->Get_Degree();
		
		if (power_charge && player1.select_Stone != NULL)
		{
			Charge_Effect->active = true;
			Charge_Effect->Set_Center_Position(player1.select_Stone->GetPosition());
		}

	}
	else if (Com_Turn && Com_Shot == false) // 컴퓨터 턴
	{
		ui_player_power->Active = false;

		ui_com_power->Active = true;


		if (Game_Over == false) // 컴퓨터 차례라면
		{
			if (computer.select_Stone == NULL) // 돌을 선택 안했다면
			{
				//std::pair<StoneObject*, StoneObject*> hard_version = Find_Nearest_Enemy_Stone();

				std::pair<StoneObject*, StoneObject*> normal_version = Select_Stone_Com();
				if (normal_version.first == NULL || normal_version.second == NULL)
					return;

				computer.select_Stone = normal_version.first;
				computer.select_Stone->ChangeMaterial(2);

				computer.target_Stone = normal_version.second;

				// 차징 시간 결정
				power_charge = true;
				((BAR_UI*)ui_com_power)->Set_Bar_Charge_Mode(true);
				computer.random_time = 0.5f + (uid(dre) / 1200);
			}
			else
			{
				if (computer.random_time < computer.sum_time) // 정해진 시간이 됬다면 동작
				{
					computer.random_time = -1;
					computer.sum_time = 0;

					Shoot_Stone_Com(power_degree);
					Com_Shot = true;

					power_charge = false;
					((BAR_UI*)ui_com_power)->Set_Bar_Charge_Mode(false);
				}
				else // 시간이 될때까지 충전 애니메이션 및 UI 업데이트
				{
					power_degree = ((BAR_UI*)ui_com_power)->Get_Degree();
					
					if (power_charge && computer.select_Stone != NULL)
					{
						Charge_Effect->active = true;
						Charge_Effect->Set_Center_Position(computer.select_Stone->GetPosition());
					}

					computer.sum_time += fTimeElapsed;
				}

			}
		}
	}

	Remove_Unnecessary_Objects();
}

void CScene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// 커멘드 리스트에 PSO 연결, 루트 시그니처는 PSO에 이미 바인딩 되어 있음 == 따로 커멘드 리스트에서 바인딩 안해도 되지만, 안정성 문제
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	Object_Shader[0].Setting_PSO(pd3dCommandList); 

	// 카메라 영역 및 정보 업데이트
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->Update_Shader_Resource(pd3dCommandList);


	// 조명 업데이트
	Update_Shader_Resource(pd3dCommandList);
	
	m_pBoards->Render(pd3dCommandList, pCamera, &Object_Shader[0]);

	for (CGameObject* gameobject : GameObject_Stone)
	{
		gameobject->UpdateTransform(NULL);
		gameobject->Render(pd3dCommandList, pCamera, &Object_Shader[0]);
	}
	
	Item_Render(pd3dDevice, pd3dCommandList, pCamera);

	Particle_Render(pd3dDevice, pd3dCommandList, pCamera);

	UI_Render(pd3dDevice, pd3dCommandList);
}
void CScene::Particle_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{	
	// 커멘드 리스트에 PSO 연결, 루트 시그니처는 PSO에 이미 바인딩 되어 있음 == 따로 커멘드 리스트에서 바인딩 안해도 되지만, 안정성 문제
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	Object_Shader[0].Setting_PSO(pd3dCommandList);

	// 카메라 영역 및 정보 업데이트
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->Update_Shader_Resource(pd3dCommandList);


	if(power_charge)
		Charge_Effect->Particle_Render(pd3dCommandList, pCamera, &Object_Shader[0]);
	
	if(Snow_Effect->active)
		Snow_Effect->Particle_Render(pd3dCommandList, pCamera);


	for (Particle* particle : m_particle)
		particle->Particle_Render(pd3dCommandList, pCamera);
}
void CScene::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 커멘드 리스트에 PSO 연결, 루트 시그니처는 PSO에 이미 바인딩 되어 있음 == 따로 커멘드 리스트에서 바인딩 안해도 되지만, 안정성 문제
	pd3dCommandList->SetGraphicsRootSignature(UI_GraphicsRootSignature);
	UI_Shader[0].Setting_PSO(pd3dCommandList);


	for (UI* ui_ptr : UI_list)
		if (ui_ptr->Active)
			ui_ptr->UI_Render(pd3dDevice, pd3dCommandList, UI_Shader);

}
void CScene::Item_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// 커멘드 리스트에 PSO 연결, 루트 시그니처는 PSO에 이미 바인딩 되어 있음 == 따로 커멘드 리스트에서 바인딩 안해도 되지만, 안정성 문제
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	Object_Shader[0].Setting_PSO(pd3dCommandList);

	// 카메라 영역 및 정보 업데이트
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->Update_Shader_Resource(pd3dCommandList);


	for (Item* item : Game_Items)
	{
		item->UpdateTransform(NULL);
		item->Render(pd3dCommandList, pCamera, &Object_Shader[0]);
	}


}

CGameObject* CScene::Pick_Stone_Pointed_By_Cursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera) 
		return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();

	// 화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 
	// 그 결과는 카메라 좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	DebugOutput("x: " + std::to_string(xmf3PickPosition.x) + "\t y: " + std::to_string(xmf3PickPosition.y));

	int nIntersected = 0;

	float fHitDistance = FLT_MAX;
	float fNearestHitDistance = FLT_MAX;

	CGameObject* pIntersectedObject = NULL;
	CGameObject* pNearestObject = NULL;
	
	pIntersectedObject = Pick_Stone_By_RayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
	if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
	{
		fNearestHitDistance = fHitDistance;
		pNearestObject = pIntersectedObject;
	}

	return(pNearestObject);
}

CGameObject* CScene::Pick_Stone_By_RayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance)
{
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	CGameObject* pSelectedObject = NULL;
	for (CGameObject* obj_stone : GameObject_Stone) 
	{
		if (player1.select_Stone == obj_stone)
			continue;
		
		nIntersected = obj_stone->Pick_Object_By_Ray_Intersection_Projection(xmf3PickPosition,xmf4x4View, &fHitDistance); 
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = obj_stone;
		}
	}
	return(pSelectedObject);
}

bool CScene::is_Object_Selectable(CGameObject* now_picked)
{
	if (now_picked == player1.select_Stone || now_picked == NULL)
		return false;

	if (now_picked->player_team == false)
		return false;
	
	if (power_charge)
		return false;

	if (item_manager->Double_Power_count == 1)
		return false;

	return true;
}

bool CScene::is_Player_Turn()
{
	return (Player_Turn && Player_Shot == false);
}

void CScene::Shoot_Stone(float power)
{
	if (player1.select_Stone != NULL)
	{
		XMFLOAT3 direction = m_pPlayer->GetLookVector();
		player1.select_Stone->SetMovingDirection(direction);
		player1.select_Stone->SetMovingSpeed(power);

		Player_Shot = true;
	}
}

void CScene::Shoot_Stone_Com(float power)
{
	StoneObject* c_stone;
	StoneObject* p_stone;


	c_stone = computer.select_Stone;
	p_stone = computer.target_Stone;

	XMFLOAT3 c_position = c_stone->GetPosition();
	XMFLOAT3 p_position = p_stone->GetPosition();

	XMFLOAT3 direction;
	XMStoreFloat3(&direction, XMVectorSubtract(XMLoadFloat3(&p_position), XMLoadFloat3(&c_position)));
	XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&direction))); // 정규화까지 하기

	// 오차 각도 : -5 ~ 5
	float random_angle = 5 - uid(dre) / 300.0f;

	// 각도를 라디안으로 변환
	float angleInRadians = XMConvertToRadians(random_angle);

	// Y 축을 기준으로 회전하는 회전 행렬을 생성
	XMMATRIX rotationMatrix = XMMatrixRotationY(angleInRadians);

	// XMFLOAT3 벡터를 XMVECTOR로 변환
	XMVECTOR dir = XMLoadFloat3(&direction);

	// 회전 행렬을 적용하여 방향 벡터를 회전
	XMVECTOR rotatedDir = XMVector3TransformNormal(dir, rotationMatrix);

	// 회전된 벡터를 XMFLOAT3로 변환
	XMFLOAT3 newDirection;
	XMStoreFloat3(&newDirection, rotatedDir);


	// 돌 날리기
	c_stone->SetMovingDirection(newDirection);

	if (power < 100)
		power = 100;
	else if (power > 300)
		power = 500;

	c_stone->SetMovingSpeed(power);

}

std::pair<StoneObject*, StoneObject*> CScene::Select_Stone_Com()
{
	// 1. Find_Nearest_Enemy_Stone를 반복하여 쌍을 찾기 위한 벡터
	std::vector<std::pair<StoneObject*, StoneObject*>> pairs;
	std::vector<StoneObject*> Living_C_Stone;
	std::vector<StoneObject*> Living_Player_Stone;

	for (CGameObject* obj_ptr : GameObject_Stone)
	{
		if ((obj_ptr->active == true) && (obj_ptr->player_team == false))
		{
			StoneObject* com_stone = (StoneObject*)obj_ptr;
			Living_C_Stone.push_back(com_stone);
		}
		else if ((obj_ptr->active == true) && (obj_ptr->player_team == true))
		{
			StoneObject* player_stone = (StoneObject*)obj_ptr;
			Living_Player_Stone.push_back(player_stone);
		}
	}

	for (auto com_stone : Living_C_Stone)
	{
		for (auto player_stone : Living_Player_Stone)
		{
			pairs.push_back(std::make_pair(com_stone, player_stone));
		}
	}

	if (pairs.empty())
	{
		DebugOutput("No Pair to Shoot Target");
		::PostQuitMessage(0);
		return std::pair<StoneObject*, StoneObject*>{NULL, NULL};
	}

	// 3. 거리 순으로 정렬
	std::sort(pairs.begin(), pairs.end(), [](const auto& lhs, const auto& rhs) {
		float lhs_D = XMVectorGetX(XMVector3Length(XMLoadFloat3(&lhs.first->GetPosition()) - XMLoadFloat3(&lhs.second->GetPosition())));
		float rhs_D = XMVectorGetX(XMVector3Length(XMLoadFloat3(&rhs.first->GetPosition()) - XMLoadFloat3(&rhs.second->GetPosition())));
		return lhs_D < rhs_D;
		});

	// 4. 배열의 크기가 0~3이 가능한지 파악
	int maxIndex = std::min<int>(3, static_cast<int>(pairs.size()) - 1);

	if (maxIndex < 0)
	{
		DebugOutput("No Pair to Shoot Target");
		::PostQuitMessage(0);
		return std::pair<StoneObject*, StoneObject*>{NULL, NULL};
	}

	// 5. 0부터 maxIndex 사이의 랜덤 인덱스를 구하여 쌍을 선택
	int randomIndex = rand() % (maxIndex + 1);
	auto selected_pair = pairs[randomIndex];
	return selected_pair;

}

std::pair<StoneObject*, StoneObject*> CScene::Find_Nearest_Enemy_Stone()
{


	std::vector<StoneObject*>Living_C_Stone;
	std::vector< StoneObject*>Living_Player_Stone;
	for (CGameObject* obj_ptr : GameObject_Stone)
	{
		if ((obj_ptr->active == true) && (obj_ptr->player_team == false))
		{
			StoneObject* com_stone = (StoneObject*)obj_ptr;
			Living_C_Stone.push_back(com_stone);
		}
		else if ((obj_ptr->active == true) && (obj_ptr->player_team == true))
		{
			StoneObject* player_stone = (StoneObject*)obj_ptr;
			Living_Player_Stone.push_back(player_stone);
		}
	}

	float Closest_Distance = 100000; // 대충 거리 최대 10만

	StoneObject* closest_com_stone = NULL;
	StoneObject* closest_player_stone = NULL;

	// 컴퓨터 돌과 플레이어 돌 간의 가장 가까운 거리를 찾기
	for (auto com_stone : Living_C_Stone)
	{
		for (auto player_stone : Living_Player_Stone)
		{
			float distance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&com_stone->GetPosition()) - XMLoadFloat3(&player_stone->GetPosition())));


			if (distance < Closest_Distance)
			{
				Closest_Distance = distance;
				closest_com_stone = com_stone;
				closest_player_stone = player_stone;
			}
		}
	}
	return std::make_pair(closest_com_stone, closest_player_stone);
}



void CScene::Mark_selected_stone()
{
	for (StoneObject* stone_obj : player1.stone_list)
	{
		if (stone_obj->active)
		{
			if (stone_obj == player1.select_Stone)
			{
				stone_obj->ChangeMaterial(2);
				stone_obj->Apply_Item(player1.selected_Item_Type);
			}
			else
			{
				stone_obj->ChangeMaterial(1);
				stone_obj->Apply_Item(Item_Type::None);
			}
		}
	}
}

void CScene::Select_Item(Item_Type i_type)
{
	if (i_type != Item_Type::None)
	{
		if (player1.Item_Inventory[i_type]) // != 0
		{
			player1.Item_Inventory[i_type] -= 1;
		}
	}
}

CGameObject* CScene::Pick_Item_Pointed_By_Cursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera)
		return(NULL);

	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();

	// 화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 
	// 그 결과는 카메라 좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) * 8;
	xmf3PickPosition.y = -(((2.0f * (yClient - 500)) / d3dViewport.Height) - 1);
	xmf3PickPosition.z = 0.0f; 

	DebugOutput("x: " + std::to_string(xmf3PickPosition.x) + "\t y: " + std::to_string(xmf3PickPosition.y));

	int nIntersected = 0;
	float fHitDistance = FLT_MAX;
	float fNearestHitDistance = FLT_MAX;

	CGameObject* pIntersectedObject = NULL;
	CGameObject* pNearestObject = NULL;

	pIntersectedObject = Pick_Item_By_RayIntersection(xmf3PickPosition, pCamera, &fHitDistance);

	if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
	{
		fNearestHitDistance = fHitDistance;
		pNearestObject = pIntersectedObject;
	}

	return(pNearestObject);
}

CGameObject* CScene::Pick_Item_By_RayIntersection(XMFLOAT3& xmf3PickPosition, CCamera* pCamera, float* pfNearHitDistance)
{
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();

	int N = 1;
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	CGameObject* pSelected_item = NULL;

	for (CGameObject* item_obj : player_inventory->Get_Inventory_board_obj()->m_pChild)
	{
		//DebugOutput("Item info : " + std::to_string(N));
		nIntersected = item_obj->Pick_Object_By_Ray_Intersection_Orthographic(xmf3PickPosition, xmf4x4View, xmf4x4Projection, &fHitDistance);

		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			DebugOutput("-------------------------------");
			DebugOutput("Picked Item: " + std::to_string(N));
			DebugOutput("-------------------------------");
			*pfNearHitDistance = fHitDistance;
			pSelected_item = item_obj;
		}
		N++;
	}
	return(pSelected_item);
}


bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//player1.inventory_open
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (player1.inventory_open && !power_charge)
		{
			int clientX = LOWORD(lParam);
			int clientY = HIWORD(lParam);

			const int xMin = 0;
			const int xMax = 800;
			const int yMin = 500;
			const int yMax = 600;

			// 좌표가 영역 내에 있는지 확인하는 조건문
			if (clientX >= xMin && clientX <= xMax && clientY >= yMin && clientY <= yMax) {

				//마우스 위치를 기반으로 레이케스팅하여 아이템 선택
				CGameObject* selected_item = Pick_Item_Pointed_By_Cursor(clientX, clientY, player_inventory);

				if (selected_item != NULL)
					player1.selected_Item_Type = ((Item*)selected_item)->item_type;
			}
		}

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		Mark_selected_stone();
		Charge_Effect->Apply_Item(player1.selected_Item_Type);
		break;

	default:
		break;
	}
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_SPACE:
			if (is_Player_Turn()) {
				power_charge = true;
				((BAR_UI*)ui_player_power)->Set_Bar_Charge_Mode(true);
			}
			break;

		case VK_UP:
			Snow_Effect->active = !Snow_Effect->active;

		default:
			break;
		}
	}
	break;


	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SPACE:
			if (is_Player_Turn()) {
				power_charge = false;
				((BAR_UI*)ui_player_power)->Set_Bar_Charge_Mode(false);
				
				Shoot_Stone(power_degree);

				item_manager->Add_Stone_Item_Applied(player1.select_Stone);
				Set_MainCamera(m_pPlayer->ChangeCamera(TOP_VIEW_CAMERA, 0.005f));

				Charge_Effect->Reset();
				ui_player_power->Reset();
				ui_com_power->Reset();
			}
			break;

		case VK_TAB:
			if (player1.select_Stone)
			{
				Camera_First_Person_View = !Camera_First_Person_View;
				if (Camera_First_Person_View)
				{
					if (m_pPlayer)
					{
						Set_MainCamera(m_pPlayer->ChangeCamera(STONE_CAMERA, 0.05f));
					}
				}
				else
				{
					if (m_pPlayer)
					{
						Set_MainCamera(m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.05f));
					}
				}
			}
			break;


		case VK_OEM_3:
		{
			player1.inventory_open = !player1.inventory_open;
			player_inventory->Set_Visualize(player1.inventory_open);
		}
			break;

		case VK_F9:
			break;

		case VK_ESCAPE:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return(false);
}

bool ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}