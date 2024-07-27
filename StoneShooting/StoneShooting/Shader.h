#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include <vector>
//셰이더 소스 코드를 컴파일하고 그래픽스 상태 객체를 생성한다. 




class CShader
{
public:
	CShader();
	virtual ~CShader();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	
	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	
	virtual void Release_Shader_Resource();

	void AddObjects(CGameObject* gameobject);


	virtual void AnimateObjects(float fTimeElapsed);

	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Setting_PSO(ID3D12GraphicsCommandList* pd3dCommandList, int N = 0);


	void ReleaseUploadBuffers();
	void ReleaseObjects();

	CGameObject* PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance);
	
	// 게임 오브젝트 벡터의 주소 반환
	std::vector<CGameObject*>& GetObjects() { return m_ppObjects; } 

protected:
	// 파이프라인 상태 종류
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;

	// 셰이더 별 게임 객체들
	std::vector<CGameObject*> m_ppObjects;
	int m_nObjects = 0;

private:
	int m_nReferences = 0;
};

class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void ReleaseUploadBuffers();
	virtual void ReleaseObjects();
};


//“CObjectsShader” 클래스는 게임 객체들을 포함하는 셰이더 객체
class CObjectsShader : public CShader
{
protected:

	// GPU에 있는 객체에 대한 리소스 포인터
	// 해당 포인터를 통해, GPU에 있는 정보 접근 가능
	CB_GAMEOBJECT_INFO* Mapped_Object_info = NULL;

	// 쉐이더 객체에 연결된 게임 객체 버퍼
	ID3D12Resource* Object_Constant_Buffer = NULL;


	// GPU에 있는 객체에 대한 리소스 포인터
	CB_MATERIAL_INFO* Mapped_Material_info = NULL;

	// 쉐이더 객체에 연결된 재질 버퍼
	ID3D12Resource* Material_Constant_Buffer = NULL;



public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	
	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Create_Shader_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Update_Object_Buffer(ID3D12GraphicsCommandList* pd3dCommandList, CB_GAMEOBJECT_INFO* Object_info);
	virtual void Update_Material_Buffer(ID3D12GraphicsCommandList* pd3dCommandList, CB_MATERIAL_INFO* Material_info);

	virtual void Release_Shader_Resource();

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList);

};

class UIShader : public CShader
{
public:
	UIShader();
	virtual ~UIShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList);
};
