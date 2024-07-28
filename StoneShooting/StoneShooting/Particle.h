#pragma once
#include "GameObject.h"



class Particle : public CRotatingObject
{
public:
	ParticleType type = ParticleType::None;

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 2.0f;
	float						Particle_Rotation = 720.0f;
	
public:
	UINT8* particles_info = NULL;
	ID3D12Resource* m_pConstant_Buffer; // 한번에 여러 정보를 담기 때문에 일반 GameObject의 버퍼와 크기가 달라야 함

	Particle(ParticleType p_type = ParticleType::None);
	virtual ~Particle();

	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	virtual void Animate(float fElapsedTime);
	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Reset();
};


#define EXPLOSION_DEBRISES 240

class Explosion_Particle : public Particle
{
	static XMFLOAT3				Explosion_Sphere_Vectors[EXPLOSION_DEBRISES];
	static bool Setting;
	static CMesh* m_ExplosionMesh; 
	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float						m_fExplosionSpeed = 10.0f;
	

public:
	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	
	Explosion_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		CMaterial* material, ParticleType p_type = ParticleType::Explosion);
	virtual ~Explosion_Particle();


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	virtual void Animate(float fElapsedTime);
	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};



#define CHARGE_DEBRISES 360

class Charge_Particle : public Particle
{
	static bool Setting;
	static CMesh* m_ChargeMesh;
	static XMFLOAT3				Charge_Sphere_Vectors[CHARGE_DEBRISES];
	XMFLOAT4X4					m_pxmf4x4Transforms[CHARGE_DEBRISES];

	std::vector<XMFLOAT3> Particle_Start_Position; // 각 입자의 시작 위치
	std::vector<float> Particle_ElapsedTime; // 각 입자의 경과 시간
	std::vector<float> Particle_Speed; // 입자별 속도
	float						Max_Range = 100.0f;
	float						m_fChargeSpeed = 10.0f;
	float						Rotation_Orbit = 200.0f;

	float						Active_Particle = 0;
public:
	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	Charge_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float Range, float cycle_time, CMaterial* material, ParticleType p_type = ParticleType::Charge);

	virtual ~Charge_Particle();


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	void Set_Center_Position(XMFLOAT3 pos);

	virtual void Animate(float fElapsedTime);
	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Reset();
};