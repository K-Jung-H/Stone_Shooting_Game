#include "stdafx.h"
#include "Particle.h"

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

inline XMFLOAT3 Random_In_Circle()
{
	float theta = RandF(0.0f, 2.0f * 3.14159265358979323846f); // 2π

	float r = sqrt(RandF(0.0f, 1.0f));
	float x = r * cosf(theta);
	float z = r * sinf(theta);
	return XMFLOAT3(x, 0.0f, z);
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

// 0,1,0 벡터가 -angle에서 angle 사이의 무작위 각도로 Z축을 기준으로 회전하고,
// 이후 0~360도 사이의 각도로 Y축을 기준으로 추가 회전한 벡터를 반환
XMVECTOR GetRandomRotatedVector(float angle)
{
	// 첫 번째 회전: 0,1,0 벡터를 Z축을 기준으로 -angle에서 angle 사이의 무작위 각도로 회전
	XMVECTOR initialVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	float randomAngleDegrees = RandF(-angle, angle);
	float randomAngleRadians = randomAngleDegrees * XM_PI / 180.0f; // 각도를 라디안으로 변환
	XMVECTOR rotationAxisZ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // Z축을 기준으로 회전
	XMVECTOR rotatedVector = RotateVector(initialVector, rotationAxisZ, randomAngleRadians);

	// 두 번째 회전: 회전된 벡터를 Y축을 기준으로 0~360도 사이의 무작위 각도로 회전
	float additionalRotationDegrees = RandF(0.0f, 360.0f);
	float additionalRotationRadians = additionalRotationDegrees * XM_PI / 180.0f; // 각도를 라디안으로 변환
	XMVECTOR rotationAxisY = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Y축을 기준으로 회전
	XMVECTOR finalVector = RotateVector(rotatedVector, rotationAxisY, additionalRotationRadians);

	return finalVector;
}

//==========================================================

Particle::Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Particle_Type particle_type) : CRotatingObject(pd3dDevice, pd3dCommandList)
{
	o_type = Object_Type::Particle;
	p_type = particle_type;
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


Explosion_Particle::Explosion_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* material, Particle_Type p_type) : Particle(pd3dDevice, pd3dCommandList, p_type)
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
		// 위치 정보 업데이트
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();

		// 활성화된 재질의 정보 업데이트
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

void Explosion_Particle::Reset()
{
	for (XMFLOAT4X4& transform_matrix : m_pxmf4x4Transforms)
	{
		transform_matrix._41 = 0.0f;
		transform_matrix._42 = 0.0f;
		transform_matrix._43 = 0.0f;
	}
	m_fElapsedTimes = 0.0f;
	active = false;
}

//===========================================================================

CMesh* Small_Explosion_Particle::m_Small_ExplosionMesh = NULL;
bool Small_Explosion_Particle::Setting = false;


Small_Explosion_Particle::Small_Explosion_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* material, Particle_Type p_type) : Explosion_Particle(pd3dDevice, pd3dCommandList, material, p_type)
{
}

Small_Explosion_Particle::~Small_Explosion_Particle()
{
}

void Small_Explosion_Particle::Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (Setting == false)
	{
		if(Explosion_Particle::Setting == false)
			for (int i = 0; i < EXPLOSION_DEBRISES; ++i)
				XMStoreFloat3(&Explosion_Sphere_Vectors[i], RandomUnitVectorOnSphere());

		m_Small_ExplosionMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 5.0f, 1.0f, 1.0f);
		Setting = true;
	}
}

void Small_Explosion_Particle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//IsVisible(pCamera)
	if (true && active)
	{
		// 위치 정보 업데이트
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();

		// 활성화된 재질의 정보 업데이트
		for (int i = 0; i < m_ppMaterials.size(); ++i)
			if (m_ppMaterials[i].second == true)
				CGameObject::Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::Material_info, i);



		if (m_Small_ExplosionMesh)
		{
			for (int j = 0; j < EXPLOSION_DEBRISES; ++j)
			{
				pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * j));

				m_Small_ExplosionMesh->Render(pd3dCommandList);

			}
		}
	}
}


//===========================================================================

XMFLOAT3 Charge_Particle::Charge_Sphere_Vectors[CHARGE_DEBRISES];
CMesh* Charge_Particle::m_ChargeMesh = NULL;
bool Charge_Particle::Setting = false;

