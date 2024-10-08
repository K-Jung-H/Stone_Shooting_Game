#pragma once
#include "stdafx.h"

//정점을 표현하기 위한 클래스를 선언한다. 

class CVertex
{

public:
	//정점의 위치 벡터이다(모든 정점은 최소한 위치 벡터를 가져야 한다). 
	XMFLOAT3 m_xmf3Position;

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};


class CDiffusedVertex : public CVertex
{

public:
	//정점의 색상이다. 
	XMFLOAT4 m_xmf4Diffuse;

public:
	CDiffusedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = xmf3Position; 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }

};
 
class CIlluminatedVertex : public CVertex
{
public:
	XMFLOAT3 m_xmf3Normal;

public:
	CIlluminatedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) {
		m_xmf3Position = XMFLOAT3(x, y, z); m_xmf3Normal = xmf3Normal;
	}

	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) {
		m_xmf3Position = xmf3Position; m_xmf3Normal = xmf3Normal;
	}
	~CIlluminatedVertex() { }
};

class CTexturedVertex : public CVertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CTexturedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; }
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; }
	~CTexturedVertex() { }
};

class CMesh
{
protected:
	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;

	/*인덱스 버퍼(인덱스의 배열)와 인덱스 버퍼를 위한 업로드 버퍼에 대한 인터페이스 포인터이다. 
	인덱스 버퍼는 정점 버퍼(배열)에 대한 인덱스를 가진다.*/
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	UINT m_nIndices = 0;
	//인덱스 버퍼에 포함되는 인덱스의 개수이다. 

	UINT m_nStartIndex = 0;
	//인덱스 버퍼에서 메쉬를 그리기 위해 사용되는 시작 인덱스이다. 
	
	int m_nBaseVertex = 0;
	//인덱스 버퍼의 인덱스에 더해질 인덱스이다. 



	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

	//정점을 픽킹을 위하여 저장한다(정점 버퍼를 Map()하여 읽지 않아도 되도록).
	CDiffusedVertex* m_pVertices_D = NULL;
	CIlluminatedVertex* m_pVertices_I = NULL;
	CTexturedVertex* m_pVertices_T = NULL;

	//메쉬의 인덱스를 저장한다(인덱스 버퍼를 Map()하여 읽지 않아도 되도록).
	UINT* m_pnIndices = NULL;

private:
	int m_nReferences = 0;

public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	BoundingOrientedBox GetBoundingBox() { return(m_xmBoundingBox); }

	//광선과 메쉬의 교차를 검사하고 교차하는 횟수와 거리를 반환하는 함수이다. 
	int CheckRayIntersection(XMFLOAT3& xmRayPosition, XMFLOAT3& xmRayDirection, float* pfNearHitDistance);
public:
	//모델 좌표계의 OOBB 바운딩 박스이다. 
	BoundingOrientedBox m_xmBoundingBox;
	BoundingSphere m_xmBoundingSphere;
	bool picked = false;

};
//////////////////////////////////////////////////////////////////////////////////////

class CCubeMeshDiffused : public CMesh
{
public:
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다. 
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, 
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, 
		XMFLOAT4 fcolor1 = { 0.5f, 0.0f, 0.0f, 1.0f }, XMFLOAT4 fcolor2 = { 0.5f, 0.0f, 0.0f, 1.0f });

	virtual ~CCubeMeshDiffused();
};
//////////////////////////////////////////////////////////////////////////////////////

class CPlaneMesh : public CMesh
{
public:
	CPlaneMesh::CPlaneMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fDepth, XMFLOAT4 xmf4Color);
	virtual ~CPlaneMesh();
}; 
//////////////////////////////////////////////////////////////////////////////////////

class CAirplaneMeshDiffused : public CMesh
{
public:
	CAirplaneMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f, XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));

	virtual ~CAirplaneMeshDiffused();

	
	CDiffusedVertex* picked_m_pVertices = NULL;
	UINT picked_m_nVertices = 0;


};
//////////////////////////////////////////////////////////////////////////////////////

