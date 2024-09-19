#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"

//=================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) 
			m_ppd3dTextureUploadBuffers[i] = m_ppd3dTextures[i] = NULL;

		m_ppstrTextureNames = new _TCHAR[m_nTextures][64];

		for (int i = 0; i < m_nTextures; i++) 
			m_ppstrTextureNames[i][0] = '\0';

		m_pd3dSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];

		for (int i = 0; i < m_nTextures; i++) 
			m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes = new UINT[m_nTextures];
		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTextures];
		m_pnBufferElements = new int[m_nTextures];
	}
	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pnRootParameterIndices = new int[nRootParameters];
	for (int i = 0; i < m_nRootParameters; i++) m_pnRootParameterIndices[i] = -1;

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}

	if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;

	if (m_pnResourceTypes) delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) delete[] m_pnBufferElements;

	if (m_pnRootParameterIndices) delete[] m_pnRootParameterIndices;
	if (m_pd3dSrvGpuDescriptorHandles) delete[] m_pd3dSrvGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootParameterIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pnRootParameterIndices[nIndex] = nRootParameterIndex;
}

void CTexture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_nRootParameters == m_nTextures)
	{
		for (int i = 0; i < m_nRootParameters; i++)
		{
			if (m_pd3dSrvGpuDescriptorHandles[i].ptr && (m_pnRootParameterIndices[i] != -1)) 
				pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[i], m_pd3dSrvGpuDescriptorHandles[i]);
		}
	}
	else
	{
		if (m_pd3dSrvGpuDescriptorHandles[0].ptr) 
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[0], m_pd3dSrvGpuDescriptorHandles[0]);
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = 1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

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

void CMaterial::SetTexture(CTexture* pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
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


CGameObject::CGameObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nMeshes)
{
	mesh_list.resize(nMeshes);
	std::transform(mesh_list.begin(), mesh_list.end(), mesh_list.begin(), [](CMesh* p_mesh) {p_mesh = NULL; return p_mesh; });

	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
}

CGameObject::~CGameObject()
{
	m_pSibling.clear();
	m_pChild.clear();

	for (CMesh* p_mesh : mesh_list)
	{
		if (p_mesh)
			p_mesh->Release();
		p_mesh = NULL;
	}
	mesh_list.clear();

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
		child_ptr->Release();

	for (CGameObject* sibling_ptr : m_pSibling)
		sibling_ptr->Release();

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

void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (nIndex < mesh_list.size())
	{
		if (mesh_list[nIndex])
			mesh_list[nIndex]->Release();
		
		mesh_list[nIndex] = pMesh;
		
		if (pMesh)
			pMesh->AddRef();
	}
}
void CGameObject::AddMesh(CMesh* pMesh)
{

	mesh_list.push_back(pMesh);
	
	if (pMesh)
		pMesh->AddRef();
	
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


void CGameObject::SetMaterial(CMaterial* pMaterial, bool front_insert)
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
		if(front_insert)
			m_ppMaterials.insert(m_ppMaterials.begin(), std::make_pair(pMaterial, true));
		else
			m_ppMaterials.push_back(std::make_pair(pMaterial, true));
		pMaterial->AddRef();
	}
}

void CGameObject::AddMaterial(CMaterial* pMaterial, bool active) // 객체에 연결할 새 재질의 포인터를 사용 안함 상태로 저장하고, 참조 카운트 증가시킴
{
	m_ppMaterials.push_back(std::make_pair(pMaterial, active));
	m_ppMaterials.back().first->AddRef();

}

void CGameObject::ChangeMaterial(UINT n)
{
	if (n >= m_ppMaterials.size())
		return;

	for (std::pair<CMaterial*, bool>& material : m_ppMaterials)
	{
		if (material.first->material_shader != NULL && material.first->material_shader->type == Shader_Type::Outline)
			continue;
		material.second = false;
	}
	m_ppMaterials[n].second = true;

}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	for (CMesh* p_mesh : mesh_list)
		if (p_mesh)
			p_mesh->ReleaseUploadBuffers();
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
		Update_Material_Buffer(pd3dCommandList, N);

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

void CGameObject::Update_Material_Buffer(ID3D12GraphicsCommandList* pd3dCommandList, int N)
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

	//UpdateBoundingBox();
}


