#include "stdafx.h"
#include "Particle.h"

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
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))
			return(XMVector3Normalize(v));
	}
}

XMVECTOR RotateVector(const XMVECTOR& vec, const XMVECTOR& axis, float angle)
{
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, angle);
	return XMVector3TransformNormal(vec, rotationMatrix);
}

// 0,1,0 ���Ͱ� -angle���� angle ������ ������ ������ Z���� �������� ȸ���ϰ�,
// ���� 0~360�� ������ ������ Y���� �������� �߰� ȸ���� ���͸� ��ȯ
XMVECTOR GetRandomRotatedVector(float angle)
{
	// ù ��° ȸ��: 0,1,0 ���͸� Z���� �������� -angle���� angle ������ ������ ������ ȸ��
	XMVECTOR initialVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	float randomAngleDegrees = RandF(-angle, angle);
	float randomAngleRadians = randomAngleDegrees * XM_PI / 180.0f; // ������ �������� ��ȯ
	XMVECTOR rotationAxisZ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // Z���� �������� ȸ��
	XMVECTOR rotatedVector = RotateVector(initialVector, rotationAxisZ, randomAngleRadians);

	// �� ��° ȸ��: ȸ���� ���͸� Y���� �������� 0~360�� ������ ������ ������ ȸ��
	float additionalRotationDegrees = RandF(0.0f, 360.0f);
	float additionalRotationRadians = additionalRotationDegrees * XM_PI / 180.0f; // ������ �������� ��ȯ
	XMVECTOR rotationAxisY = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Y���� �������� ȸ��
	XMVECTOR finalVector = RotateVector(rotatedVector, rotationAxisY, additionalRotationRadians);

	return finalVector;
}



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

Particle::Particle(Particle_Type p_type)
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

XMFLOAT3 Explosion_Particle::Explosion_Sphere_Vectors[EXPLOSION_DEBRISES];
CMesh* Explosion_Particle::m_ExplosionMesh = NULL;
bool Explosion_Particle::Setting = false;


Explosion_Particle::Explosion_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* material, Particle_Type p_type) : Particle(p_type)
{
	m_fDuration = 1.0f;
	m_fExplosionSpeed = 100.0f;

	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
	Create_Material_Buffer(pd3dDevice, pd3dCommandList);
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
			XMStoreFloat3(&Explosion_Sphere_Vectors[i], RandomUnitVectorOnSphere());
		
		m_ExplosionMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 5.5f, 5.5f, 5.5f);
		Setting = true;
	}
}

void Explosion_Particle::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * EXPLOSION_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);
}

void Explosion_Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	XMFLOAT4X4 xmf4x4World;
	for (int i = 0; i < EXPLOSION_DEBRISES; ++i)
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4Transforms[i])));

		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)(particles_info + (i * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
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
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + Explosion_Sphere_Vectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + Explosion_Sphere_Vectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + Explosion_Sphere_Vectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(Explosion_Sphere_Vectors[i], Particle_Rotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
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
		// ��ġ ���� ������Ʈ
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();

		// Ȱ��ȭ�� ������ ���� ������Ʈ
		for (int i = 0; i < m_ppMaterials.size(); ++i)
			if (m_ppMaterials[i].second == true)
				CGameObject::Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::Material_info, i);



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

//===========================================================================

XMFLOAT3 Charge_Particle::Charge_Sphere_Vectors[CHARGE_DEBRISES];
CMesh* Charge_Particle::m_ChargeMesh = NULL;
bool Charge_Particle::Setting = false;

Charge_Particle::Charge_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float Range, float cycle_time, CMaterial* material, Particle_Type p_type) : Particle(p_type)
{
	m_fDuration = cycle_time;
	Max_Range = Range;
	Particle_Rotation = 30.0f;

	Particle_Start_Position.resize(CHARGE_DEBRISES);
	Particle_ElapsedTime.resize(CHARGE_DEBRISES);
	Particle_Speed.resize(CHARGE_DEBRISES);
	

	for (int i = 0; i < CHARGE_DEBRISES; ++i)
	{
		Particle_Start_Position[i].x = Charge_Sphere_Vectors[i].x * Max_Range;
		Particle_Start_Position[i].y = Charge_Sphere_Vectors[i].y * Max_Range;
		Particle_Start_Position[i].z = Charge_Sphere_Vectors[i].z * Max_Range;

		Particle_Speed[i] = (rand() % 10 + 1) * 0.5f; // 0.5f ~ 5.0f
	}

	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
	Create_Material_Buffer(pd3dDevice, pd3dCommandList);

	SetMaterial(material);
}

Charge_Particle::~Charge_Particle()
{
}

void Charge_Particle::Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (Setting == false)
	{
		for (int i = 0; i < CHARGE_DEBRISES; ++i)
			XMStoreFloat3(&Charge_Sphere_Vectors[i], ::RandomUnitVectorOnSphere());

		m_ChargeMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 1.5f, 1.5f, 1.5f);
		Setting = true;
	}
}

void Charge_Particle::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * CHARGE_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);
}

