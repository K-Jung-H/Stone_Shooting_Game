#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "Timer.h"
#include "Shader.h"
#include "Particle.h"
#include "UI.h"
#include "Item.h"

class CScene
{
public:
	CScene();
	~CScene();

	ID3D12RootSignature* GetGraphicsRootSignature();

	//씬에서 마우스와 키보드 메시지를 처리
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	
	virtual void ReleaseObjects();

	virtual bool ProcessInput(UCHAR* pKeysBuffer);
	virtual void AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);
	virtual void Scene_Update(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);

	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Particle_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Message_Render(ID2D1DeviceContext2* pd2dDevicecontext);

	virtual void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성 
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	virtual ID3D12RootSignature* Create_UI_GraphicsRootSignature(ID3D12Device* pd3dDevice);


	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);

	//씬의 모든 조명과 재질을 생성
	virtual void Build_Lights_and_Materials();

	//씬의 모든 조명과 재질을 위한 리소스를 생성하고 갱신
	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	virtual void Update_Lights(ID3D12GraphicsCommandList* pd3dCommandList);

	void Set_BackGround_Color(XMFLOAT4 color);
	float* Get_BackGround_Color();

	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	CPlayer* GetPlayer() { return(m_pPlayer); }

	void Add_Font(IDWriteTextFormat* pfont) { write_font_list.push_back(pfont); }
	void Add_Brush(ID2D1SolidColorBrush* pbrush) { brush_list.push_back(pbrush); }
	//=============================================

protected:
	CShader* Object_Shader = NULL;
	int N_Object_Shader = 1;

	CShader* UI_Shader = NULL;
	int N_UI_Shader = 1;

	std::vector<UI*> UI_list;
	int ui_num = 0;

	CPlayer* m_pPlayer = NULL;
	//==========================================

	LIGHTS* m_pLights = NULL; // 씬의 조명

	ID3D12Resource* m_pd3dcbLights = NULL; // 조명을 나타내는 리소스
	LIGHTS* m_pcbMappedLights = NULL; // 조명 리소스에 대한 포인터

	//==========================================
	static CMaterial* material_color_none;

public:
	//루트 시그너쳐를 나타내는 인터페이스 포인터이다.
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	ID3D12RootSignature* UI_GraphicsRootSignature = NULL;

	//파이프라인 상태를 나타내는 인터페이스 포인터이다.
	ID3D12PipelineState* m_pd3dPipelineState = NULL;

public:
	// 그려질 모든 게임 객체들
	std::vector<CGameObject*> game_objects;

	float m_fElapsedTime = 0.0f;

	float background_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	std::vector<IDWriteTextFormat*> write_font_list;
	std::vector< ID2D1SolidColorBrush*> brush_list;
};

class Start_Scene : public CScene
{
public:
	Start_Scene();
	~Start_Scene();


	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	ID3D12RootSignature* Create_UI_GraphicsRootSignature(ID3D12Device* pd3dDevice) override;

	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice) override;

	void BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;

	void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList) override;
	void Release_Shader_Resource() override;

	void Update_Lights(ID3D12GraphicsCommandList* pd3dCommandList) override;

	void ReleaseObjects() override;
	void ReleaseUploadBuffers() override;

	void Build_Lights_and_Materials() override;

	void AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed) override;
	void Scene_Update(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed) override;

	void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void Message_Render(ID2D1DeviceContext2* pd2dDevicecontext) override;

	//-----------------------------------------------------

	void Create_Board(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float Board_Width, float Board_Depth);



	static CMaterial* material_color_white_stone;
	static CMaterial* material_color_black_stone;
	static CMaterial* material_color_board;
	static CMaterial* material_color_none;

	bool zooming = false;
	int zoom_value = 0;
};



class Playing_Scene : public CScene
{
private:
	std::vector<std::pair<int, int>> FindCollisionPairs(const std::vector<StoneObject*>& gameObjects);
	void UpdateVelocities(CGameObject* stone1, CGameObject* stone2, XMVECTOR vel1, XMVECTOR vel2);

public:
	Playing_Scene();
	~Playing_Scene();

	//씬에서 마우스와 키보드 메시지를 처리한다. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	
	void BuildScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void BuildUIs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void ReleaseObjects() override;
	void ReleaseUploadBuffers() override;

