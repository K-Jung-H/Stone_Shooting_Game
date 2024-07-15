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

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

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
//protected:
//	// 쉐이더 객체에 포함되어 있는 모든 게임 객체들에 대한 리소스
//	ID3D12Resource* m_pd3dcbGameObjects = NULL;
//
//	// 쉐이더 객체에 포함되어 있는 모든 게임 객체들에 대한 리소스 포인터
//	UINT8* m_pcbMappedGameObjects = NULL;

public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	
	virtual void Create_Shader_Resource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update_Shader_Resource(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release_Shader_Resource();

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

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
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};