void Charge_Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	XMFLOAT4X4 xmf4x4World;
	for (int i = 0; i < CHARGE_DEBRISES; ++i)
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4Transforms[i])));

		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)(particles_info + (i * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
	}
}

void Charge_Particle::Release_Shader_Resource()
{
	if (m_pConstant_Buffer)
	{
		m_pConstant_Buffer->Unmap(0, NULL);
		m_pConstant_Buffer->Release();
	}
}

void Charge_Particle::Set_Center_Position(XMFLOAT3 pos)
{
	for (int i = 0; i < CHARGE_DEBRISES; ++i)
	{
		Particle_Start_Position[i].x = pos.x + Charge_Sphere_Vectors[i].x * Max_Range;
		Particle_Start_Position[i].y = pos.y + Charge_Sphere_Vectors[i].y * Max_Range;
		Particle_Start_Position[i].z = pos.z + Charge_Sphere_Vectors[i].z * Max_Range;

	}
	SetPosition(pos);
}

void Charge_Particle::Animate(float fElapsedTime)
{
	if (active)
	{

		if (Active_Particle < CHARGE_DEBRISES)
			Active_Particle += 0.5;


		XMFLOAT3 xmf3Position = GetPosition();

		for (int i = 0; i < int(Active_Particle); i++)
		{
			// ��� �ð� ������Ʈ
			Particle_ElapsedTime[i] += fElapsedTime * Particle_Speed[i];

			// ��� �ð��� ���� ���� ��ġ���� �̵��ϵ��� ����
			float t = Particle_ElapsedTime[i] / m_fDuration;

			// ������ ������ ���ڴ� �ٽ� ���� ��ġ�� ���ư�
			if (t >= 1.0f)
			{
				t = 0.0f;
				Particle_ElapsedTime[i] = 0.0f;
				Particle_Speed[i] = (rand() % 10 + 1) * 0.5f;
			}

			m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
			m_pxmf4x4Transforms[i]._41 = Particle_Start_Position[i].x * (1.0f - t) + xmf3Position.x * t;
			m_pxmf4x4Transforms[i]._42 = Particle_Start_Position[i].y * (1.0f - t) + xmf3Position.y * t;
			m_pxmf4x4Transforms[i]._43 = Particle_Start_Position[i].z * (1.0f - t) + xmf3Position.z * t;

			// ȸ�� ����
			m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(Charge_Sphere_Vectors[i], Particle_Rotation * Particle_ElapsedTime[i]), m_pxmf4x4Transforms[i]);
		}
	}

}
void Charge_Particle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//IsVisible(pCamera)

	if (true && active)
	{
		// ��ġ ���� ������Ʈ
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();

		// Ȱ��ȭ�� ������ ���� ������Ʈ
		for (int i = 0; i < m_ppMaterials.size(); ++i)
			if (m_ppMaterials[i].second == true)
				CGameObject::Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::Material_info, i);

		if (m_ChargeMesh)
		{
			for (int j = 0; j < int(Active_Particle); ++j)
			{
				pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * j));

				m_ChargeMesh->Render(pd3dCommandList);

			}
		}
	}
}

void Charge_Particle::Reset()
{
	Active_Particle = 0;
	
	for (int i = 0; i < CHARGE_DEBRISES; ++i)
	{
		Particle_ElapsedTime[i] = 0.0f;

		m_pxmf4x4Transforms[i]._41 = Particle_Start_Position[i].x;
		m_pxmf4x4Transforms[i]._42 = Particle_Start_Position[i].y;
		m_pxmf4x4Transforms[i]._43 = Particle_Start_Position[i].z;
	}
	m_fElapsedTimes = 0.0f;
	active = false;
}

//===========================================================================================================


XMFLOAT3 Firework_Particle::Firework_Vectors[Firework_DEBRISES];
CMesh* Firework_Particle::m_FireworkMesh = NULL;
bool Firework_Particle::Setting = false;

Firework_Particle::Firework_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList
	, float cycle_time, CMaterial* material, Particle_Type p_type) : Particle(p_type)
{
	m_fDuration = cycle_time;
	Particle_Rotation = 300.0f;

	Reset();
	active = true;

	Create_Shader_Resource(pd3dDevice, pd3dCommandList);

	SetMaterial(material);
}

Firework_Particle::~Firework_Particle()
{
}

void Firework_Particle::Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (Setting == false)
	{
		for (int i = 0; i < Firework_DEBRISES; ++i)
			XMStoreFloat3(&Firework_Vectors[i], ::GetRandomRotatedVector(15.0f));

		m_FireworkMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 3.0f, 1.0f, 3.0f);
		Setting = true;
	}
}

void Firework_Particle::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes1 = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes1 * Firework_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);

	//-------------------------------------------------------------------

	// ���� ���� ���� ���� �� ����
	UINT ncbElementBytes2 = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255); //256�� ���
	firework_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes2 * Firework_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	firework_Constant_Buffer->Map(0, NULL, (void**)&firework_material_info);


}

