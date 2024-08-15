#pragma once
#include "Camera.h"
#include "Mesh.h"

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



class CShader;

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
	CShader* material_shader = NULL; 	// 재질에 적용할 추가적인 셰이더
	CMaterialColors* Material_Colors = NULL;


	CMaterial();
	CMaterial(CMaterialColors* M_C);

	virtual ~CMaterial();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }


	void SetMaterialColors(CMaterialColors* pMaterialColors);
	void SetShader(CShader* pShader);

};


class CGameObject
{
private:
	int m_nReferences = 0;

public:
	// 게임 객체가 쉐이더를 가지지 않고 재질을 가짐
	// 재질마다 사용할 셰이더가 다를 수 있음
	std::vector<std::pair<CMaterial*, bool>> m_ppMaterials;


	CMesh* m_pMesh = NULL;
	
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
	Object_Type					type = Object_Type::None;

	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();
	BoundingSphere				m_xmOOSP = BoundingSphere();
	CGameObject*				m_pObjectCollided = NULL;


	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float						m_fMovingSpeed = 0.0f;
	float						m_fMovingRange = 0.0f;
	float						m_fFriction = 2.0f;

	CGameObject() {}
	CGameObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CGameObject();

	void AddRef();
	void Release();

	void SetActive(bool bActive) { active = bActive; }



	void ReleaseUploadBuffers();
	
	void SetMesh(CMesh* pMesh);
	void Set_MaterialShader(CShader* pShader, int nMaterial = 0);
	void SetMaterial(CMaterial* pMaterial);
	void AddMaterial(CMaterial* pMaterial);
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
	virtual void Update_Material_Buffer(int N);
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
	bool IsVisible(CCamera *pCamera=NULL);

	//모델 좌표계의 픽킹 광선을 생성한다. 
	void Generate_Ray_For_Picking_Projection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	
	void Generate_Ray_For_Picking_Orthographic(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT4X4& xmf4x4Projection,
		XMFLOAT3* pxmf3RayOrigin, XMFLOAT3* pxmf3RayDirection);


	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다. 
	int Pick_Object_By_Ray_Intersection_Projection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
	int Pick_Object_By_Ray_Intersection_Orthographic(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT4X4& xmfprojection, float* pfHitDistance);


	void UpdateBoundingBox();

	void UpdateFriction(float fTimeElapsed);
};

class CRotatingObject : public CGameObject
{

public:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

	CRotatingObject() {}
	CRotatingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
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

	StoneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~StoneObject();

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
};

