#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

//=================================================================================

CMaterial::CMaterial()
{
	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

CMaterial::~CMaterial()
{
	if (m_pShader)
	{
		m_pShader->Release_Shader_Resource();
		m_pShader->Release();
	}
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

//=================================================================================


CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();

	// 재질에서 처리
	// 게임 객체는 셰이더가 아니라 재질을 가짐
	/*if (m_pShader) 
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}*/

	if (m_pMaterial)
		m_pMaterial->Release();
}

void CGameObject::SetShader(CShader* pShader)
{
	// 재질에서 처리
	// 게임 객체는 셰이더가 아니라 재질을 가짐
	//if (m_pShader) m_pShader->Release();
	//m_pShader = pShader;
	//if (m_pShader) m_pShader->AddRef();

	if (!m_pMaterial)
	{
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}

	if (m_pMaterial)
		m_pMaterial->SetShader(pShader);
}
void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetMaterial(CMaterial* pMaterial)
{
	if (m_pMaterial)
		m_pMaterial->Release();

	m_pMaterial = pMaterial;

	if (m_pMaterial)
		m_pMaterial->AddRef();
}

void CGameObject::SetMaterial(UINT nReflection)
{
	if (!m_pMaterial)
		m_pMaterial = new CMaterial();

	m_pMaterial->m_nReflection = nReflection;
}

void CGameObject::ReleaseUploadBuffers()
{
	////정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pMesh) 
		m_pMesh->ReleaseUploadBuffers();
}

//객체의 정보를 저장하기 위한 리소스를 생성하고 리소스에 대한 포인터를 가져온다. 
void CGameObject::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&m_pMapped_object_info);
}

//객체의 월드변환 행렬과 재질 번호를 상수 버퍼에 쓴다. 
void CGameObject::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)m_pMapped_object_info;
	::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
	pbMappedcbGameObject->m_nMaterial = m_pMaterial->m_nReflection;

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();

	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress);
}

void CGameObject::Release_Shader_Resource()
{
	if (m_pConstant_Buffer)
	{
		m_pConstant_Buffer->Unmap(0, NULL);
		m_pConstant_Buffer->Release();
	}
}



void CGameObject::Animate(float fTimeElapsed)
{
	if (m_fMovingSpeed != 0.0f)
		Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);

	UpdateBoundingBox();
}

void CGameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->m_xmBoundingBox.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::UpdateFriction(float fTimeElapsed)
{ 
	if (m_fMovingSpeed > 0.0f)
	{
		m_fMovingSpeed -= m_fFriction * m_fMovingSpeed * fTimeElapsed;

		if (m_fMovingSpeed < 0.1f)
			m_fMovingSpeed = 0.0f;
	}
}

XMFLOAT3 CGameObject::ApplyTransform(XMFLOAT3 xmfloat3, XMFLOAT4X4 xmfloat4x4)
{
	XMVECTOR temp_pos = XMVectorSet(xmfloat3.x, xmfloat3.y, xmfloat3.z, 1.0f);

	XMMATRIX worldMatrix = XMLoadFloat4x4(&xmfloat4x4);
	temp_pos = XMVector3Transform(temp_pos, worldMatrix);

	XMFLOAT3 now_pos;
	XMStoreFloat3(&now_pos, temp_pos);

	return now_pos;
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	Update_Shader_Resource(pd3dCommandList);
	if (m_pMesh)
		m_pMesh->Render(pd3dCommandList);
}


void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}
//게임 객체의 로컬 z-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,m_xmf4x4World._33)));
}
//게임 객체의 로컬 y-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}
//게임 객체의 로컬 x-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}
//게임 객체를 로컬 x-축 방향으로 이동한다.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 y-축 방향으로 이동한다.
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 z-축 방향으로 이동한다.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}


void CGameObject::SetMovingDirection_Reverse()
{
	m_xmf3MovingDirection.x *= -1.0f;
	m_xmf3MovingDirection.y *= -1.0f;
	m_xmf3MovingDirection.z *= -1.0f;
}

//게임 객체를 주어진 각도로 회전한다.
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookToLH(GetPosition(), xmf3LookTo, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}