void Firework_Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	UINT ncbMaterialBytes = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255);

	XMFLOAT4X4 xmf4x4World;
	for (int i = 0; i < Firework_DEBRISES; ++i)
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4Transforms[i])));

		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)(particles_info + (i * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));

		//-------------------------------------------------------------------


		int T = i % m_ppMaterials.size();
		CMaterialColors* colors = m_ppMaterials[T].first->Material_Colors;

		CB_MATERIAL_INFO* pbMappedMaterial = (CB_MATERIAL_INFO*)(firework_material_info + (i * ncbMaterialBytes));
		::memcpy(&pbMappedMaterial->m_cAmbient, &colors->m_xmf4Ambient, sizeof(XMFLOAT4));
		::memcpy(&pbMappedMaterial->m_cDiffuse, &colors->m_xmf4Diffuse, sizeof(XMFLOAT4));
		::memcpy(&pbMappedMaterial->m_cSpecular, &colors->m_xmf4Specular, sizeof(XMFLOAT4));
		::memcpy(&pbMappedMaterial->m_cEmissive, &colors->m_xmf4Emissive, sizeof(XMFLOAT4));
	}
}

void Firework_Particle::Release_Shader_Resource()
{
	if (m_pConstant_Buffer)
	{
		m_pConstant_Buffer->Unmap(0, NULL);
		m_pConstant_Buffer->Release();
	}
}

void Firework_Particle::Set_Main_Direction(XMFLOAT3 Direction)
{
	XMVECTOR defaultDirection = XMLoadFloat3(&Default_Direction);
	XMVECTOR targetDirection = XMLoadFloat3(&Direction);

	// ȸ�� ��� ������ ���
	XMVECTOR rotationAxis = XMVector3Cross(defaultDirection, targetDirection);
	float dotProduct = XMVectorGetX(XMVector3Dot(defaultDirection, targetDirection));
	float angle = acosf(dotProduct);

	// ȸ�� ���� ���� ����ȭ
	rotationAxis = XMVector3Normalize(rotationAxis);

	XMMATRIX rotationMatrix = XMMatrixRotationAxis(rotationAxis, angle);


	for (int i = 0; i < Firework_DEBRISES; ++i)
	{
		XMVECTOR particleDirection = XMLoadFloat3(&Particle_Direction[i]);
		XMVECTOR rotatedDirection = XMVector3TransformNormal(particleDirection, rotationMatrix);

		// ȸ���� ���� ���͸� XMFLOAT3�� ��ȯ�Ͽ� �迭�� ����
		XMStoreFloat3(&Particle_Direction[i], rotatedDirection);
	}

	Default_Direction = Direction;

}

void Firework_Particle::Animate(float fElapsedTime)
{
	static int x = 0;
	x += 1;
	if (active)
	{
		if (Active_Particle < Firework_DEBRISES)
			Active_Particle += 1;

		m_fElapsedTimes += fElapsedTime;

		XMFLOAT3 xmf3Position = GetPosition();

		for (int i = 0; i < int(Active_Particle); ++i)
		{
			Particle_ElapsedTime[i] += fElapsedTime;

			if (-1.0f <= Particle_Direction[i].y)
				Particle_Direction[i].y -= 0.5f * fElapsedTime;

			m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
			m_pxmf4x4Transforms[i]._41 = xmf3Position.x + Particle_Direction[i].x * Particle_Speed[i] * Particle_ElapsedTime[i];
			m_pxmf4x4Transforms[i]._42 = xmf3Position.y + Particle_Direction[i].y * Particle_Speed[i] * Particle_ElapsedTime[i];
			m_pxmf4x4Transforms[i]._43 = xmf3Position.z + Particle_Direction[i].z * Particle_Speed[i] * Particle_ElapsedTime[i];

			// ȸ�� ����
			m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(Particle_Direction[i], Particle_Rotation * Particle_ElapsedTime[i]), m_pxmf4x4Transforms[i]);

		}
	}

	if (m_fDuration < m_fElapsedTimes)
		Reset();

}
void Firework_Particle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//IsVisible(pCamera)
	if (true && active)
	{
		// ��ġ ���� ������Ʈ
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		UINT ncbMaterialBytes = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255);

		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialGpuVirtualAddress = firework_Constant_Buffer->GetGPUVirtualAddress();

		if (m_FireworkMesh)
		{
			for (int j = 0; j < int(Active_Particle); ++j)
			{
				pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * j));
				pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialGpuVirtualAddress + (ncbMaterialBytes * j));
				m_FireworkMesh->Render(pd3dCommandList);
			}
		}
	}
}

void Firework_Particle::Reset()
{
	Particle_Speed.resize(Firework_DEBRISES);
	Particle_Direction.resize(Firework_DEBRISES);
	Particle_ElapsedTime.resize(Firework_DEBRISES);

	for (int i = 0; i < Firework_DEBRISES; ++i)
	{
		Particle_ElapsedTime[i] = 0.0f;
		Particle_Direction[i] = Firework_Vectors[i];
		Particle_Speed[i] = 200.0f;
	}

	Active_Particle = 0;
	m_fElapsedTimes = 0.0f;
	active = false;
}