class CSphereMeshDiffused : public CMesh
{
public:
	CSphereMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fRadius = 2.0f, int nSlices = 20, int nStacks = 20, bool black = false);
	virtual ~CSphereMeshDiffused();
};
//////////////////////////////////////////////////////////////////////////////////////

class UIMesh : public CMesh
{
public:
	UIMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fWidth = 200.0f, float fHeight = 90.0f, float layer_z = 1.0f, XMFLOAT4 xmf4Color = { 1.0f, 0.0f, 0.0f, 1.0f }, bool Color_Mix = true);
	virtual ~UIMesh();
};

//===================================================
//===================================================
//===================================================

class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMeshIlluminated();
public:
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices);
};

class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};


class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fRadius = 2.0f, int nSlices = 20, int nStacks = 20, float y_scale = 1.0f);
	virtual ~CSphereMeshIlluminated();
};


class CTorusMeshIlluminated : public CMeshIlluminated
{
public:
	CTorusMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		float fMajorRadius, float fMinorRadius, int nMajorSegments, int nMinorSegments);
	virtual ~CTorusMeshIlluminated();

};


class CPlaneMeshIlluminated : public CMeshIlluminated
{
public:
	CPlaneMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fDepth, int N_SubRect);
	virtual ~CPlaneMeshIlluminated();
};



//======================================================================================

class CHeightMapImage
{
private:
	//높이 맵 이미지 픽셀(8-비트)들의 이차원 배열이다. 각 픽셀은 0~255의 값을 갖는다.
	BYTE* m_pHeightMapPixels;
	//높이 맵 이미지의 가로와 세로 크기이다.
	int m_nWidth;
	int	m_nLength;

	//높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;
public:
	
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);
	
	//높이 맵 이미지에서 (x, z) 위치의 픽셀 값에 기반한 지형의 높이를 반환한다.
	float GetHeight(float x, float z);
	
	//높이 맵 이미지에서 (x, z) 위치의 법선 벡터를 반환한다.
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapGridMesh_Illuminated : public CMeshIlluminated
{
protected:
	//격자의 크기(가로: x-방향, 세로: z-방향)
	int	m_nWidth;
	int	m_nLength;

	/*
	격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터
	실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 가짐.
	-> 실제 격자의 x-축 방향의 간격은 1이 아니라 스케일 벡터의 x-좌표가 됨
	-->작은 격자(적은 정점)를 사용하더라도 큰 크기의 격자(지형)를 생성할 수 있음.
	*/
	XMFLOAT3 m_xmf3Scale;

public:

	CHeightMapGridMesh_Illuminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength,
		XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), 
		void* pContext = NULL);

	virtual ~CHeightMapGridMesh_Illuminated();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다.
	virtual float OnGetHeight(int x, int z, void* pContext);

	//격자의 좌표가 (x, z)일 때 교점(정점)의 색상을 반환하는 함수이다.
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);
};

//======================================================================================



class CPlaneMeshTextured : public CMesh
{
public:
	CPlaneMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fDepth, int N_SubRect);
	virtual ~CPlaneMeshTextured();
};


class CHeightMapGridMesh_Textured : public CMesh
{
protected:
	//격자의 크기(가로: x-방향, 세로: z-방향)
	int	m_nWidth;
	int	m_nLength;

	/*
	격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터
	실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 가짐.
	-> 실제 격자의 x-축 방향의 간격은 1이 아니라 스케일 벡터의 x-좌표가 됨
	-->작은 격자(적은 정점)를 사용하더라도 큰 크기의 격자(지형)를 생성할 수 있음.
	*/
	XMFLOAT3 m_xmf3Scale;

public:

	CHeightMapGridMesh_Textured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength,
		XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f),
		void* pContext = NULL);

	virtual ~CHeightMapGridMesh_Textured();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다.
	virtual float OnGetHeight(int x, int z, void* pContext);


};