	void AnimateObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed) override;
	void Scene_Update(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed) override;

	void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	void Particle_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
	void UI_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void Item_Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Message_Render(ID2D1DeviceContext2* pd2dDevicecontext) override;


	//그래픽 루트 시그너쳐를 생성한다. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice) override;
	ID3D12RootSignature* Create_UI_GraphicsRootSignature(ID3D12Device* pd3dDevice) override;

	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice) override;

	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다. 
	StoneObject*Pick_Stone_Pointed_By_Cursor(int xClient, int yClient, CCamera *pCamera);
	StoneObject* Pick_Stone_By_RayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance);
	bool is_Object_Selectable(CGameObject* gameobject);
	//=============================================
	
	CGameObject* Pick_Item_Pointed_By_Cursor(int xClient, int yClient, CCamera* pCamera);
	CGameObject* Pick_Item_By_RayIntersection(XMFLOAT3& xmf3PickPosition, CCamera* pCamera, float* pfNearHitDistance);

	//씬의 모든 조명과 재질을 생성
	void Build_Lights_and_Materials() override;

	//씬의 모든 조명과 재질을 위한 리소스를 생성하고 갱신
	void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList) override;
	void Release_Shader_Resource() override;

	void Update_Lights(ID3D12GraphicsCommandList* pd3dCommandList);

	
	//=============================================
	void Create_Board(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float Board_Width, float Board_Depth);
	Inventory_UI* Create_Inventory_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_RECT area);


	void Setting_Stone(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMesh* mesh, XMFLOAT3 pos, bool player_team);


	// 돌 & 돌 충돌 검사
	void Check_Stones_Collisions();

	// 돌 & 보드 충돌 검사
	void Check_Board_and_Stone_Collisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	// 돌 & 아이템 충돌 검사
	void Check_Item_and_Stone_Collisions(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);


	std::pair<StoneObject*, StoneObject*> Select_Stone_Com();
	std::pair<StoneObject*, StoneObject*> Find_Nearest_Enemy_Stone();
	void Shoot_Stone_Com(float power);
	
	bool is_Player_Turn();
	void Shoot_Stone(float power);


	void Mark_selected_stone();
	bool Check_Item(Item_Type i_type);
	void Update_Item_Inventory();
	bool Update_Item_Manager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	bool Change_Turn();
	bool Check_Turn();
	bool Check_GameOver();

	void Remove_Unnecessary_Objects();

	void Setting_Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, Item_Type type);
	void Setting_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, CMaterial* material, Particle_Type type);
	
	//=============================================
protected:
	CShader* Outline_Shader = NULL;
	int N_Outline_Shader = 1;

//==========================================

	LIGHTS* m_pLights = NULL; // 씬의 조명

	ID3D12Resource* m_pd3dcbLights = NULL; // 조명을 나타내는 리소스
	LIGHTS* m_pcbMappedLights = NULL; // 조명 리소스에 대한 포인터

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
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//루트 시그너쳐를 나타내는 인터페이스 포인터이다.

	ID3D12RootSignature* UI_GraphicsRootSignature = NULL;


	ID3D12PipelineState* m_pd3dPipelineState = NULL;
	//파이프라인 상태를 나타내는 인터페이스 포인터이다.
	

public:
	CBoardObject* m_pBoards = NULL;
	
	UI* ui_player_power;
	UI* ui_com_power;
	Inventory_UI* player_inventory;

	Charge_Particle* Charge_Effect = NULL;
	Snow_Particle* Snow_Effect = NULL;

	Item_Manager* item_manager = NULL;

	// 그려질 모든 게임 객체들
	std::vector<StoneObject*> GameObject_Stone;
	std::vector<Item*> Game_Items;
	std::vector<Particle*>m_particle;


	float m_fElapsedTime = 0.0f;

	bool Camera_First_Person_View = false;

	bool Com_Turn = false;
	bool Com_Shot = false;

	bool Player_Turn = true;
	bool Player_Shot = false;

	bool Game_Over = false;

	bool power_charge = false;
	int power_degree = 0;

	struct Game_Player
	{
		std::unordered_map<Item_Type, int> Item_Inventory;
		std::vector<StoneObject*> stone_list; 
		StoneObject* select_Stone = NULL; // 피킹된 것
		Item_Type selected_Item_Type = Item_Type::None; // 피킹된 아이템의 타입
		bool inventory_open = false;
	}player1;


	struct Computer
	{
		std::vector<StoneObject*> stone_list;
		StoneObject* select_Stone = NULL;
		StoneObject* target_Stone = NULL;
		float random_time = -1;
		float sum_time = 0;

	}computer;


	float random_time = -1;
	float sum_time = 0;

	// 턴 시간 제한
	float Limit_time = 0.0f;

	// 턴 넘김 시간 : 2초
	float Delay_time = 0.0f;

	bool Need_to_change_turn = false;
};