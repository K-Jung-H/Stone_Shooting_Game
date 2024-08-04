#pragma once
#include "GameObject.h"



class Particle : public CRotatingObject
{
public:
	Particle_Type type = Particle_Type::None;

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 2.0f;
	float						Particle_Rotation = 720.0f;
	
public:
	UINT8* particles_info = NULL;
	ID3D12Resource* m_pConstant_Buffer; // �ѹ��� ���� ������ ��� ������ �Ϲ� GameObject�� ���ۿ� ũ�Ⱑ �޶�� ��

	Particle(Particle_Type p_type = Particle_Type::None);
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
		CMaterial* material, Particle_Type p_type = Particle_Type::Explosion);
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

	std::vector<XMFLOAT3> Particle_Start_Position; // �� ������ ���� ��ġ
	std::vector<float> Particle_ElapsedTime; // �� ������ ��� �ð�
	std::vector<float> Particle_Speed; // ���ں� �ӵ�
	float						Max_Range = 100.0f;
	float						m_fChargeSpeed = 10.0f;
	float						Rotation_Orbit = 200.0f;

	float						Active_Particle = 0;
public:
	static void Prepare_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	Charge_Particle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float Range, float cycle_time, CMaterial* material, Particle_Type p_type = Particle_Type::Charge);

	virtual ~Charge_Particle();


	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	void Set_Center_Position(XMFLOAT3 pos);

	virtual void Animate(float fElapsedTime);
	virtual void Particle_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Reset();
};