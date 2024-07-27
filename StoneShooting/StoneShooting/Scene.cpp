#include "stdafx.h"
#include "Scene.h"
#include "Player.h"
#include <vector>

//=========================================================================================
CMaterialColors* CScene::material_color_white_stone = NULL;
CMaterialColors* CScene::material_color_black_stone = NULL;

CMaterialColors* CScene::material_color_white_particle = NULL;
CMaterialColors* CScene::material_color_black_particle = NULL;

CMaterialColors* CScene::material_color_board = NULL;


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
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(0.0f, 10.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights->m_pLights[1].m_bEnable = false;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 50.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(0.0f, 20.0f, .0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights->m_pLights[2].m_bEnable = false;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -0.5f, 0.1f);

	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 50.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.01f, 0.01f, 0.01f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(0.0f, 30.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(0.5f, 0.001f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 0.5f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	//=====================================================

	material_color_white_stone = new CMaterialColors
	{
		XMFLOAT4(0.5f, 0.5, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0, 1.0f, 20.0f),
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f)
	};

	material_color_black_stone = new CMaterialColors
	{
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), 
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), 
		XMFLOAT4(0.1f, 0.1f, 0.1f, 20.0f), 
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
	};

	material_color_white_particle = new CMaterialColors
	{
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),  
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),  
		XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),  
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f)
	};

	material_color_black_particle = new CMaterialColors
	{
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f),
		XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f)
	};

	material_color_board = new CMaterialColors
	{
		XMFLOAT4(2.0f, 1.5f, 0.2f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 20.0f),
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f)
	};

	//=====================================================


	//m_pMaterials = new MATERIALS;
	//::ZeroMemory(m_pMaterials, sizeof(MATERIALS));
	//m_pMaterials->m_pReflections[0] = { XMFLOAT4(0.5f, 0.5, 0.5f, 1.0f),  XMFLOAT4(1.0f, 1.0, 1.0f, 1.0f),  XMFLOAT4(1.0f, 1.0, 1.0f, 20.0f),  XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f) }; // 백돌
	//m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) }; // 흑돌
	//m_pMaterials->m_pReflections[2] = { XMFLOAT4(2.0f, 1.5f, 0.2f, 1.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 20.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f) }; // 게임 판
	//m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), XMFLOAT4(0.1f, 0.3f, 0.1f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 100.0f), XMFLOAT4(0.0f, 0.2f, 0.0f, 1.0f) }; // 플레이어 선택 표시
	//m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.3f, 0.1f, 0.1f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 100.0f), XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f) }; // 컴퓨터 선택 표시
	//m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),  XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),  XMFLOAT4(0.0f, 0.0, 0.0f, 1.0f),  XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f) }; // 백돌 파티클
	//m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f) };
	//m_pMaterials->m_pReflections[7] = { XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
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
	Build_Lights_and_Materials();
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	// 조명 및 재질 리소스 생성
	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
	
	BuildObjects(pd3dDevice, pd3dCommandList);
	BuildUIs(pd3dDevice, pd3dCommandList);
}

void CScene::Setting_Stone(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMesh* mesh, XMFLOAT3 pos, bool player_team)
{
	StoneObject* pStoneObject = NULL;
	CMaterial* material = new CMaterial;

	pStoneObject = new StoneObject(pd3dDevice, pd3dCommandList);
	pStoneObject->SetMesh(mesh);
	//~~~~~~~~~~~~~~
	pStoneObject->SetPosition(pos.x, pos.y, pos.z);
	pStoneObject->SetFriction(2);										// Default

	pStoneObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));			// Default
	pStoneObject->SetRotationSpeed(0.0f);								// Default

	if (player_team)
	{
		material->SetMaterialColors(material_color_white_stone);

		pStoneObject->SetMaterial(material);
		pStoneObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));	// Default
	}
	else
	{
		material->SetMaterialColors(material_color_black_stone);

		pStoneObject->SetMaterial(material);
		pStoneObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));	// Default
	}
	pStoneObject->SetMovingSpeed(0.0f);									// Default

	pStoneObject->player_team = player_team;

	Scene_GameObjects.push_back(pStoneObject);
	// Object_Shader[0].AddObjects(pStoneObject);
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//===========================================================
	// 게임 객체
	Object_Shader = new CObjectsShader[N_Object_Shader];
	Object_Shader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	Object_Shader[0].Create_Shader_Resource(pd3dDevice, pd3dCommandList);
	//===========================================================

	float Board_Half_Width = 100.0f;
	float Board_Half_Depth = 300.0f;
	CPlaneMeshIlluminated* pboard = new CPlaneMeshIlluminated(pd3dDevice, pd3dCommandList, Board_Half_Width * 2.0f, Board_Half_Depth * 2.0f);
	CMaterial* material = new CMaterial;

	m_pBoards = new CBoardObject(pd3dDevice, pd3dCommandList);
	m_pBoards->SetPosition(0.0f, 0.0f, 0.0f);
	m_pBoards->SetMesh(pboard);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~
	material->SetMaterialColors(material_color_board);
	//material->SetShader(&Object_Shader[0]);
	m_pBoards->SetMaterial(material);

	m_pBoards->m_xmOOBB = m_pBoards->m_pMesh->m_xmBoundingBox; // 시작할 때 한번만 하면 됨

	//===========================================================
	CSphereMeshIlluminated* StoneMesh = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, 6.0f, 20, 20);
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
	Charge_Effect = new Charge_Particle(pd3dDevice, pd3dCommandList, 50.0f, 5.0f, UINT(0), ParticleType::Charge);
}