void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
//	pd3dCommandList->SetGraphicsRootSignature(Object_GraphicsRootSignature_ptr);

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
				m_ppMaterials[i].first->material_shader->Setting_Render(pd3dCommandList, o_type, used_item);
				shader_changed = true;

				// 재질에 텍스쳐가 연결되어 있다면 활용
				if (m_ppMaterials[i].first->m_pTexture)
					m_ppMaterials[i].first->m_pTexture->UpdateShaderVariables(pd3dCommandList);

			}
			else // Ghost는 선으로 그리기
				pShader->Setting_Render(pd3dCommandList, o_type, used_item);

			// 재질 정보 컨테이너 업데이트 :: Mapped_Material_info
			Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::Material_info, i);

			if (mesh_list.size())
			{
				// 메쉬 마다 절두체 컬링 필요 -> 메쉬마다 메쉬 충돌체의 모델 좌표계가 달라야 함 -> 메쉬 생성시 변환 필요
				// 그 다음에 해당 과정 추가 할 것
				for (CMesh* p_mesh : mesh_list)
					if (p_mesh)
						p_mesh->Render(pd3dCommandList);
			}
		}

		if (shader_changed)
			pShader->Setting_Render(pd3dCommandList);
	}

	for (CGameObject* sibling_ptr : m_pSibling)
		sibling_ptr->Render(pd3dCommandList, pCamera, pShader);

	for (CGameObject* child_ptr : m_pChild)
		child_ptr->Render(pd3dCommandList, pCamera, pShader);
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

	for (CGameObject* sibling_ptr : m_pSibling)
		sibling_ptr->UpdateTransform(pxmf4x4Parent);

	for (CGameObject* child_ptr : m_pChild)
		child_ptr->UpdateTransform(&m_xmf4x4World);
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
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
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

BoundingOrientedBox CGameObject::Get_Collider()
{
	BoundingOrientedBox xmBoundingBox;
	default_collider.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&xmBoundingBox.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmBoundingBox.Orientation)));

	return(xmBoundingBox);
}


bool CGameObject::Is_Visible_Collider(CCamera* pCamera)
{
	bool bIsVisible = true;

	// 그릴 메쉬가 있는 객체인가?
	if (mesh_list.size())
	{
		// 메쉬가 여러개 이더라도, 게임 객체는 충돌체 1개임
		BoundingOrientedBox xmBoundingOrientedBox = Get_Collider();

		if (pCamera)
			bIsVisible = pCamera->IsInFrustum(xmBoundingOrientedBox);
	}

	return(bIsVisible);
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
//게임 객체를 주어진 각도로 회전한다.


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

//bool CGameObject::IsVisible(CCamera* pCamera)
//{
//	bool bIsVisible = false;
//	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();
//
//	//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다. 
//	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
//	if (pCamera) 
//		bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
//	return(bIsVisible);
//}

void CGameObject::Generate_Ray_For_Picking_Projection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&xmf4x4View,
	XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
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

void CGameObject::Generate_Ray_For_Picking_Orthographic( XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT4X4& xmf4x4Projection,
	XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	// 1. [-x, x], [-y, y] 좌표계를 클립 좌표로 변환 (직교 투영에서는 Z는 항상 0)
	float screen_ratio = xmf4x4Projection._11 / xmf4x4Projection._22;
	XMVECTOR ndcPoint = XMVectorSet(xmf3PickPosition.x * screen_ratio, xmf3PickPosition.y, 0.0f, 1.0f);

	// 2. 클립 좌표를 뷰 공간으로 변환
	XMMATRIX projMatrix = XMLoadFloat4x4(&xmf4x4Projection); // 직교 투영 행렬
	XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projMatrix);
	XMVECTOR clipPoint = XMVector4Transform(ndcPoint, invProjMatrix);

	// 3. 클립 좌표를 뷰 공간으로 변환
	XMMATRIX viewMatrix = XMLoadFloat4x4(&xmf4x4View);
	XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	XMVECTOR viewSpacePoint = XMVector4Transform(clipPoint, invViewMatrix);

	// 4. 뷰 공간 좌표의 w를 나눠서 정규화
	float w = XMVectorGetW(viewSpacePoint);
	if (w != 0.0f) 
		viewSpacePoint = XMVectorDivide(viewSpacePoint, XMVectorSet(w, w, w, 1.0f));

	XMFLOAT3 nearPlanePoint;
	XMStoreFloat3(&nearPlanePoint, viewSpacePoint);

	// 5. 뷰 공간 좌표를 월드 공간으로 변환
	XMMATRIX worldMatrix = XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);
	XMVECTOR worldSpacePoint = XMVector4Transform(XMVectorSet(nearPlanePoint.x, nearPlanePoint.y, nearPlanePoint.z, 1.0f), invWorldMatrix);
	XMFLOAT3 nearPlaneWorldPoint;
	XMStoreFloat3(&nearPlaneWorldPoint, worldSpacePoint);

	// 6. 광선의 시작점 (Near 평면에서의 월드 좌표)
	*pxmf3PickRayOrigin = nearPlaneWorldPoint;

	// 7. 광선의 방향 벡터 설정
	XMFLOAT3 cameraDirection(0.0f, 0.0f, 1.0f); // 직교 투영에서는 카메라의 시선 방향이 항상 (0, 0, 1)
	*pxmf3PickRayDirection = cameraDirection; // 광선 방향은 카메라의 시선 방향과 동일
}


