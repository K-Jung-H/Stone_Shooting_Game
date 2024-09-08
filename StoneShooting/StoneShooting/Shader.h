#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include <vector>
//셰이더 소스 코드를 컴파일하고 그래픽스 상태 객체를 생성한다. 




class CShader
{
private:
	int m_nReferences = 0;

protected:
	// 파이프라인 상태 종류
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;

public:
	Shader_Type type = Shader_Type::ETC;


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

	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList, Object_Type o_type = Object_Type::ETC, Item_Type i_type = Item_Type::None);


	virtual void Setting_PSO(ID3D12GraphicsCommandList* pd3dCommandList, int N = 0);


	void ReleaseUploadBuffers();
	void ReleaseObjects();




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
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	


	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList, Object_Type o_type = Object_Type::ETC, Item_Type i_type = Item_Type::None);

};


struct Outline_INFO
{
	XMFLOAT4 color;
	float Outline_Thickness;
	float padding[3];
};

class OutlineShader : public CShader
{
protected:
	XMFLOAT4 color;
	float Thickness;

public:
	OutlineShader();
	virtual ~OutlineShader();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void Create_Outline_Buffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Update_Outline_Buffer(ID3D12GraphicsCommandList* pd3dCommandList, float thickness = 0.5, XMFLOAT4 color = { 0.0f,0.0f,0.0f,1.0f });

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList, Object_Type o_type = Object_Type::ETC, Item_Type i_type = Item_Type::None);

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
	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList, Object_Type o_type = Object_Type::ETC, Item_Type i_type = Item_Type::None);

};

class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	virtual void Setting_Render(ID3D12GraphicsCommandList* pd3dCommandList, Object_Type o_type, Item_Type i_type);
};