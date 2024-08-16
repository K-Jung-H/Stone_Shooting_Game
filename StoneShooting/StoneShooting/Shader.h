#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include <vector>
//���̴� �ҽ� �ڵ带 �������ϰ� �׷��Ƚ� ���� ��ü�� �����Ѵ�. 




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

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Setting_PSO(ID3D12GraphicsCommandList* pd3dCommandList, int N = 0);


	void ReleaseUploadBuffers();
	void ReleaseObjects();



protected:
	// ���������� ���� ����
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;

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


//��CObjectsShader�� Ŭ������ ���� ��ü���� �����ϴ� ���̴� ��ü
class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	


	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList);

};


struct CB_Outline_INFO
{
	XMFLOAT4 color;
	float Outline_Thickness;
};

class OutlineShader : public CShader
{
private:
	CB_Outline_INFO* Mapped_Outline_info = NULL;
	ID3D12Resource* Outline_Constant_Buffer = NULL;

public:
	OutlineShader();
	virtual ~OutlineShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void Create_Outline_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Update_Outline_Buffer(ID3D12GraphicsCommandList* pd3dCommandList);

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
