#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"

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


CGameObject::CGameObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
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

	for (CGameObject* child_ptr : m_pChild)
		child_ptr->AddRef();

	for (CGameObject* sibling_ptr : m_pSibling)
		sibling_ptr->AddRef();

}

void CGameObject::Release()
{
	for (CGameObject* child_ptr : m_pChild)
		delete child_ptr;

	for (CGameObject* sibling_ptr : m_pSibling)
		delete sibling_ptr;

	if (--m_nReferences <= 0) 
		delete this;
}

void CGameObject::Add_Child(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;

		if (bReferenceUpdate) 
			pChild->AddRef();
	}
	m_pChild.push_back(pChild);
}

void CGameObject::Add_Sibling(CGameObject* pSibling, bool bReferenceUpdate)
{
	if (pSibling)
	{
		pSibling->m_pParent = this->GetParent();

		if (bReferenceUpdate)
			pSibling->AddRef();
	}
	m_pSibling.push_back(pSibling);
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

void CGameObject::AddMaterial(CMaterial* pMaterial) // ��ü�� ������ �� ������ �����͸� ��� ���� ���·� �����ϰ�, ���� ī��Ʈ ������Ŵ
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
	////���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
	if (m_pMesh) 
		m_pMesh->ReleaseUploadBuffers();
}

//��ü�� ������ �����ϱ� ���� ���ҽ��� �����ϰ� ���ҽ��� ���� �����͸� �����´�. 
void CGameObject::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	Create_Object_Buffer(pd3dDevice, pd3dCommandList);
	Create_Material_Buffer(pd3dDevice, pd3dCommandList);
}
void CGameObject::Create_Object_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ���� ��ü ���� ���� �� ����
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	Object_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	Object_Constant_Buffer->Map(0, NULL, (void**)&Mapped_Object_info);
}
void CGameObject::Create_Material_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ���� ���� ���� ���� �� ����
	UINT ncbElementBytes = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255); //256�� ���
	Material_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	Material_Constant_Buffer->Map(0, NULL, (void**)&Mapped_Material_info);
}


//��� ���ۿ� ������ ������ ���̴��� ����
void CGameObject::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList, Resource_Buffer_Type type, int N)
{
	// ���� ���� ��ü ���� ��� ���۸� ���ε�
	switch (type)
	{
	case Resource_Buffer_Type::GameObject_info:
	{
		Update_Object_Buffer();
		// ��� Ŀ��� ����Ʈ���� �ѹ��� ���ε� �ϸ� �� == ��� �����Ӹ��� ���۴� ���ε� �ѹ��� �ϸ� ��
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = Object_Constant_Buffer->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress);
	}
	break;

	case Resource_Buffer_Type::Material_info:
	{
		Update_Material_Buffer(N);

		// ���� ���� ���� ���� ��� ���۸� ���ε�
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

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if (!strncmp(FrameName, pstrFrameName, strlen(pstrFrameName)))
		return(this);

	for (CGameObject* sibling_ptr : m_pSibling)
	{
		if (sibling_ptr)
			if (pFrameObject = sibling_ptr->FindFrame(pstrFrameName))
				return(pFrameObject);
	}

	for (CGameObject* child_ptr : m_pChild)
	{
		if (child_ptr)
			if (pFrameObject = child_ptr->FindFrame(pstrFrameName))
				return(pFrameObject);
	}

	return(NULL);
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	for (CGameObject* sibling_ptr : m_pSibling)
		sibling_ptr->Animate(fTimeElapsed, pxmf4x4Parent);

	for (CGameObject* child_ptr : m_pChild)
		child_ptr->Animate(fTimeElapsed, &m_xmf4x4World);

	UpdateBoundingBox();
}


void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	pd3dCommandList->SetGraphicsRootSignature(Object_GraphicsRootSignature_ptr);
	if (!active)
		return;

	bool shader_changed = false;
	
	// ��ü ���� �����̳� ������Ʈ :: Mapped_Object_info
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
			// ���� ���� �����̳� ������Ʈ :: Mapped_Material_info
			Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::Material_info, i);
			
			if (m_pMesh)
				m_pMesh->Render(pd3dCommandList);
		}
		if(shader_changed)
			pShader->Setting_Render(pd3dCommandList);
	}

	for (CGameObject* sibling_ptr : m_pSibling)
		sibling_ptr->Render(pd3dCommandList, pCamera, pShader);

	for (CGameObject* child_ptr : m_pChild)
		child_ptr->Render(pd3dCommandList, pCamera, pShader);
}

// FLOAT3 ���͸� XMFLOAT4X4 ��Ŀ� ������ ���� ��ȯ
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

	for (CGameObject* sibling_ptr : m_pSibling)
		sibling_ptr->UpdateTransform(pxmf4x4Parent);

	for (CGameObject* child_ptr : m_pChild)
		child_ptr->UpdateTransform(&m_xmf4x4World);

	UpdateBoundingBox();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}


XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}
//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,m_xmf4x4World._33)));
}
//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}
//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�.
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}
//���� ��ü�� ���� x-�� �������� �̵��Ѵ�.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� y-�� �������� �̵��Ѵ�.
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� z-�� �������� �̵��Ѵ�.
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
	// ���Ŀ� ���� ������ ���� ������Ʈ �� �ʿ� ����
	if (m_pMesh)
	{
		m_pMesh->m_xmBoundingBox.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}

	//for (CGameObject* sibling_ptr : m_pSibling)
	//	sibling_ptr->UpdateBoundingBox();

	//for (CGameObject* child_ptr : m_pChild)
	//	child_ptr->UpdateBoundingBox();
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


//===========================================================================================================
//���� ��ü�� �־��� ������ ȸ���Ѵ�.


void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

//===========================================================================================================



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

	//�� ��ǥ���� �ٿ�� �ڽ��� ���� ��ǥ��� ��ȯ�Ѵ�. 
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) 
		bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
}

void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&xmf4x4View, 
	XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//ī�޶� ��ǥ���� ������ �� ��ǥ��� ��ȯ�Ѵ�. 
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//ī�޶� ��ǥ���� ��(���콺 ��ǥ�� ����ȯ�Ͽ� ���� ��)�� �� ��ǥ��� ��ȯ�Ѵ�. 
	*pxmf3PickRayDirection= Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//������ ���� ���͸� ���Ѵ�. 
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}


int CGameObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	float currentHitDistance = FLT_MAX;  

	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;

		// �� ��ǥ���� ������ �����Ѵ�. 
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);

		//DebugOutput("p_x: " + std::to_string(xmf3PickPosition.x) + "\t p_y: " + std::to_string(xmf3PickPosition.y) + "\t p_z: " + std::to_string(xmf3PickPosition.z));
		//DebugOutput("ray_x: " + std::to_string(xmf3PickRayOrigin.x) + "\t ray_y: " + std::to_string(xmf3PickRayOrigin.y) + "\t ray_z: " + std::to_string(xmf3PickRayOrigin.z));
		//DebugOutput("dir_x: " + std::to_string(xmf3PickRayDirection.x) + "\t dir_y: " + std::to_string(xmf3PickRayDirection.y) + "\t dir_z: " + std::to_string(xmf3PickRayDirection.z));

		// �� ��ǥ���� ������ �޽��� ������ �˻��Ѵ�. 
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, &currentHitDistance);
		if (nIntersected != 0 && currentHitDistance < *pfHitDistance)
			*pfHitDistance = currentHitDistance;


	}

	// �ڽ� ��ü�� ��ȸ
	for (CGameObject* child_ptr : m_pChild)
	{
		int childIntersected = child_ptr->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &currentHitDistance);
		if (childIntersected > nIntersected && currentHitDistance < *pfHitDistance)
		{
			nIntersected = childIntersected;
			*pfHitDistance = currentHitDistance;
		}
	}
	// ���� ��ü�� ��ȸ
	for (CGameObject* sibling_ptr : m_pSibling)
	{
		int siblingIntersected = sibling_ptr->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &currentHitDistance);
		if (siblingIntersected > nIntersected && currentHitDistance < *pfHitDistance)
		{
			nIntersected = siblingIntersected;
			*pfHitDistance = currentHitDistance;
		}
	}


	return nIntersected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CRotatingObject::CRotatingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CGameObject(pd3dDevice, pd3dCommandList)
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

void CRotatingObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_fRotationSpeed != 0.0f)
		Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);

}

////////////////////////////////////////////////////////////////////////////////

CBoardObject::CBoardObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CRotatingObject(pd3dDevice, pd3dCommandList)
{
	SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	SetRotationSpeed(30.0f);
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
	: CRotatingObject(pd3dDevice, pd3dCommandList)
{	
}

StoneObject::~StoneObject()
{
}

void StoneObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	CGameObject::Render(pd3dCommandList, pCamera, pShader);
}

void StoneObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	// �� ��ü���� �ؾ��� �⺻ �ִϸ��̼� == �̵�
	if (m_fMovingSpeed != 0.0f)
		Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);

	// �̵��� ��ġ�� ������� ���� �浹ü ������Ʈ
	XMVECTOR center = XMLoadFloat3(&m_pMesh->m_xmBoundingSphere.Center);
	XMVECTOR transformedCenter = XMVector3Transform(center, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat3(&m_xmOOSP.Center, transformedCenter);

	// ������ ���� �� �浹ü ������Ʈ
	UpdateFriction(fTimeElapsed);
	UpdateBoundingBox();

	// �浹ü ���� ������ ������Ʈ
	// �����Ҷ� �ѹ��� �ϸ� �Ǵµ� ��� ����
	m_xmOOSP.Radius = m_pMesh->m_xmBoundingSphere.Radius;
	
	// �ڽ� ��ü�� �ִٸ�, �ش� ��ü�� ������Ʈ
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////
