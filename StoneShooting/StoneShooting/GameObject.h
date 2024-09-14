#pragma once
#include "Camera.h"
#include "Mesh.h"

class CShader;

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

// 객체를 렌더링할 때 적용하는 상수 버퍼 데이터
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

struct CB_MATERIAL_INFO
{
	XMFLOAT4				m_cAmbient;
	XMFLOAT4				m_cDiffuse;
	XMFLOAT4				m_cSpecular; //a = power
	XMFLOAT4				m_cEmissive;
};

struct MATRIX4X4_BUFFER
{
	XMFLOAT4X4 mat4x4; 
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType;

	int								m_nTextures = 0;
	ID3D12Resource** m_ppd3dTextures = NULL;
	ID3D12Resource** m_ppd3dTextureUploadBuffers;

	UINT* m_pnResourceTypes = NULL;

	_TCHAR(*m_ppstrTextureNames)[64] = NULL;

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;

	int								m_nRootParameters = 0;
	int* m_pnRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);

	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);
	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);

	int GetRootParameters() { return(m_nRootParameters); }
	int GetTextures() { return(m_nTextures); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }
	int GetRootParameter(int nIndex) { return(m_pnRootParameterIndices[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};

class CMaterialColors
{
private:
	int								m_nReferences = 0;

public:
	XMFLOAT4						m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	XMFLOAT4						m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); //(r,g,b,a=power)
	XMFLOAT4						m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);


	CMaterialColors() { }
	CMaterialColors(XMFLOAT4 a, XMFLOAT4 d, XMFLOAT4 s, XMFLOAT4 e) 
	{
		m_xmf4Ambient = a;
		m_xmf4Diffuse = d;
		m_xmf4Specular = s;
		m_xmf4Emissive = e;
	}
	virtual ~CMaterialColors() { }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

};

class CMaterial
{
private:
	int m_nReferences = 0;

public:
	CTexture* m_pTexture = NULL; // 재질에 사용할 텍스쳐
	CShader* material_shader = NULL; 	// 재질에 적용할 추가적인 셰이더
	CMaterialColors* Material_Colors = NULL;


	CMaterial();
	CMaterial(CMaterialColors* M_C);

	virtual ~CMaterial();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }


	void SetMaterialColors(CMaterialColors* pMaterialColors);
	void SetShader(CShader* pShader);
	void SetTexture(CTexture* pTexture);
};


class CGameObject
{
private:
	int m_nReferences = 0;

public:
	// 게임 객체가 쉐이더를 가지지 않고 재질을 가짐
	// 재질마다 사용할 셰이더가 다를 수 있음
	std::vector<std::pair<CMaterial*, bool>> m_ppMaterials;

	std::vector<CMesh*> mesh_list;

//=========================================

	XMFLOAT4X4						m_xmf4x4Transform = Matrix4x4::Identity();
	XMFLOAT4X4						m_xmf4x4World = Matrix4x4::Identity();

	CGameObject* m_pParent = NULL;
	std::vector<CGameObject*> m_pChild;  // 자식 노드들
	std::vector<CGameObject*> m_pSibling;  // 형제 노드들

//=========================================
// 
	// GPU에 있는 객체 정보 컨테이너에 대한 리소스 포인터
	// 해당 포인터를 통해, GPU에 있는 정보 접근 가능
	CB_GAMEOBJECT_INFO* Mapped_Object_info = NULL;

	// GPU에 있는 객체 재질에 대한 리소스 포인터
	CB_MATERIAL_INFO* Mapped_Material_info = NULL;

	// 쉐이더 객체에 연결된 게임 객체 버퍼
	ID3D12Resource* Object_Constant_Buffer = NULL;

	// 쉐이더 객체에 연결된 재질 버퍼
	ID3D12Resource* Material_Constant_Buffer = NULL;

//=========================================
	char						FrameName[64] = {};

	bool						active = true;
	bool						player_team = false;
	Object_Type					o_type = Object_Type::None;
	Item_Type					used_item = Item_Type::None;


