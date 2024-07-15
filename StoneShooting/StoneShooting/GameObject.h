#pragma once
#include "Camera.h"
#include "Mesh.h"

class CShader;

// 객체를 렌더링할 때 적용하는 상수 버퍼 데이터
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;

	// 객체에 적용될 재질 번호
	UINT m_nMaterial;
};

struct MATERIAL
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive;
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	//재질의 기본 색상
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//재질의 번호
	UINT m_nReflection = 0;

	// 별도로 재질을 적용하여 렌더링을 하기 위한 쉐이더
	CShader* m_pShader = NULL;
	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShader(CShader* pShader);
};


class CGameObject
{
private:
	int m_nReferences = 0;

public:
	CMesh* m_pMesh = NULL;
	CMaterial* m_pMaterial = NULL; // 게임 객체가 쉐이더를 가지지 않고 재질을 가짐
	
	// 객체에 대한 리소스 포인터
	CB_GAMEOBJECT_INFO* m_pMapped_object_info = NULL;

	// 쉐이더 객체에 포함되어 있는 모든 게임 객체들에 대한 리소스
	ID3D12Resource* m_pConstant_Buffer = NULL;


	bool						m_bActive = true;
	bool						picked = false;
	bool						player_team = false;
	XMFLOAT4X4					m_xmf4x4World = Matrix4x4::Identity();

	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();
	BoundingSphere				m_xmOOSP = BoundingSphere();
	CGameObject*				m_pObjectCollided = NULL;
	DWORD						m_dwDefaultColor = RGB(255, 0, 0);
	DWORD						m_dwColor = RGB(255, 0, 0);

	XMFLOAT3					m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float						m_fMovingSpeed = 0.0f;
	float						m_fMovingRange = 0.0f;

	float m_fFriction;

	CGameObject();
	virtual ~CGameObject();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetActive(bool bActive) { m_bActive = bActive; }


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	void ReleaseUploadBuffers();
	
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);

	void SetMaterial(CMaterial* pMaterial);
	void SetMaterial(UINT nReflection);

	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

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

	
	void LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up);
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	
	//게임 객체가 카메라에 보인는 가를 검사한다. 
	bool IsVisible(CCamera *pCamera=NULL);

	//모델 좌표계의 픽킹 광선을 생성한다. 
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	
	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다. 
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
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};



#define EXPLOSION_DEBRISES		240

class CParticle : public CRotatingObject
{
public:
	UINT8* particles_info = NULL;

	static CMesh* m_ExplosionMesh;
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 2.0f;
	float						m_fExplosionSpeed = 10.0f;
	float						m_fExplosionRotation = 720.0f;
	
	CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CParticle();

	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	virtual void Animate(float fElapsedTime);
	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

};


class CBoardObject : public CGameObject
{
public:
	CBoardObject();
	virtual ~CBoardObject();

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};


class StoneObject : public CExplosiveObject
{
public:
	StoneObject* Overlaped = NULL;

	StoneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~StoneObject();

	float Fire_Period{ 1 };
	float cool_down{ 0 };

	float						m_fBulletEffectiveRange = 150.0f;

	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};

inline float RandF(float fMin, float fMax);
XMVECTOR RandomUnitVectorOnSphere();