Charge_Particle::Charge_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float Range, float cycle_time, CMaterial* material, Particle_Type p_type) : Particle(pd3dDevice, pd3dCommandList, p_type)
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
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * CHARGE_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);
}

void Charge_Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
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
			// 경과 시간 업데이트
			Particle_ElapsedTime[i] += fElapsedTime * Particle_Speed[i];

			// 경과 시간에 따라 시작 위치에서 이동하도록 설정
			float t = Particle_ElapsedTime[i] / m_fDuration;

			// 원점에 도달한 입자는 다시 시작 위치로 돌아감
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

			// 회전 적용
			m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(Charge_Sphere_Vectors[i], Particle_Rotation * Particle_ElapsedTime[i]), m_pxmf4x4Transforms[i]);
		}
	}

}
void Charge_Particle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	//IsVisible(pCamera)

	if (true && active)
	{
		bool shader_changed = false;
		// 위치 정보 업데이트
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();

		// 활성화된 재질의 정보 업데이트
		for (int i = 0; i < m_ppMaterials.size(); ++i)
		{
			if (m_ppMaterials[i].second)
			{
				if (m_ppMaterials[i].first->material_shader)
				{
					m_ppMaterials[i].first->material_shader->Setting_Render(pd3dCommandList, o_type, used_item);
					shader_changed = true;
				}
				// 재질 정보 컨테이너 업데이트 :: Mapped_Material_info
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
			if (shader_changed)
				pShader->Setting_Render(pd3dCommandList);

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
	used_item = Item_Type::None;
	m_fElapsedTimes = 0.0f;
	active = false;
}

//===========================================================================================================


XMFLOAT3 Firework_Particle::Firework_Vectors[Firework_DEBRISES];
CMesh* Firework_Particle::m_FireworkMesh = NULL;
bool Firework_Particle::Setting = false;

Firework_Particle::Firework_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float cycle_time, CMaterial* material, Particle_Type p_type) : Particle(pd3dDevice, pd3dCommandList, p_type)
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
	UINT ncbElementBytes1 = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes1 * Firework_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);

	//-------------------------------------------------------------------

	// 게임 재질 버퍼 생성 및 매핑
	UINT ncbElementBytes2 = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255); //256의 배수
	firework_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes2 * Firework_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	firework_Constant_Buffer->Map(0, NULL, (void**)&firework_material_info);


}

void Firework_Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
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

	if (firework_Constant_Buffer)
	{
		firework_Constant_Buffer->Unmap(0, NULL);
		firework_Constant_Buffer->Release();
	}
	
}

void Firework_Particle::Set_Main_Direction(XMFLOAT3 Direction)
{
	XMVECTOR defaultDirection = XMLoadFloat3(&Default_Direction);
	XMVECTOR targetDirection = XMLoadFloat3(&Direction);

	// 회전 축과 각도를 계산
	XMVECTOR rotationAxis = XMVector3Cross(defaultDirection, targetDirection);
	float dotProduct = XMVectorGetX(XMVector3Dot(defaultDirection, targetDirection));
	float angle = acosf(dotProduct);

	// 회전 축의 길이 정규화
	rotationAxis = XMVector3Normalize(rotationAxis);

	XMMATRIX rotationMatrix = XMMatrixRotationAxis(rotationAxis, angle);


	for (int i = 0; i < Firework_DEBRISES; ++i)
	{
		XMVECTOR particleDirection = XMLoadFloat3(&Particle_Direction[i]);
		XMVECTOR rotatedDirection = XMVector3TransformNormal(particleDirection, rotationMatrix);

		// 회전된 방향 벡터를 XMFLOAT3로 변환하여 배열에 저장
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

			// 회전 적용
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
		// 위치 정보 업데이트
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


//==================================================================================

CMesh* Snow_Particle::m_SnowMesh = NULL;
bool Snow_Particle::Setting = false;
XMFLOAT3 Snow_Particle::Particle_Pos_XZ[Snow_DEBRISES];
XMFLOAT3 Snow_Particle::Particle_Rotation_Vector[Snow_DEBRISES];

Snow_Particle::Snow_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	XMFLOAT3 start_pos, float c_range, CMaterial* material, Particle_Type p_type) : Particle(pd3dDevice, pd3dCommandList, p_type)
{
	Set_Center(start_pos);
	range = c_range;

	
	m_fDuration = 10.0f;
	Particle_Rotation = 300.0f;
	Reset();
	active = true;


	Create_Shader_Resource(pd3dDevice, pd3dCommandList);

	SetMaterial(material);	
}

Snow_Particle::~Snow_Particle()
{
}

void Snow_Particle::Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (Setting == false)
	{
		for (int i = 0; i < Snow_DEBRISES; ++i)
		{
			Particle_Pos_XZ[i] = Random_In_Circle();
			XMStoreFloat3(&Particle_Rotation_Vector[i], RandomUnitVectorOnSphere());
		}
		m_SnowMesh =  new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
		Setting = true;
	}
}

