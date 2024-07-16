#include "stdafx.h"
#include "Particle.h"

XMFLOAT3& Get_Random_Normalize_Direction()
{
	float randX = static_cast<float>(rand()) / RAND_MAX;
	float randY = static_cast<float>(rand()) / RAND_MAX;
	float randZ = static_cast<float>(rand()) / RAND_MAX;

	XMFLOAT3 random_D(randX, randY, randZ);

	XMVECTOR normalized_D = XMVector3Normalize(XMLoadFloat3(&random_D));

	XMStoreFloat3(&random_D, normalized_D);

	return random_D;
}

//==========================================================

Particle::Particle(ParticleType p_type)
{
	type = p_type;
	active = false;
}

Particle::~Particle()
{
}

void Particle::Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void Particle::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void Particle::Release_Shader_Resource()
{
}

void Particle::Animate(float fElapsedTime)
{
}

void Particle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}

void Particle::Reset()
{
	m_fElapsedTimes = 0.0f;
	active = false;
}

//===========================================================================

XMFLOAT3 Explosion_Particle::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh* Explosion_Particle::m_ExplosionMesh = NULL;
bool Explosion_Particle::Setting = false;

Explosion_Particle::Explosion_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT material, ParticleType p_type) : Particle(p_type)
{
	m_fDuration = 5.5f;
	m_fExplosionSpeed = 50.0f;

	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
	SetMaterial(material);
}

Explosion_Particle::~Explosion_Particle()
{
}

void Explosion_Particle::Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (Setting == false)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; ++i)
			XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());
		
		m_ExplosionMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 5.5f, 5.5f, 5.5f);
		Setting = true;
	}
}

void Explosion_Particle::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * EXPLOSION_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);
}

void Explosion_Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
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

void Explosion_Particle::Release_Shader_Resource()
{
	if (m_pConstant_Buffer)
	{
		m_pConstant_Buffer->Unmap(0, NULL);
		m_pConstant_Buffer->Release();
	}
}

void Explosion_Particle::Animate(float fElapsedTime)
{
	if (active)
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
			Reset();
	}
}

void Explosion_Particle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//IsVisible(pCamera)
	if (true && active)
	{
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();

		if (m_ExplosionMesh)
		{
			for (int j = 0; j < EXPLOSION_DEBRISES; ++j)
			{
				pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * j));

				m_ExplosionMesh->Render(pd3dCommandList);

			}
		}
	}
}