#include "stdafx.h"
#include "Scene.h"
#include "Player.h"
#include <vector>
XMFLOAT3& Get_Random_Normalize_Direction()
{
	float randX = static_cast<float>(rand()) / RAND_MAX; 
	float randY = static_cast<float>(rand()) / RAND_MAX; 
	float randZ = static_cast<float>(rand()) / RAND_MAX; 

	XMFLOAT3 random_D(randX, randY, randZ);

	XMVECTOR normalized_D= XMVector3Normalize(XMLoadFloat3(&random_D));

	XMStoreFloat3(&random_D, normalized_D);

	return random_D;
}

CScene::CScene()
{
}
CScene::~CScene()
{
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

void CScene::Setting_Stone(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMesh* mesh, XMFLOAT3 pos, bool player_team)
{
	StoneObject* pStoneObject = NULL;

	pStoneObject = new StoneObject(pd3dDevice, pd3dCommandList);
	pStoneObject->SetMesh(mesh);
	pStoneObject->SetPosition(pos.x, pos.y, pos.z);
	pStoneObject->SetFriction(2);										// Default

	pStoneObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));			// Default
	pStoneObject->SetRotationSpeed(0.0f);								// Default

	if (player_team)
		pStoneObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));	// Default
	else
		pStoneObject->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));	// Default

	pStoneObject->SetMovingSpeed(0.0f);									// Default

	pStoneObject->player_team = player_team;
	m_pShaders[0].AddObjects(pStoneObject);
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);


	//===========================================================
	// 게임 객체
	m_pShaders = new CObjectsShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	//===========================================================

	float Board_Half_Width = 100.0f;
	float Board_Half_Depth = 300.0f;
	CPlaneMesh* pboard = new CPlaneMesh(pd3dDevice, pd3dCommandList, Board_Half_Width * 2.0f, Board_Half_Depth * 2.0f, XMFLOAT4(0.4f, 0.3f, 0.1f, 0.0f));
	m_pBoards = new CBoardObject();
	m_pBoards->SetPosition(0.0f, 0.0f, 0.0f);
	m_pBoards->SetMesh(pboard);
	m_pBoards->m_xmOOBB = m_pBoards->m_pMesh->m_xmBoundingBox; // 시작할 때 한번만 하면 됨

	//===========================================================
	CSphereMeshDiffused* pWhiteStoneMesh = new CSphereMeshDiffused(pd3dDevice, pd3dCommandList, 6.0f, 20, 20, false);
	std::vector<XMFLOAT3> w_stone_pos_list;

	w_stone_pos_list.push_back({ -50, 5, 100 });
	w_stone_pos_list.push_back({ 0, 5, 100 });
	w_stone_pos_list.push_back({ 50, 5, 100 });
	w_stone_pos_list.push_back({ -25, 5, 150 });
	w_stone_pos_list.push_back({ 0, 5, 150 });
	w_stone_pos_list.push_back({ 25, 5, 150 });

	for(XMFLOAT3& w_stone_pos : w_stone_pos_list)
		Setting_Stone(pd3dDevice, pd3dCommandList, pWhiteStoneMesh, w_stone_pos, true);

	//-------------------------------------------------------------------

	CSphereMeshDiffused* pBlackStoneMesh = new CSphereMeshDiffused(pd3dDevice, pd3dCommandList, 6.0f, 20, 20, true);
	std::vector<XMFLOAT3> b_stone_pos_list;

	b_stone_pos_list.push_back({ -50, 5, -100 });
	b_stone_pos_list.push_back({ 0, 5, -100 });
	b_stone_pos_list.push_back({ 50, 5, -100 });
	b_stone_pos_list.push_back({ -25, 5, -150 });
	b_stone_pos_list.push_back({ 0, 5, -150 });
	b_stone_pos_list.push_back({ 25, 5, -150 });

	for (XMFLOAT3& b_stone_pos : b_stone_pos_list)
		Setting_Stone(pd3dDevice, pd3dCommandList, pBlackStoneMesh, b_stone_pos, false);

	CExplosiveObject::PrepareExplosion(pd3dDevice, pd3dCommandList);

	//===========================================================
	// UI 객체
	m_uiShaders = new UIShader[m_n_uiShaders];
	m_uiShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);

	CMesh* ui_mesh = new UIMesh(pd3dDevice, pd3dCommandList, 200.0f, 90.0f, 1.0f, XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f));
	 
	CGameObject* ui_sample = NULL;
	ui_sample = new CGameObject();
	ui_sample->SetMesh(ui_mesh);
	m_uiShaders->AddObjects(ui_sample);
	//===========================================================
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) 
		m_pd3dGraphicsRootSignature->Release();


	for (int j = 0; j < m_nShaders; ++j)
	{
		m_pShaders[j].ReleaseShaderVariables();
		m_pShaders[j].ReleaseObjects();
	}

	for (int j = 0; j < m_n_uiShaders; ++j)
	{
		m_uiShaders[j].ReleaseShaderVariables();
		m_uiShaders[j].ReleaseObjects();
	}


	if (m_pShaders)
		delete[] m_pShaders;

	if (m_uiShaders)
		delete[] m_uiShaders;
}