void Snow_Particle::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes1 = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes1 * Snow_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);

	//-------------------------------------------------------------------

	// 눈 재질은 하나로 통일
	UINT ncbElementBytes2 = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255); //256의 배수
	Snow_Constant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes2,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	Snow_Constant_Buffer->Map(0, NULL, (void**)&snow_material_info);


}

void Snow_Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	UINT ncbMaterialBytes = ((sizeof(CB_MATERIAL_INFO) + 255) & ~255);

	XMFLOAT4X4 xmf4x4World;
	for (int i = 0; i < Snow_DEBRISES; ++i)
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4Transforms[i])));

		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)(particles_info + (i * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));

		//-------------------------------------------------------------------
	}

	CMaterialColors* colors = m_ppMaterials[0].first->Material_Colors;
	CB_MATERIAL_INFO* pbMappedMaterial = (CB_MATERIAL_INFO*)(snow_material_info);
	::memcpy(&pbMappedMaterial->m_cAmbient, &colors->m_xmf4Ambient, sizeof(XMFLOAT4));
	::memcpy(&pbMappedMaterial->m_cDiffuse, &colors->m_xmf4Diffuse, sizeof(XMFLOAT4));
	::memcpy(&pbMappedMaterial->m_cSpecular, &colors->m_xmf4Specular, sizeof(XMFLOAT4));
	::memcpy(&pbMappedMaterial->m_cEmissive, &colors->m_xmf4Emissive, sizeof(XMFLOAT4));
}

void Snow_Particle::Release_Shader_Resource()
{
	if (m_pConstant_Buffer)
	{
		m_pConstant_Buffer->Unmap(0, NULL);
		m_pConstant_Buffer->Release();
	}

	if (Snow_Constant_Buffer)
	{
		Snow_Constant_Buffer->Unmap(0, NULL);
		Snow_Constant_Buffer->Release();
	}
	
}

void Snow_Particle::Set_Center(XMFLOAT3 pos)
{
	center_pos = pos;
	Max_Move_Y = pos.y;
	SetPosition(pos);
}

void Snow_Particle::Animate(float fElapsedTime)
{
	if (active)
	{
		if (Active_Particle < Snow_DEBRISES)
			Active_Particle += 0.2f;

		m_fElapsedTimes += fElapsedTime;

		XMFLOAT3 xmf3Position = GetPosition();

		for (int i = 0; i < int(Active_Particle); ++i)
		{
			// 입자의 높이y 업데이트
			float move_distance = Particle_Speed[i] * Particle_ElapsedTime[i];
			Particle_ElapsedTime[i] += fElapsedTime;

			// 원점에 도달한 입자는 다시 시작 위치로 돌아감
			if (move_distance > Max_Move_Y)
			{
				Particle_ElapsedTime[i] = 0.0f;
				Particle_Speed[i] = ((rand() % 10 + 1) * 0.5f) + 10.0f; // 10.5f ~ 15.0f
			}

			m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
			m_pxmf4x4Transforms[i]._41 = xmf3Position.x + (Particle_Pos_XZ[i].x * range) + Default_Direction.x * Particle_Speed[i] * Particle_ElapsedTime[i];
			m_pxmf4x4Transforms[i]._42 = xmf3Position.y + Particle_Pos_XZ[i].y + Default_Direction.y * move_distance;
			m_pxmf4x4Transforms[i]._43 = xmf3Position.z + (Particle_Pos_XZ[i].z * range) + Default_Direction.z * Particle_Speed[i] * Particle_ElapsedTime[i];

			// 회전 적용
			m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(Particle_Rotation_Vector[i], Particle_Rotation * Particle_ElapsedTime[i]), m_pxmf4x4Transforms[i]);
		}
	}
	//if (m_fDuration < m_fElapsedTimes)
	//	Reset();

}
void Snow_Particle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//IsVisible(pCamera)
	if (true && active)
	{
		// 위치 + 재질 정보 업데이트
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialGpuVirtualAddress = Snow_Constant_Buffer->GetGPUVirtualAddress();
		
		pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialGpuVirtualAddress);;

		if (m_SnowMesh)
		{
			for (int i = 0; i < int(Active_Particle); ++i)
			{
				pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * i));
				m_SnowMesh->Render(pd3dCommandList);
			}
		}
	}
}