int CGameObject::Pick_Object_By_Ray_Intersection_Orthographic(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT4X4& xmf4x4Projection, float* pfHitDistance)
{
	int nIntersected = 0;
	float currentHitDistance = FLT_MAX;

	for(CMesh* p_mesh : mesh_list)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;

		// 모델 좌표계의 광선을 생성한다. 
		Generate_Ray_For_Picking_Orthographic(xmf3PickPosition, xmf4x4View, xmf4x4Projection, &xmf3PickRayOrigin, &xmf3PickRayDirection);

//		DebugOutput("obj_x: " + std::to_string(GetPosition().x) + "\t obj_y: " + std::to_string(GetPosition().y) + "\t obj_z: " + std::to_string(GetPosition().z));
//		DebugOutput("ray_x: " + std::to_string(xmf3PickRayOrigin.x) + "\t ray_y: " + std::to_string(xmf3PickRayOrigin.y) + "\t ray_z: " + std::to_string(xmf3PickRayOrigin.z));
//		DebugOutput("dir_x: " + std::to_string(xmf3PickRayDirection.x) + "\t dir_y: " + std::to_string(xmf3PickRayDirection.y) + "\t dir_z: " + std::to_string(xmf3PickRayDirection.z));

		// 모델 좌표계의 광선과 메쉬의 교차를 검사한다. 
		nIntersected = p_mesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, &currentHitDistance);
		if (nIntersected != 0 && currentHitDistance < *pfHitDistance)
		{
			*pfHitDistance = currentHitDistance;
		}

	}



	// 자식 객체들 순회
	for (CGameObject* child_ptr : m_pChild)
	{
		int childIntersected = child_ptr->Pick_Object_By_Ray_Intersection_Orthographic(xmf3PickPosition, xmf4x4View, xmf4x4Projection, &currentHitDistance);
		if (childIntersected > nIntersected && currentHitDistance < *pfHitDistance)
		{
			nIntersected = childIntersected;
			*pfHitDistance = currentHitDistance;
		}
	}
	// 형제 객체들 순회
	for (CGameObject* sibling_ptr : m_pSibling)
	{
		int siblingIntersected = sibling_ptr->Pick_Object_By_Ray_Intersection_Orthographic(xmf3PickPosition, xmf4x4View, xmf4x4Projection, &currentHitDistance);
		if (siblingIntersected > nIntersected && currentHitDistance < *pfHitDistance)
		{
			nIntersected = siblingIntersected;
			*pfHitDistance = currentHitDistance;
		}
	}


	return nIntersected;
}

int CGameObject::Pick_Object_By_Ray_Intersection_Projection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	float currentHitDistance = FLT_MAX;  

	for (CMesh* p_mesh : mesh_list)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;

		// 모델 좌표계의 광선을 생성한다. 
		Generate_Ray_For_Picking_Projection(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);

		//DebugOutput("obj_x: " + std::to_string(GetPosition().x) + "\t obj_y: " + std::to_string(GetPosition().y) + "\t obj_z: " + std::to_string(GetPosition().z));
		//DebugOutput("ray_x: " + std::to_string(xmf3PickRayOrigin.x) + "\t ray_y: " + std::to_string(xmf3PickRayOrigin.y) + "\t ray_z: " + std::to_string(xmf3PickRayOrigin.z));
		//DebugOutput("dir_x: " + std::to_string(xmf3PickRayDirection.x) + "\t dir_y: " + std::to_string(xmf3PickRayDirection.y) + "\t dir_z: " + std::to_string(xmf3PickRayDirection.z));

		// 모델 좌표계의 광선과 메쉬의 교차를 검사한다. 
		nIntersected = p_mesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, &currentHitDistance);
		if (nIntersected != 0 && currentHitDistance < *pfHitDistance)
		{
			*pfHitDistance = currentHitDistance;
		}
	}

	// 자식 객체들 순회
	for (CGameObject* child_ptr : m_pChild)
	{
		int childIntersected = child_ptr->Pick_Object_By_Ray_Intersection_Projection(xmf3PickPosition, xmf4x4View, &currentHitDistance);
		if (childIntersected > nIntersected && currentHitDistance < *pfHitDistance)
		{
			nIntersected = childIntersected;
			*pfHitDistance = currentHitDistance;
		}
	}

	// 형제 객체들 순회
	for (CGameObject* sibling_ptr : m_pSibling)
	{
		int siblingIntersected = sibling_ptr->Pick_Object_By_Ray_Intersection_Projection(xmf3PickPosition, xmf4x4View, &currentHitDistance);
		if (siblingIntersected > nIntersected && currentHitDistance < *pfHitDistance)
		{
			nIntersected = siblingIntersected;
			*pfHitDistance = currentHitDistance;
		}
	}


	return nIntersected;
}

