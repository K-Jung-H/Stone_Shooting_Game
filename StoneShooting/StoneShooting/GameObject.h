#pragma once
#include "Camera.h"
#include "Mesh.h"




class CShader;

// ��ü�� �������� �� �����ϴ� ��� ���� ������
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
	CShader* material_shader = NULL; 	// ������ ������ �߰����� ���̴�
	CMaterialColors* Material_Colors = NULL;

	bool Active { false };

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
	// ���� ��ü�� ���̴��� ������ �ʰ� ������ ����
	// �������� ����� ���̴��� �ٸ� �� ����
	std::vector<CMaterial*> m_ppMaterials;


	CMesh* m_pMesh = NULL;
	
//=========================================

	XMFLOAT4X4						m_xmf4x4Transform = Matrix4x4::Identity();
	XMFLOAT4X4						m_xmf4x4World = Matrix4x4::Identity();

	CGameObject* m_pParent = NULL;
	CGameObject* m_pChild = NULL;
	CGameObject* m_pSibling = NULL;

//=========================================
	// GPU�� �ִ� ��ü ���� �����̳ʿ� ���� ���ҽ� ������
	// �ش� �����͸� ����, GPU�� �ִ� ���� ���� ����
	CB_GAMEOBJECT_INFO* Mapped_Object_info = NULL;

	// GPU�� �ִ� ��ü ������ ���� ���ҽ� ������
	CB_MATERIAL_INFO* Mapped_Material_info = NULL;

	// ���̴� ��ü�� ����� ���� ��ü ����
	ID3D12Resource* Object_Constant_Buffer = NULL;

	// ���̴� ��ü�� ����� ���� ����
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

	CGameObject();
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

	void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);


	CGameObject* FindFrame(char* pstrFrameName);
	CGameObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	
	virtual void Create_Object_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Create_Material_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);


	virtual void Update_Object_Buffer();
	virtual void Update_Material_Buffer(int N);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList, Resource_Buffer_Type type, int N = 0);

	virtual void Release_Shader_Resource();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);

	// FLOAT3�� ����� ������Ѽ� ��ȯ
	XMFLOAT3 ApplyTransform(XMFLOAT3 xmfloat3, XMFLOAT4X4 xmfloat4x4);

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	
	//���� ��ü�� ��ġ�� �����Ѵ�.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetFriction(float f) { m_fFriction = f; }
	
	//�ʱ� ������ ����
	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingDirection_Reverse();
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }


	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	
	void LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	
	//���� ��ü�� ī�޶� ���δ� ���� �˻��Ѵ�. 
	bool IsVisible(CCamera *pCamera=NULL);

	//�� ��ǥ���� ��ŷ ������ �����Ѵ�. 
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	
	//ī�޶� ��ǥ���� �� ���� ���� �� ��ǥ���� ��ŷ ������ �����ϰ� ��ü���� ������ �˻��Ѵ�. 
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);


	void UpdateBoundingBox();

	void UpdateFriction(float fTimeElapsed);
};

class CRotatingObject : public CGameObject
{

public:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

	CRotatingObject();
	virtual ~CRotatingObject();

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	void Rotate_to_Player(float fElapsedTime, XMFLOAT3& xmf3LookTo);
	virtual void Animate(float fTimeElapsed);
};


class CExplosiveObject : public CRotatingObject
{
public:
	CExplosiveObject();
	virtual ~CExplosiveObject();

	bool						m_bBlowingUp = false;

	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
};

class CBoardObject : public CGameObject
{
public:
	CBoardObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CBoardObject();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);

};


class StoneObject : public CExplosiveObject
{
public:
	StoneObject* Overlaped = NULL;

	StoneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~StoneObject();

	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
};

inline float RandF(float fMin, float fMax);
XMVECTOR RandomUnitVectorOnSphere();