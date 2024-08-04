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
	Material_Colors = new CMaterialColors{
		XMFLOAT4(0.0f,0.0f,0.0f,0.0f),
		XMFLOAT4(0.0f,0.0f,0.0f,0.0f),
		XMFLOAT4(0.0f,0.0f,0.0f,0.0f),
		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
	};
}

CMaterial::CMaterial(CMaterialColors* M_C)
{
	Material_Colors = new CMaterialColors;
	Material_Colors->m_xmf4Ambient = M_C->m_xmf4Ambient;
	Material_Colors->m_xmf4Diffuse = M_C->m_xmf4Diffuse;
	Material_Colors->m_xmf4Specular = M_C->m_xmf4Specular;
	Material_Colors->m_xmf4Emissive = M_C->m_xmf4Emissive;
}

CMaterial::~CMaterial()
{
	if (material_shader)
		material_shader->Release();

	if (Material_Colors)
		Material_Colors->Release();
}

void CMaterial::SetShader(CShader* pShader)
{
	if (material_shader)
		material_shader->Release();

	material_shader = pShader;

	if (material_shader)
		material_shader->AddRef();
}

void CMaterial::SetMaterialColors(CMaterialColors* pMaterialColors)
{
	if (Material_Colors)
		Material_Colors->Release();

	Material_Colors = pMaterialColors;

	if (Material_Colors)
		Material_Colors->AddRef();
}

//=================================================================================


CGameObject::CGameObject()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();


}

CGameObject::~CGameObject()
{
	if (m_pMesh)
		m_pMesh->Release();


	for (int i = 0; i < m_ppMaterials.size(); ++i)
	{
		if (m_ppMaterials[i].first)
			m_ppMaterials[i].first->Release();
	}
	if (m_ppMaterials.size())
		m_ppMaterials.clear();
}

void CGameObject::AddRef()
{
	m_nReferences++;

	if (m_pSibling) 
		m_pSibling->AddRef();
	if (m_pChild) 
		m_pChild->AddRef();
}

void CGameObject::Release()
{
	if (m_pChild)
		m_pChild->Release();
	if (m_pSibling) 
		m_pSibling->Release();

	if (--m_nReferences <= 0) 
		delete this;
}

void CGameObject::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) 
			pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) 
			pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh)
		m_pMesh->Release();

	m_pMesh = pMesh;

	if (m_pMesh)
		m_pMesh->AddRef();
}


void CGameObject::Set_MaterialShader(CShader* pShader, int nMaterial)
{
	if (0 <= nMaterial && nMaterial < m_ppMaterials.size())
	{
		if (m_ppMaterials[nMaterial].first)
			m_ppMaterials[nMaterial].first->SetShader(pShader);
	}
	else
		DebugOutput("SetShader :: Wrong Index for Material");
}


void CGameObject::SetMaterial(CMaterial* pMaterial)
{
	bool is_exist = false;
	for (std::pair<CMaterial*, bool>& material : m_ppMaterials)
	{	
		if (material.first == pMaterial)
		{
			material.second = true;
			is_exist = true;
		}
		else
			material.second = false;
	}
	
	if (!is_exist)
	{
		m_ppMaterials.push_back(std::make_pair(pMaterial, true));
		pMaterial->AddRef();
	}
}

void CGameObject::AddMaterial(CMaterial* pMaterial) // 객체에 연결할 새 재질의 포인터를 사용 안함 상태로 저장하고, 참조 카운트 증가시킴
{
	m_ppMaterials.push_back(std::make_pair(pMaterial, false));
	m_ppMaterials.back().first->AddRef();

}

void CGameObject::ChangeMaterial(UINT n)
{
	if (n >= m_ppMaterials.size())
		return;

	for (std::pair<CMaterial*, bool>& material : m_ppMaterials)
		material.second = false;

	m_ppMaterials[n].second = true;

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
	Create_Object_Buffer(pd3dDevice, pd3dCommandList);
	Create_Material_Buffer(pd3dDevice, pd3dCommandList);
}
void CGameObject::Create_Object_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 게임 객체 버퍼 생성 및 매핑
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	Object_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	Object_Constant_Buffer->Map(0, NULL, (void**)&Mapped_Object_info);
}
void CGameObject::Create_Material_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 게임 재질 버퍼 생성 및 매핑
	UINT ncbElementBytes = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255); //256의 배수
	Material_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	Material_Constant_Buffer->Map(0, NULL, (void**)&Mapped_Material_info);
}


//상수 버퍼에 쓰여진 내용을 셰이더에 전달
void CGameObject::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList, Resource_Buffer_Type type, int N)
{
	// 현재 게임 객체 정보 상수 버퍼를 바인딩
	switch (type)
	{
	case Resource_Buffer_Type::GameObject_info:
	{
		Update_Object_Buffer();
		// 사실 커멘드 리스트마다 한번만 바인딩 하면 됨 == 사실 프레임마다 버퍼는 바인딩 한번만 하면 됨
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = Object_Constant_Buffer->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress);
	}
	break;

	case Resource_Buffer_Type::Material_info:
	{
		Update_Material_Buffer(N);

		// 현재 게임 재질 정보 상수 버퍼를 바인딩
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialGpuVirtualAddress = Material_Constant_Buffer->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialGpuVirtualAddress);
	}
	break;

	default:
		DebugOutput("Update_Shader_Resource :: Accessing Wrond Buffer Type");
		break;
	}
}