void CGameObject::Apply_Item(Item_Type type)
{
	used_item = type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


CRotatingObject::CRotatingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nMeshes) : CGameObject(pd3dDevice, pd3dCommandList, nMeshes)
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
	o_type = Object_Type::Board;

	SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	SetRotationSpeed(30.0f);
}

CBoardObject::~CBoardObject()
{
}

void CBoardObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	if (true) // Is_Visible_Collider(pCamera)
	{
		CGameObject::Render(pd3dCommandList, pCamera, pShader);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

StoneObject::StoneObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) 
	: CRotatingObject(pd3dDevice, pd3dCommandList)
{	
	o_type = Object_Type::Stone;
}

StoneObject::~StoneObject()
{
}

void StoneObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	if (Is_Visible_Collider(pCamera))
	{
		CGameObject::Render(pd3dCommandList, pCamera, pShader);
	}
}

void StoneObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	switch (used_item)
	{
	case Item_Type::Double_Power:
	{
		if (m_fMovingSpeed != 0.0f)
			Move(m_xmf3MovingDirection, m_fMovingSpeed/5 * fTimeElapsed);
	}
	break;

	case Item_Type::Taunt:
	case Item_Type::Fire_Shot:
	case Item_Type::Frozen_Time:
	case Item_Type::Max_Power:
	case Item_Type::Ghost:
	case Item_Type::ETC:
	case Item_Type::None:
	default:
		if (m_fMovingSpeed != 0.0f)
			Move(m_xmf3MovingDirection, m_fMovingSpeed * fTimeElapsed);
		break;
	}

	// 마찰력 연산 및 충돌체 업데이트
	UpdateFriction(fTimeElapsed);
	//UpdateBoundingBox();

	// 자식 객체가 있다면, 해당 객체들 업데이트
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

BoundingSphere StoneObject::Get_Collider()
{
	BoundingSphere xmBoundingSphere;

	XMVECTOR center = XMLoadFloat3(&mesh_list[0]->m_xmBoundingSphere.Center); // 첫번째 메쉬의 모델 좌표계 충돌 구에서 가져옴
	XMVECTOR transformedCenter = XMVector3Transform(center, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat3(&xmBoundingSphere.Center, transformedCenter);

	xmBoundingSphere.Radius = mesh_list[0]->m_xmBoundingSphere.Radius;
	stone_collider = xmBoundingSphere;

	return(xmBoundingSphere);
}

////////////////////////////////////////////////////////////////////////////////////////////////

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, 
	int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, bool texture) : CGameObject(pd3dDevice, pd3dCommandList, 0)
{
	//지형에 사용할 높이 맵의 가로, 세로의 크기이다.
	m_nWidth = nWidth;
	m_nLength = nLength;
	/*지형 객체는 격자 메쉬들의 배열로 만들 것이다. nBlockWidth, nBlockLength는 격자 메쉬 하나의 가로, 세로 크
	기이다. cxQuadsPerBlock, czQuadsPerBlock은 격자 메쉬의 가로 방향과 세로 방향 사각형의 개수이다.*/
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다.
	m_xmf3Scale = xmf3Scale;
	//지형에 사용할 높이 맵을 생성한다.
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);
	//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다.
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	//지형 전체를 표현하기 위한 격자 메쉬의 개수이다.
	m_nMeshes = cxBlocks * czBlocks;
	
	mesh_list.resize(m_nMeshes);

	for (int i = 0; i < m_nMeshes; i++) 
		mesh_list[i] = NULL;


	CHeightMapGridMesh_Textured* pHeightMapGridMesh_T = NULL;
	CHeightMapGridMesh_Illuminated* pHeightMapGridMesh_I = NULL;
	
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//지형의 일부분을 나타내는 격자 메쉬의 시작 위치
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);

			//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장
			if (texture)
			{
				pHeightMapGridMesh_T = new CHeightMapGridMesh_Textured(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, m_pHeightMapImage);
				SetMesh(x + (z * cxBlocks), pHeightMapGridMesh_T);
			}
			else
			{
				pHeightMapGridMesh_I = new CHeightMapGridMesh_Illuminated(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, m_pHeightMapImage);
				SetMesh(x + (z * cxBlocks), pHeightMapGridMesh_I);
			}
		}
	}
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}