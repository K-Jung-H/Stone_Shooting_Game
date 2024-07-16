#pragma once
#include "GameObject.h"

enum class ParticleType {
	Explosion,
	None,
};

class Particle : public CRotatingObject
{
public:
	ParticleType type = ParticleType::None;

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 2.0f;

public:
	UINT8* particles_info = NULL;

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
	static bool Setting;
	static CMesh* m_ExplosionMesh; 
	static XMFLOAT3				m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
	XMFLOAT4X4					m_pxmf4x4Transforms[EXPLOSION_DEBRISES];


	float						m_fExplosionSpeed = 10.0f;
	float						m_fExplosionRotation = 720.0f;

public:
	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	
	Explosion_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		UINT material = UINT(0), ParticleType p_type = ParticleType::Explosion);
	virtual ~Explosion_Particle();


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	virtual void Animate(float fElapsedTime);
	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};