void CScene::BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
//	// UI 객체 // UIShader 
//
//	// UI Area
//	D3D12_RECT power_ui_area_1 = { 600, 0, 800, 90 };
//	D3D12_RECT power_endline_ui_area_1 = { 595, 0, 600, 90 };
//
//	D3D12_RECT power_ui_area_2 = { 0, 0, 200, 90 };
//	D3D12_RECT power_endline_ui_area_2 = { 200, 0, 205, 90 };
//
//	//-----------------------------------
//
//	// UI Mesh
//	CMesh* ui_power_mesh = new UIMesh(pd3dDevice, pd3dCommandList, 200.0f, 90.0f, 1.0f, XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f));
//	
//	CMesh* ui_endline_mesh = new UIMesh(pd3dDevice, pd3dCommandList, 5.0f, 90.0f, 1.0f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), false);
//
//	//-----------------------------------
//
//	// UI's Object
//	CGameObject* ui_power_bar = NULL;
//	ui_power_bar = new CGameObject();
//	ui_power_bar->SetMesh(ui_power_mesh);
//	ui_power_bar->Create_Shader_Resource(pd3dDevice, pd3dCommandList);
//	ui_power_bar->SetMaterial(UINT(1));
//
//	CGameObject* ui_endline = NULL;
//	ui_endline = new CGameObject();
//	ui_endline->SetMesh(ui_endline_mesh);
//	ui_endline->Create_Shader_Resource(pd3dDevice, pd3dCommandList);
//	ui_endline->SetMaterial(UINT(1));
//
//	//=======================================================================
//
//	// UI Object
//	UI* Power_UI_player = new BAR_UI(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, power_ui_area_1);
//	ui_player_power = Power_UI_player;
//	
//	Power_UI_player->m_uiShaders[0].AddObjects(ui_power_bar);
//	pUI_list.push_back(Power_UI_player);
//
//	//-----------------------------------
//
//	UI* Power_endline_player = new UI(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, power_endline_ui_area_1);
//	ui_player_power_endline = Power_endline_player;
//
//	Power_endline_player->m_uiShaders[0].AddObjects(ui_endline);
//	pUI_list.push_back(Power_endline_player);
//
//	//=======================================================================
//
//	UI* Power_UI_com = new BAR_UI(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, power_ui_area_2, false);
//	ui_com_power = Power_UI_com;
//
//	Power_UI_com->m_uiShaders[0].AddObjects(ui_power_bar);
//	pUI_list.push_back(Power_UI_com);
//
//	//-----------------------------------
//
//	UI* Power_endline_com = new UI(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, power_endline_ui_area_2);
//	ui_com_power_endline = Power_endline_com;
//
//	Power_endline_com->m_uiShaders[0].AddObjects(ui_endline);
//	pUI_list.push_back(Power_endline_com);
//
//	ui_num = pUI_list.size();
//
//	//=======================================================================
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


	for (int j = 0; j < N_Object_Shader; ++j)
	{
		Object_Shader[j].Release_Shader_Resource();
		Object_Shader[j].ReleaseObjects();
	}

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
	D3D12_ROOT_PARAMETER pd3dRootParameters[5];

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