void Snow_Particle::Reset()
{
	Particle_Speed.resize(Snow_DEBRISES);
	Particle_ElapsedTime.resize(Snow_DEBRISES);

	for (int i = 0; i < Snow_DEBRISES; ++i)
	{
		Particle_ElapsedTime[i] = 0.0f;
		Particle_Speed[i] = ((rand() % 10 + 1) * 0.5f) + 10.0f; // 10.5f ~ 15.0f
	}

	Active_Particle = 0;
	m_fElapsedTimes = 0.0f;
	active = false;
}

//==================================================================================

CMesh* Fire_Boom_Particle::m_Fire_Boom_Mesh = NULL;
bool Fire_Boom_Particle::Setting = false;

void Fire_Boom_Particle::Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (Setting == false)
	{
		m_Fire_Boom_Mesh = new CTorusMeshIlluminated(pd3dDevice, pd3dCommandList, 5.0f, 4.0f, 20, 20);
		Setting = true;
	}
}



Fire_Boom_Particle::Fire_Boom_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	CMaterial* material, Particle_Type p_type) : Particle(pd3dDevice, pd3dCommandList, p_type)
{
	m_fDuration = 1.0f;
	m_fBoom_Speed = 100.0f;

	Create_Shader_Resource(pd3dDevice, pd3dCommandList);
	Create_Material_Buffer(pd3dDevice, pd3dCommandList);
	SetMaterial(material);
}
Fire_Boom_Particle::~Fire_Boom_Particle()
{

}


void Fire_Boom_Particle::Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes1 = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pConstant_Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes1 * Fire_Boom_DEBRISES,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pConstant_Buffer->Map(0, NULL, (void**)&particles_info);
}

void Fire_Boom_Particle::Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	XMFLOAT4X4 xmf4x4World;

	for (int i = 0; i < Fire_Boom_DEBRISES; ++i)
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4Transforms[i])));

		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)(particles_info + (i * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
	}

}
void Fire_Boom_Particle::Release_Shader_Resource()
{
	if (m_pConstant_Buffer)
	{
		m_pConstant_Buffer->Unmap(0, NULL);
		m_pConstant_Buffer->Release();
	}
}

void Fire_Boom_Particle::Animate(float fElapsedTime)
{
	m_fElapsedTimes += fElapsedTime;

	scale_value += fElapsedTime;
	scale_value = (int)scale_value % 3 + 1;

	XMFLOAT3 center = GetPosition();

	for (int i = 0; i < Fire_Boom_DEBRISES; ++i)
	{
		m_pxmf4x4Transforms[i] = Matrix4x4::Identity();

		XMMATRIX translationToCenter = XMMatrixTranslation(-center.x, -center.y, -center.z);
		XMMATRIX scaling = XMMatrixScaling(scale_value, scale_value, scale_value);
		XMMATRIX translationBack = XMMatrixTranslation(center.x, center.y, center.z);

		XMMATRIX transform = translationToCenter * scaling * translationBack;
		XMStoreFloat4x4(&m_pxmf4x4Transforms[i], transform);

		m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f), Particle_Rotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);

	}

}
void Fire_Boom_Particle::Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//IsVisible(pCamera)
	if (true && active)
	{
		// 위치 정보 업데이트
		Update_Shader_Resource(pd3dCommandList);

		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pConstant_Buffer->GetGPUVirtualAddress();

		// 활성화된 재질의 정보 업데이트
		for (int i = 0; i < m_ppMaterials.size(); ++i)
			if (m_ppMaterials[i].second == true)
				CGameObject::Update_Shader_Resource(pd3dCommandList, Resource_Buffer_Type::Material_info, i);


		if (m_Fire_Boom_Mesh)
		{
			for (int j = 0; j < Fire_Boom_DEBRISES; ++j)
			{
				pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * j));

				m_Fire_Boom_Mesh->Render(pd3dCommandList);

			}
		}
	}
}
void Fire_Boom_Particle::Reset()
{
}