void CScene::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nShaders; ++j)
		m_pShaders[j].ReleaseUploadBuffers();
}




ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}


ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[2];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
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

void CScene::CheckObjectByObjectCollisions()
{
	// CShader 벡터의 참조를 임시 변수에 저장
	auto& objshader_ptrs = m_pShaders[0].GetObjects();
	int objs_N = objshader_ptrs.size();

	// 충돌 객체 초기화
	for (CGameObject* obj_ptr : objshader_ptrs)
		obj_ptr->m_pObjectCollided = NULL;

	// 충돌 체크 및 충돌 객체 설정
	for (int i = 0; i < objs_N; ++i)
	{
		if (objshader_ptrs[i]->m_bActive)
		{
			for (int j = (i + 1); j < objs_N; ++j)
			{
				if (objshader_ptrs[j]->m_bActive)
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

void CScene::CheckObject_Out_Board_Collisions()
{
	// 충돌 객체 초기화
	for (CGameObject* obj_ptr : m_pShaders[0].GetObjects())
	{
		if (!obj_ptr->m_bActive)
			continue;
		ContainmentType containType = m_pBoards->m_xmOOBB.Contains(obj_ptr->m_xmOOSP);
		switch (containType)
		{
		case DISJOINT:
		{
			CExplosiveObject* pExplosiveObject = (CExplosiveObject*)obj_ptr;
			pExplosiveObject->SetMovingSpeed(0.0f);
			pExplosiveObject->m_bBlowingUp = true;
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
	}
	else if (Com_Turn)
	{
		Player_Turn = true;
		Player_Shot = false;
		Com_Turn = false;
	}
}

bool CScene::Check_Turn()
{
	if (!Player_Shot && Player_Turn)
		return false;

	if (!Com_Shot && Com_Turn)
		return false;

	int stop_stone_n = 0;

	for (CGameObject* obj_ptr : m_pShaders[0].GetObjects())
	{
		if ((obj_ptr->m_bActive == false) || 0.1f >= obj_ptr->m_fMovingSpeed)
			stop_stone_n += 1;
	}

	if (stop_stone_n == m_pShaders[0].GetObjects().size())
		return true;
	else
		return false;
	
}

bool CScene::Check_GameOver()
{
	int dead_White_Stone = 0;
	int dead_Black_Stone = 0;
	for (CGameObject* obj_ptr : m_pShaders[0].GetObjects())
	{
		if (obj_ptr->m_bActive == false)
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

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_pShaders[0].AnimateObjects(fTimeElapsed);
	m_uiShaders[0].AnimateObjects(fTimeElapsed);

	((CAirplanePlayer*)m_pPlayer)->Animate(fTimeElapsed);
}

void CScene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera); 
	}

	// 보드는 Scene에서 관리 중
	// 아직은 움직임이 없을꺼니까
	m_pBoards->Render(pd3dCommandList, pCamera);

	CAirplanePlayer* pPlayer = (CAirplanePlayer*)m_pPlayer;

	//=======================플레이어 렌더링 =======================

	if (m_pPlayer)
	{
		pPlayer->Render(pd3dCommandList, pCamera);
	}
}

void CScene::UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	for (int i = 0; i < m_n_uiShaders; i++)
	{
		m_uiShaders[i].Render(pd3dCommandList, pCamera);
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

	pIntersectedObject = m_pShaders[0].PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
	if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
	{
		fNearestHitDistance = fHitDistance;
		pNearestObject = pIntersectedObject;
	}

	return(pNearestObject);
}

void CScene::Shoot_Stone(float power)
{
	if (m_pSelectedObject != NULL)
	{
		XMFLOAT3 direction = m_pPlayer->GetLookVector();
		m_pSelectedObject->SetMovingDirection(direction);
		m_pSelectedObject->SetMovingSpeed(power);
	}
}

void CScene::Shoot_Stone_Com(float power)
{
	StoneObject* c_stone;
	StoneObject* p_stone;

	std::pair<StoneObject*, StoneObject*> stones = Find_Nearest_Enemy_Stone();

	c_stone = stones.first;
	p_stone = stones.second;
	XMFLOAT3 c_position = c_stone->GetPosition();
	XMFLOAT3 p_position = p_stone->GetPosition();

	XMFLOAT3 direction;
	XMStoreFloat3(&direction, XMVectorSubtract(XMLoadFloat3(&p_position), XMLoadFloat3(&c_position)));
	XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&direction))); // 정규화까지 하기

	// 돌 날리기
	c_stone->SetMovingDirection(direction);
	c_stone->SetMovingSpeed(500.0f);
	Com_Shot = true;

}

std::pair<StoneObject*, StoneObject*> CScene::Find_Nearest_Enemy_Stone()
{


	std::vector<StoneObject*>Living_C_Stone;
	std::vector< StoneObject*>Living_Player_Stone;
	for (CGameObject* obj_ptr : m_pShaders[0].GetObjects())
	{
		if ((obj_ptr->m_bActive == true) && (obj_ptr->player_team == false))
		{
			StoneObject* com_stone = (StoneObject*)obj_ptr;
			Living_C_Stone.push_back(com_stone);
		}
		else if ((obj_ptr->m_bActive == true) && (obj_ptr->player_team == true))
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
	return(false);
}
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}