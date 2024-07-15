#pragma once
#include "Camera.h"
#include "Timer.h"
#include "Shader.h"


struct LIGHT
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT3 m_xmf3Position;
	float m_fFalloff;
	XMFLOAT3 m_xmf3Direction;
	float m_fTheta; //cos(m_fTheta)
	XMFLOAT3 m_xmf3Attenuation;
	float m_fPhi; //cos(m_fPhi)
	bool m_bEnable;
	int m_nType;
	float m_fRange;
	float padding;
};

struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
};

struct MATERIALS
{
	MATERIAL m_pReflections[MAX_MATERIALS];
};


class CScene
{
public:
	CScene();
	~CScene();

	//씬에서 마우스와 키보드 메시지를 처리한다. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);


	void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	
	void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);

	ID3D12RootSignature* GetGraphicsRootSignature();

	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다. 
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);

	//=============================================
	
	//씬의 모든 조명과 재질을 생성
	void Build_Lights_and_Materials();

	//씬의 모든 조명과 재질을 위한 리소스를 생성하고 갱신
	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();


	void Update_Lights_and_Materials(ID3D12GraphicsCommandList* pd3dCommandList);
	//=============================================

	void Setting_Stone(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMesh* mesh, XMFLOAT3 pos, bool player_team);

	void CheckObjectByObjectCollisions();
	void CheckObject_Out_Board_Collisions();

	void Shoot_Stone(float power);
	void Shoot_Stone_Com(float power);
	
	std::pair<StoneObject*, StoneObject*> Find_Nearest_Enemy_Stone();


	void Change_Turn();
	bool Check_Turn();
	bool Check_GameOver();

	void Defend_Overlap();

	//=============================================

protected:
	CObjectsShader* m_pShaders = NULL;
	int m_nShaders = 1;

	LIGHTS* m_pLights = NULL; // 씬의 조명

	ID3D12Resource* m_pd3dcbLights = NULL; // 조명을 나타내는 리소스
	LIGHTS* m_pcbMappedLights = NULL; // 조명 리소스에 대한 포인터


	MATERIALS* m_pMaterials = NULL; //씬의 객체들에 적용되는 재질


	ID3D12Resource* m_pd3dcbMaterials = NULL; // 재질을 나타내는 리소스
	MATERIAL* m_pcbMappedMaterials = NULL; // 재질 리소스에 대한 포인터

public:
	CObjectsShader* m_uiShaders = NULL;
	int m_n_uiShaders = 1;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//루트 시그너쳐를 나타내는 인터페이스 포인터이다.

	ID3D12PipelineState* m_pd3dPipelineState = NULL;
	//파이프라인 상태를 나타내는 인터페이스 포인터이다.
	

public:
	CPlayer* m_pPlayer = NULL;

	CGameObject** m_ppGameObjects = NULL;

	CBoardObject* m_pBoards = NULL;

	std::vector<CParticle*>m_particle;

	CGameObject* m_pSelectedObject = NULL; // 피킹된 것
	
	int m_nGameObjects = 0;

	float m_fElapsedTime = 0.0f;

	bool Com_Turn = false;
	bool Com_Shot = false;

	bool Player_Turn = true;
	bool Player_Shot = false;

	bool Game_Over = false;

};