void CScene::CheckObjectByObjectCollisions()
{
	// CShader 벡터의 참조를 임시 변수에 저장
	auto& objshader_ptrs = Object_Shader[0].GetObjects();
	int objs_N = objshader_ptrs.size();

	// 충돌 객체 초기화
	for (CGameObject* obj_ptr : objshader_ptrs)
		obj_ptr->m_pObjectCollided = NULL;

	// 충돌 체크 및 충돌 객체 설정
	for (int i = 0; i < objs_N; ++i)
	{
		if (objshader_ptrs[i]->active)
		{
			for (int j = (i + 1); j < objs_N; ++j)
			{
				if (objshader_ptrs[j]->active)
				{
					if (objshader_ptrs[i]->m_xmOOSP.Intersects(objshader_ptrs[j]->m_xmOOSP))
					{
						objshader_ptrs[i]->m_pObjectCollided = objshader_ptrs[j];
						objshader_ptrs[j]->m_pObjectCollided = objshader_ptrs[i];
					}
				}
			}
		}
	}

	// 충돌 처리
	for (int i = 0; i < objs_N; i++)
	{
		if (objshader_ptrs[i]->m_pObjectCollided)
		{
			auto* bumped_stone1 = objshader_ptrs[i];
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

void CScene::Defend_Overlap()
{
	auto& objshader_ptrs = Object_Shader[0].GetObjects();
	int objs_N = objshader_ptrs.size();

	// 충돌 체크 및 overlaped 상태 설정
	for (int i = 0; i < objs_N; ++i)
	{
		auto* stone_1 = static_cast<StoneObject*>(objshader_ptrs[i]);
		if (!stone_1->active || stone_1->m_fMovingSpeed > 1.0f)
			continue;

		for (int j = i + 1; j < objs_N; ++j)
		{
			auto* stone_2 = static_cast<StoneObject*>(objshader_ptrs[j]);
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
		auto* stone_1 = static_cast<StoneObject*>(objshader_ptrs[i]);
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
		auto* stone_1 = static_cast<StoneObject*>(objshader_ptrs[i]);
		if (stone_1->Overlaped && stone_1->m_xmOOSP.Intersects(stone_1->Overlaped->m_xmOOSP) == DISJOINT)
		{
			stone_1->Overlaped->Overlaped = NULL;
			stone_1->Overlaped = NULL;
		}
	}
}

void CScene::CheckObject_Out_Board_Collisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 충돌 객체 초기화
	for (CGameObject* obj_ptr : Object_Shader[0].GetObjects())
	{
		if (!obj_ptr->active)
			continue;
		ContainmentType containType = m_pBoards->m_xmOOBB.Contains(obj_ptr->m_xmOOSP);
		switch (containType)
		{
		case DISJOINT:
		{
			obj_ptr->SetMovingSpeed(0.0f);
			obj_ptr->GetPosition();

			if (obj_ptr->active == true)
			{
				if (obj_ptr->player_team)
					Setting_Particle(pd3dDevice, pd3dCommandList, obj_ptr->GetPosition(), UINT(5), ParticleType::Explosion);
				else if (!obj_ptr->player_team)
					Setting_Particle(pd3dDevice, pd3dCommandList, obj_ptr->GetPosition(), UINT(6), ParticleType::Explosion);
			}
			obj_ptr->active = false;
		}
		break;

		case INTERSECTS:
		case CONTAINS:
			break;
		}
	}
}

void CScene::Change_Turn()
{
	if (Player_Turn)
	{
		Player_Turn = false;
		Com_Turn = true;
		Com_Shot = false;

		//if (m_pSelectedObject->active)
		//	m_pSelectedObject->Set_Material_and_Shader(material_white_stone, &Object_Shader[0]);

		//m_pSelectedObject = NULL;
		//Charge_Effect->Set_Material_and_Shader(material_white_stone, &Object_Shader[0]);
	}
	else if (Com_Turn)
	{
		Player_Turn = true;
		Player_Shot = false;
		Com_Turn = false;
		
		//if (computer.select_Stone->active)
		//{
		//	computer.select_Stone->Set_Material_and_Shader(material_black_stone, &Object_Shader[0]);
		//	computer.select_Stone = NULL;
		//	computer.target_Stone = NULL;
		//
		//}
		//Charge_Effect->Set_Material_and_Shader(material_black_stone, &Object_Shader[0]);
	}

	Charge_Effect->Reset();
}

bool CScene::Check_Turn()
{
	if (!Player_Shot && Player_Turn)
		return false;

	if (!Com_Shot && Com_Turn)
		return false;

	int stop_stone_n = 0;

	for (const CGameObject* obj_ptr : Object_Shader[0].GetObjects())
	{
		const StoneObject* stone_ptr = static_cast<const StoneObject*>(obj_ptr);
		if (0.1f >= stone_ptr->m_fMovingSpeed || !stone_ptr->active)
			stop_stone_n += 1;
		else
			stone_ptr = stone_ptr;
	}

	if (stop_stone_n == Object_Shader[0].GetObjects().size())
		return true;
	else
		return false;
	
}

bool CScene::Check_GameOver()
{
	int dead_White_Stone = 0;
	int dead_Black_Stone = 0;
	for (CGameObject* obj_ptr : Object_Shader[0].GetObjects())
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

void CScene::Setting_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, UINT material, ParticleType type)
{
//	bool Done = false;
//	for (Particle* particle : m_particle)
//	{
//		if (particle->type == type && particle->active == false)
//		{
//			particle->SetPosition(pos);
//			particle->SetActive(true);
//			particle->SetMaterial(material);
//			Done = true;
//			break;
//		}
//	}
//
//	if (Done == false)
//	{
//		Particle* particle = NULL;
//		switch (type)
//		{
//
//		case ParticleType::Explosion:
//		{
//			particle = new Explosion_Particle(pd3dDevice, pd3dCommandList, UINT(material), ParticleType::Explosion);
//			particle->SetActive(true);
//			particle->SetPosition(pos);
//			m_particle.push_back(particle);
//		}
//		break;
//
//		case ParticleType::Charge:
//		{
//			particle = new Charge_Particle(pd3dDevice, pd3dCommandList, 50.0f, 5.0f, UINT(material), ParticleType::Charge);
//			particle->SetActive(true);
//			particle->SetPosition(pos);
//			((Charge_Particle*)particle)->Set_Center_Position(pos);
//			m_particle.push_back(particle);
//		}
//		case ParticleType::None:
//		default:
//			break;
//		}
//	}
//
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	Object_Shader[0].AnimateObjects(fTimeElapsed);

	for (UI* ui_ptr : pUI_list)
		ui_ptr->AnimateObjects(fTimeElapsed);

	for (Particle* particle : m_particle)
		particle->Animate(fTimeElapsed);

	if (Charge_Effect)
	{
		Charge_Effect->Animate(fTimeElapsed);
	}

	if (m_pLights)
	{
		if (m_pSelectedObject)
		{
			m_pLights->m_pLights[3].m_bEnable = true;
			m_pLights->m_pLights[3].m_xmf3Position = m_pSelectedObject->GetPosition();
			m_pLights->m_pLights[3].m_xmf3Position.y = 30.0f;
		}
		else if (computer.select_Stone)
		{
			m_pLights->m_pLights[3].m_bEnable = true;
			m_pLights->m_pLights[3].m_xmf3Position = computer.select_Stone->GetPosition();
			m_pLights->m_pLights[3].m_xmf3Position.y = 30.0f;
		}
		else
			m_pLights->m_pLights[3].m_bEnable = false;
	}

}

void CScene::Scene_Update(float fTimeElapsed)
{
	if (Player_Turn) // 플레이어 턴
	{
		ui_player_power->Active = true;
		ui_player_power_endline->Active = true;

		ui_com_power->Active = false;
		ui_com_power_endline->Active = false;

		power_degree = ui_player_power->Update(fTimeElapsed, power_charge);
		
		if (power_charge && m_pSelectedObject != NULL)
		{
			Charge_Effect->active = true;
			Charge_Effect->Set_Center_Position(m_pSelectedObject->GetPosition());
		}

	}
	else if (Com_Turn && Com_Shot == false) // 컴퓨터 턴
	{
		ui_player_power->Active = false;
		ui_player_power_endline->Active = false;

		ui_com_power->Active = true;
		ui_com_power_endline->Active = true;

		if (Game_Over == false) // 컴퓨터 차례라면
		{
			if (computer.select_Stone == NULL) // 돌을 선택 안했다면
			{
				//std::pair<StoneObject*, StoneObject*> hard_version = Find_Nearest_Enemy_Stone();

				std::pair<StoneObject*, StoneObject*> normal_version = Select_Stone_Com();
				if (normal_version.first == NULL || normal_version.second == NULL)
					return;

				computer.select_Stone = normal_version.first;
				//computer.select_Stone->Set_Material_and_Shader(material_black_stone, &Object_Shader[0]);

				computer.target_Stone = normal_version.second;

				// 차징 시간 결정
				power_charge = true;
				computer.random_time = 1.0f + (uid(dre) / 1000);
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
				}
				else // 시간이 될때까지 충전 애니메이션 및 UI 업데이트
				{
					power_degree = ui_com_power->Update(fTimeElapsed, power_charge);

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
}

void CScene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->Update_Shader_Resource(pd3dCommandList);

	Object_Shader[0].Setting_PSO(pd3dCommandList);

	// 조명 업데이트
	Update_Shader_Resource(pd3dCommandList);
	
//	Scene_GameObjects[0]->Render(pd3dCommandList, pCamera, &Object_Shader[0]);

	m_pBoards->Render(pd3dCommandList, pCamera, &Object_Shader[0]);

	for (CGameObject* gameobject : Scene_GameObjects)
		gameobject->Render(pd3dCommandList, pCamera, &Object_Shader[0]);

	//for (int i = 0; i < N_Object_Shader; ++i)
	//{
	//	Object_Shader[i].Render(pd3dCommandList, pCamera);
	//}

	// 보드는 Scene에서 관리 중
	// 아직은 움직임이 없을꺼니까

	//Particle_Render(pd3dDevice, pd3dCommandList, pCamera);

	//UI_Render(pd3dDevice, pd3dCommandList);

	//=======================플레이어 렌더링 =======================
	//CAirplanePlayer* pPlayer = (CAirplanePlayer*)m_pPlayer;
	//if (m_pPlayer)
	//	pPlayer->Render(pd3dCommandList, pCamera);

}
void CScene::Particle_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{	
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->Update_Shader_Resource(pd3dCommandList);

	if(power_charge)
		Charge_Effect->Particle_Render(pd3dCommandList, pCamera);

	for (Particle* particle : m_particle)
		particle->Particle_Render(pd3dCommandList, pCamera);
}

void CScene::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (UI* ui_ptr : pUI_list)
		if (ui_ptr->Active)
		{
			ui_ptr->UI_Render(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		}
}

CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera) return(NULL);
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;
	// 화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 
	// 그 결과는 카메라 좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject* pIntersectedObject = NULL, * pNearestObject = NULL;

	pIntersectedObject = Object_Shader[0].PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
	if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
	{
		fNearestHitDistance = fHitDistance;
		pNearestObject = pIntersectedObject;
	}

	return(pNearestObject);
}
bool CScene::is_Object_Selectable(CGameObject* now_picked)
{
	if (now_picked == m_pSelectedObject || now_picked == NULL)
		return false;

	if (now_picked->player_team == false)
		return false;
	
	return true;
}

bool CScene::is_Player_Turn()
{
	return (Player_Turn && Player_Shot == false);
}

void CScene::Shoot_Stone(float power)
{
	if (m_pSelectedObject != NULL)
	{
		XMFLOAT3 direction = m_pPlayer->GetLookVector();
		m_pSelectedObject->SetMovingDirection(direction);
		m_pSelectedObject->SetMovingSpeed(power);

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

	for (CGameObject* obj_ptr : Object_Shader[0].GetObjects())
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
	for (CGameObject* obj_ptr : Object_Shader[0].GetObjects())
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


bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	for (CGameObject* obj_ptr : Object_Shader[0].GetObjects())
	{
		if ((obj_ptr->active == true) && (obj_ptr->player_team == true))
		{
			StoneObject* player_stone = (StoneObject*)obj_ptr;
			/*if (player_stone == m_pSelectedObject)
				m_pSelectedObject->Set_Material_and_Shader(material_black_stone, &Object_Shader[0]);
			else
				player_stone->Set_Material_and_Shader(material_black_stone, &Object_Shader[0]);*/
		}
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
			if (is_Player_Turn())
				power_charge = true;
			break;

		default:
			break;
		}
	}
	break;


	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SPACE:
			power_charge = false;
			ui_player_power->Reset();
			ui_com_power->Reset();
			break;

		case VK_TAB:
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