bool CGameObject::IsVisible(CCamera* pCamera)
{
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();

	//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다. 
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) 
		bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&
	xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//카메라 좌표계의 원점을 모델 좌표계로 변환한다. 
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다. 
	*pxmf3PickRayDirection= Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//광선의 방향 벡터를 구한다. 
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}

int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//모델 좌표계의 광선을 생성한다. 
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, 
		&xmf3PickRayDirection);
		//모델 좌표계의 광선과 메쉬의 교차를 검사한다. 
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, 
		xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

/// //////////////////////////////////////////////////////////////////////////////////////////////////

CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_fRotationSpeed = 360.0f;
}
CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Rotate_to_Player(float fElapsedTime, XMFLOAT3& xmf3LookTo)
{
	LookAt(xmf3LookTo, GetLook());
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	if (m_fRotationSpeed != 0.0f)
		Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);

	CGameObject::Animate(fTimeElapsed);

}

/////////////////////////////////////////////////////////////////////////////////////////

CExplosiveObject::CExplosiveObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

}

CExplosiveObject::~CExplosiveObject()
{
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (m_bBlowingUp)
	{
		if (true)
			return;
		else
		{
			m_bBlowingUp = false;
			m_bActive = false;
		}
	}
	else
	{
		CRotatingObject::Animate(fElapsedTime);
	}
}

void CExplosiveObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bBlowingUp)
	{
		//Render_Particle(pd3dCommandList, pCamera);
	}
	else
	{	
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

XMFLOAT3 CParticle::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh* CParticle::m_ExplosionMesh = NULL;

CParticle::CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_fDuration = 50.5f;
	m_fExplosionSpeed = 50.0f;

	Prepare_Particle(pd3dDevice, pd3dCommandList);
	Create_Shader_Resource(pd3dDevice, pd3dCommandList);

}

CParticle::~CParticle()
{
}

void CParticle::Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < EXPLOSION_DEBRISES; ++i)
		XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	m_ExplosionMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 5.5f, 5.5f, 5.5f);
}
 
void CParticle::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * EXPLOSION_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);
}

void CParticle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	XMFLOAT4X4 xmf4x4World;
	for (int i = 0; i < EXPLOSION_DEBRISES; ++i)
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4Transforms[i])));
		
		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)(particles_info + (i * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
		pbMappedcbGameObject->m_nMaterial = m_pMaterial->m_nReflection;
	}
}

void CParticle::Release_Shader_Resource()
{
	if (m_pConstant_Buffer)
	{
		m_pConstant_Buffer->Unmap(0, NULL);
		m_pConstant_Buffer->Release();
	}
}

void CParticle::Animate(float fElapsedTime)
{
	m_fElapsedTimes += fElapsedTime;
	if (m_fElapsedTimes <= m_fDuration)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
			m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
			m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
			m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
			m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
		}
	}
	else
	{
		m_fElapsedTimes = 0.0f;
	}
}

void CParticle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//IsVisible(pCamera)
	if (true)
	{
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();
		
		if (m_ExplosionMesh)
		{
			for (int j = 0; j < EXPLOSION_DEBRISES; j++)
			{
				pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * j));

				m_ExplosionMesh->Render(pd3dCommandList);

			}
		}			
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CBoardObject::CBoardObject()
{
	
}

CBoardObject::~CBoardObject()
{
}

void CBoardObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////

StoneObject::StoneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{	
}

StoneObject::~StoneObject()
{
}

void StoneObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CExplosiveObject::Render(pd3dCommandList, pCamera);
}

void StoneObject::Animate(float fElapsedTime)
{
	CExplosiveObject::Animate(fElapsedTime);

	XMVECTOR center = XMLoadFloat3(&m_pMesh->m_xmBoundingSphere.Center);
	XMVECTOR transformedCenter = XMVector3Transform(center, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat3(&m_xmOOSP.Center, transformedCenter);

	// 시작할때 한번만 하면 되는데 어디에 넣지
	m_xmOOSP.Radius = m_pMesh->m_xmBoundingSphere.Radius;
}



////////////////////////////////////////////////////////////////////////////////////////////////