void CGameObject::Update_Object_Buffer()
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)Mapped_Object_info;
	::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
}

void CGameObject::Update_Material_Buffer(int N)
{
	if (m_ppMaterials.size() == 0) {
		DebugOutput("Update_Material_Buffer :: Object has no material");
		return;
	}
	else if (N >= m_ppMaterials.size())
	{
		DebugOutput("Update_Material_Buffer :: Wrong Index");
		return;
	}

	CMaterialColors* colors = m_ppMaterials[N].first->Material_Colors;

	CB_MATERIAL_INFO* pbMappedMaterial = (CB_MATERIAL_INFO*)Mapped_Material_info;
	::memcpy(&pbMappedMaterial->m_cAmbient, &colors->m_xmf4Ambient, sizeof(XMFLOAT4));
	::memcpy(&pbMappedMaterial->m_cDiffuse, &colors->m_xmf4Diffuse, sizeof(XMFLOAT4));
	::memcpy(&pbMappedMaterial->m_cSpecular, &colors->m_xmf4Specular, sizeof(XMFLOAT4));
	::memcpy(&pbMappedMaterial->m_cEmissive, &colors->m_xmf4Emissive, sizeof(XMFLOAT4));

}

void CGameObject::Release_Shader_Resource()
{
	if (Object_Constant_Buffer)
	{
		Object_Constant_Buffer->Unmap(0, NULL);
		Object_Constant_Buffer->Release();
	}

	if (Material_Constant_Buffer)
	{
		Material_Constant_Buffer->Unmap(0, NULL);
		Material_Constant_Buffer->Release();
	}
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (m_fMovingSpeed != 0.0f)
		Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);

	UpdateFriction(fTimeElapsed);
	UpdateBoundingBox();
}

// FLOAT3 백터를 XMFLOAT4X4 행렬에 적용한 값을 반환
XMFLOAT3 CGameObject::ApplyTransform(XMFLOAT3 xmfloat3, XMFLOAT4X4 xmfloat4x4)
{
	XMVECTOR temp_pos = XMVectorSet(xmfloat3.x, xmfloat3.y, xmfloat3.z, 1.0f);

	XMMATRIX worldMatrix = XMLoadFloat4x4(&xmfloat4x4);
	temp_pos = XMVector3Transform(temp_pos, worldMatrix);

	XMFLOAT3 now_pos;
	XMStoreFloat3(&now_pos, temp_pos);

	return now_pos;
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	if (pxmf4x4Parent) 
		m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent);
	else 
		m_xmf4x4World = m_xmf4x4Transform;

	if (m_pSibling)
		m_pSibling->UpdateTransform(pxmf4x4Parent);

	if (m_pChild)
		m_pChild->UpdateTransform(&m_xmf4x4World);
}

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if (!strncmp(FrameName, pstrFrameName, strlen(pstrFrameName))) 
		return(this);

	if (m_pSibling) 
		if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) 
			return(pFrameObject);

	if (m_pChild) 
		if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) 
			return(pFrameObject);

	return(NULL);
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	if (!active)
		return;

	bool shader_changed = false;
	
	// 객체 정보 컨테이너 업데이트 :: Mapped_Object_info
	Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::GameObject_info);
	for (int i = 0; i < m_ppMaterials.size(); ++i)
	{
		if (m_ppMaterials[i].second)
		{
			if (m_ppMaterials[i].first->material_shader)
			{
				m_ppMaterials[i].first->material_shader->Setting_Render(pd3dCommandList);
				shader_changed = true;
			}
			// 재질 정보 컨테이너 업데이트 :: Mapped_Material_info
			Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::Material_info, i);
			
			if (m_pMesh)
				m_pMesh->Render(pd3dCommandList);
		}
		if(shader_changed)
			pShader->Setting_Render(pd3dCommandList);
	}

	
	if (m_pSibling) 
		m_pSibling->Render(pd3dCommandList, pCamera, pShader);

	if (m_pChild)
		m_pChild->Render(pd3dCommandList, pCamera, pShader);

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
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
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

////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

CBoardObject::CBoardObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::Create_Shader_Resource(pd3dDevice, pd3dCommandList);
}

CBoardObject::~CBoardObject()
{
}

void CBoardObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	CGameObject::Render(pd3dCommandList, pCamera, pShader);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////

StoneObject::StoneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{	
	CGameObject::Create_Shader_Resource(pd3dDevice, pd3dCommandList);
}

StoneObject::~StoneObject()
{
}

void StoneObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	CGameObject::Render(pd3dCommandList, pCamera, pShader);
}

void StoneObject::Animate(float fElapsedTime)
{
	CRotatingObject::Animate(fElapsedTime);

	XMVECTOR center = XMLoadFloat3(&m_pMesh->m_xmBoundingSphere.Center);
	XMVECTOR transformedCenter = XMVector3Transform(center, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat3(&m_xmOOSP.Center, transformedCenter);

	// 시작할때 한번만 하면 되는데 어디에 넣지
	m_xmOOSP.Radius = m_pMesh->m_xmBoundingSphere.Radius;
}



////////////////////////////////////////////////////////////////////////////////////////////////