	BoundingOrientedBox			default_collider = BoundingOrientedBox();
	CGameObject*				m_pObjectCollided = NULL;


	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float						m_fMovingSpeed = 0.0f;
	float						m_fMovingRange = 0.0f;
	float						m_fFriction = 2.0f;

	CGameObject() {};
	CGameObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nMeshes = 0);
	virtual ~CGameObject();

	void AddRef();
	void Release();

	void SetActive(bool bActive) { active = bActive; }



	void ReleaseUploadBuffers();
	
	void SetMesh(int nIndex, CMesh* pMesh);
	void AddMesh(CMesh* pMesh);

	void Set_MaterialShader(CShader* pShader, int nMaterial = 0);
	void SetMaterial(CMaterial* pMaterial, bool front_insert = false);
	void AddMaterial(CMaterial* pMaterial, bool active = false);
	void ChangeMaterial(UINT n);

	void Add_Child(CGameObject* pChild, bool bReferenceUpdate = false);
	void Add_Sibling(CGameObject* pSibling, bool bReferenceUpdate = false);
	CGameObject* GetParent() { return(m_pParent); }


	CGameObject* FindFrame(char* pstrFrameName);
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	
	virtual void Create_Object_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Create_Material_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);


	virtual void Update_Object_Buffer();
	virtual void Update_Material_Buffer(ID3D12GraphicsCommandList* pd3dCommandList, int N);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList, Resource_Buffer_Type type, int N = 0);

	virtual void Release_Shader_Resource();

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);

	// FLOAT3에 행렬을 적용시켜서 반환
	XMFLOAT3 ApplyTransform(XMFLOAT3 xmfloat3, XMFLOAT4X4 xmfloat4x4);

	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	
	//게임 객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetFriction(float f) { m_fFriction = f; }
	void SetScale(float x, float y, float z);
	//초기 움직임 설정
	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingDirection_Reverse();
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }


	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	//게임 객체를 회전(x-축, y-축, z-축)한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);
	
	void LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	
	//게임 객체가 카메라에 보인는 가를 검사한다. 
	//bool IsVisible(CCamera *pCamera=NULL);

	//모델 좌표계의 픽킹 광선을 생성한다. 
	void Generate_Ray_For_Picking_Projection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	
	void Generate_Ray_For_Picking_Orthographic(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT4X4& xmf4x4Projection,
		XMFLOAT3* pxmf3RayOrigin, XMFLOAT3* pxmf3RayDirection);


	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다. 
	int Pick_Object_By_Ray_Intersection_Projection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
	int Pick_Object_By_Ray_Intersection_Orthographic(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT4X4& xmfprojection, float* pfHitDistance);


	void UpdateBoundingBox();

	BoundingOrientedBox Get_Collider();

	bool Is_Visible_Collider(CCamera* pCamera);


	void UpdateFriction(float fTimeElapsed);

	void Apply_Item(Item_Type type);
};

class CRotatingObject : public CGameObject
{

public:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

	CRotatingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nMeshes = 0);
	virtual ~CRotatingObject();

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	void Rotate_to_Player(float fElapsedTime, XMFLOAT3& xmf3LookTo);
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent);
};


class CBoardObject : public CRotatingObject
{
public:
	CBoardObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CBoardObject();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
};


class StoneObject : public CRotatingObject
{
public:
	StoneObject* Overlaped = NULL;
	BoundingSphere stone_collider = BoundingSphere();

	StoneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~StoneObject();

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
	
	BoundingSphere Get_Collider();

};

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, 
		int nBlockWidth, int nBlockLength,  XMFLOAT3 xmf3Scale, bool texture);
	virtual ~CHeightMapTerrain();

private:
	// 지형의 높이 맵으로 사용할 이미지
	CHeightMapImage* m_pHeightMapImage;

	// 높이 맵의 가로와 세로 크기
	int	m_nWidth;
	int	m_nLength;
		
	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터
	XMFLOAT3 m_xmf3Scale;
	int m_nMeshes;

public:
	// 지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값
	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y);
	}

	// 지형의 법선 벡터를 계산하는 함수(월드 좌표계). 
	// 높이 맵의 법선 벡터를 사용
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}
	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다.
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};