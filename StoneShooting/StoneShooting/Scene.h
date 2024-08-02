#pragma once
#include "Camera.h"
#include "Timer.h"
#include "Shader.h"
#include "Particle.h"
#include "UI.h"


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

class CScene
{
public:
	CScene();
	~CScene();

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	void BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Scene_Update(float fTimeElapsed);

	void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void Particle_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	
	void ReleaseUploadBuffers();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* Create_UI_GraphicsRootSignature(ID3D12Device* pd3dDevice);


	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);

	ID3D12RootSignature* GetGraphicsRootSignature();

	//���� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ѵ�. 
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);
	CGameObject* PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance);
	bool is_Object_Selectable(CGameObject* gameobject);
	//=============================================
	
	//���� ��� ����� ������ ����
	void Build_Lights_and_Materials();

	//���� ��� ����� ������ ���� ���ҽ��� �����ϰ� ����
	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();


	void Update_Lights(ID3D12GraphicsCommandList* pd3dCommandList);

	
	//=============================================
	void Create_Board(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float Board_Width, float Board_Depth);



	void Setting_Stone(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMesh* mesh, XMFLOAT3 pos, bool player_team);

	void CheckObjectByObjectCollisions();
	void CheckObject_Out_Board_Collisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void Shoot_Stone(float power);

	std::pair<StoneObject*, StoneObject*> Select_Stone_Com();
	void Shoot_Stone_Com(float power);
	
	bool is_Player_Turn();

	std::pair<StoneObject*, StoneObject*> Find_Nearest_Enemy_Stone();


	void Change_Turn();
	bool Check_Turn();
	bool Check_GameOver();

	void Defend_Overlap();
	void Remove_Unnecessary_Objects();

	void Setting_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, CMaterial* material, ParticleType type);
	//=============================================
private:
	CCamera* pMainCamera = NULL;



protected:
	CShader* Object_Shader = NULL;
	int N_Object_Shader = 1;

	CShader* UI_Shader = NULL;
	int N_UI_Shader = 1;

	std::vector<UI*> UI_list;
	int ui_num = 0;

//==========================================

	LIGHTS* m_pLights = NULL; // ���� ����

	ID3D12Resource* m_pd3dcbLights = NULL; // ������ ��Ÿ���� ���ҽ�
	LIGHTS* m_pcbMappedLights = NULL; // ���� ���ҽ��� ���� ������

//==========================================
	
	static CMaterial* material_color_white_stone;
	static CMaterial* material_color_black_stone;

	static CMaterial* material_color_player_selected;
	static CMaterial* material_color_com_selected;

	static CMaterial* material_color_white_particle;
	static CMaterial* material_color_black_particle;

	static CMaterial* material_color_board;
	static CMaterial* material_color_none;
public:
	//UIShader* m_uiShaders = NULL;
	//int m_n_uiShaders = 1;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�.

	ID3D12RootSignature* UI_GraphicsRootSignature = NULL;


	ID3D12PipelineState* m_pd3dPipelineState = NULL;
	//���������� ���¸� ��Ÿ���� �������̽� �������̴�.
	

public:
	CPlayer* m_pPlayer = NULL;

	CBoardObject* m_pBoards = NULL;

	// �׷��� ��� ���� ��ü��
	std::vector<CGameObject*> GameObject_Stone;
	int Scene_GameObjects_N = 0;

	std::vector<Particle*>m_particle;


	UI* ui_player_power;
	UI* ui_player_power_endline;

	UI* ui_com_power;
	UI* ui_com_power_endline;

	CGameObject* m_pSelectedObject = NULL; // ��ŷ�� ��
	Charge_Particle* Charge_Effect = NULL;


	int m_nGameObjects = 0;

	float m_fElapsedTime = 0.0f;

	bool Com_Turn = false;
	bool Com_Shot = false;

	bool Player_Turn = true;
	bool Player_Shot = false;

	bool Game_Over = false;

	bool power_charge = false;
	int power_degree = 0;

	struct Computer
	{
		StoneObject* select_Stone;
		StoneObject* target_Stone;
		float random_time = -1;
		float sum_time = 0;

	}computer;
};