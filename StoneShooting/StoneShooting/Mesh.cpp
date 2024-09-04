﻿#include "stdafx.h"
#include "Mesh.h"
#include <vector>

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();

	if (m_pVertices_D) delete[] m_pVertices_D;
	if (m_pVertices_I) delete[] m_pVertices_I;
	if (m_pnIndices) delete[] m_pnIndices;

}

void CMesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dVertexUploadBuffer) 
		m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) 
		m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, float fWidth, float fHeight, float fDepth, XMFLOAT4 fcolor1, XMFLOAT4 fcolor2) : CMesh(pd3dDevice,
		pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다. 
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;



	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 
	m_pVertices_D = new CDiffusedVertex[m_nVertices];
	m_pVertices_D[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), fcolor1);
	m_pVertices_D[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), fcolor2);
	m_pVertices_D[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), fcolor1);
	m_pVertices_D[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), fcolor2);
	m_pVertices_D[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), fcolor1);
	m_pVertices_D[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), fcolor2);
	m_pVertices_D[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), fcolor1);
	m_pVertices_D[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), fcolor2);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices_D, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		&m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;


	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이
	므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인
	덱스를 가져야 한다.*/
	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;

	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;

	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;

	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;

	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;

	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;

	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;

	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;

	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;

	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;

	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;

	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;


	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	//메쉬의 바운딩 박스(모델 좌표계)를 생성한다.
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
CPlaneMesh::CPlaneMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fDepth, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	int nSubRects = 20;

	float fHalfWidth = fWidth * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth / nSubRects;
	float fCellDepth = fDepth / nSubRects;

	XMFLOAT4 color1 = XMFLOAT4(xmf4Color.x+0.1f, xmf4Color.y, xmf4Color.z, 1.0f);
	XMFLOAT4 color2 = XMFLOAT4(xmf4Color.x + 0.2f, xmf4Color.y + 0.1f, xmf4Color.z, 1.0f);
	XMFLOAT4 color3 = XMFLOAT4(xmf4Color.x + 0.1f, xmf4Color.y + 0.1f, xmf4Color.z, 1.0f);

	m_nVertices = 0;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<CDiffusedVertex> tempVertices;

	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v1(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v2(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth));
			XMFLOAT3 v3(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth));

			// Calculate average color
			XMFLOAT4 randomColor = (rand() % 3 == 0) ? color1 : (rand() % 2 == 0) ? color2 : color3;

			tempVertices.push_back(CDiffusedVertex(v0, randomColor));
			tempVertices.push_back(CDiffusedVertex(v2, randomColor));
			tempVertices.push_back(CDiffusedVertex(v1, randomColor));

			tempVertices.push_back(CDiffusedVertex(v0, randomColor));
			tempVertices.push_back(CDiffusedVertex(v3, randomColor));
			tempVertices.push_back(CDiffusedVertex(v2, randomColor));
		}
	}

	m_nVertices = tempVertices.size();
	m_pVertices_D = new CDiffusedVertex[m_nVertices];
	std::copy(tempVertices.begin(), tempVertices.end(), m_pVertices_D);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices_D, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, 10.0f, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CPlaneMesh::~CPlaneMesh() {
	if (m_pVertices_D) delete[] m_pVertices_D;
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
}



//////////////////////////////////////////////////////////////////////////////////////////////


CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth,
	XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	//비행기 메쉬를 표현하기 위한 정점 데이터이다.
	m_pVertices_D = new CDiffusedVertex[m_nVertices];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;

	//비행기 메쉬의 위쪽 면
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,RANDOM_COLOR));

	//비행기 메쉬의 아래쪽 면
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color, RANDOM_COLOR));

	//비행기 메쉬의 오른쪽 면
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/오른쪽 면
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 왼쪽 면
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/왼쪽 면
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices_D[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices_D,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//메쉬의 바운딩 박스(모델 좌표계)를 생성한다. 
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSphereMeshDiffused::CSphereMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, int nSlices, int nStacks, bool black) :
	CMesh(pd3dDevice, pd3dCommandList)
{
	XMFLOAT4 stone_color1;
	XMFLOAT4 stone_color2;

	float y_scale = 0.5f;

	if (black)
	{
		stone_color1 = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		stone_color2 = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	}
	else
	{
		stone_color1 = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		stone_color2 = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	}
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/*원기둥의 표면에 있는 사각형(줄)의 개수는 {nSlices * (nStacks-2)}이다. 사각형들이 원기둥의 표면을 따라 연속되
	고 처음과 마지막 사각형이 연결되어 있으므로 첫 번째 원기둥을 제외하고 사각형 하나를 표현하기 위하여 하나의 정
	점이 필요하다. 첫 번째 원기둥은 위아래로 두 개의 정점이 필요하므로 원기둥의 표면의 사각형들을 표현하기 위하여
	필요한 정점의 개수는 {(nSlices * (nStacks–1))}이다. 그런데 구의 위와 아래(구가 지구라고 가정할 때 남극과 북극)
	를 자르면 원기둥이 아니라 원뿔이 되므로 이 원뿔을 표현하기 위하여 2개의 정점이 더 필요하다. 그러므로 정점의 전
	체 개수는 {(nSlices * (nStacks–1)) + 2}이다.*/
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pVertices_D = new CDiffusedVertex[m_nVertices];

	//180도를 nStacks 만큼 분할한다. 
	float fDeltaPhi = float(XM_PI / nStacks);
	//360도를 nSlices 만큼 분할한다. 

	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	int k = 0;



	//구의 위(북극)를 나타내는 정점이다. 
	m_pVertices_D[k++] = CDiffusedVertex(0.0f, +fRadius * y_scale, 0.0f, (rand() % 2 == 0 ? stone_color1 : stone_color2)
	);

	float theta_i, phi_j;
	//원기둥 표면의 정점이다. 
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pVertices_D[k++] = CDiffusedVertex(
				fRadius * sinf(phi_j) * cosf(theta_i), 
				fRadius * cosf(phi_j) * y_scale,
				fRadius * sinf(phi_j) * sinf(theta_i), 
				(rand() % 2 == 0 ? stone_color1 : stone_color2));
		}
	}
	//구의 아래(남극)를 나타내는 정점이다. 
	m_pVertices_D[k] = CDiffusedVertex(0.0f, -fRadius * y_scale, 0.0f, (rand() % 2 == 0 ? stone_color1 : stone_color2));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices_D,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	/*원기둥의 표면에 존재하는 사각형의 개수는 {nSlices * (nStacks-2)}이고 사각형은 2개의 삼각형으로 구성되므로
	삼각형 리스트일 때 필요한 인덱스의 개수는 {nSlices * (nStacks-2) * 2 * 3}이다. 그리고 구의 위아래 원뿔의 표면
	에 존재하는 삼각형의 개수는 nSlices개이므로 구의 위아래 원뿔을 표현하기 위한 인덱스의 개수는 {(nSlices * 3) *
	2}이다. 그러므로 구의 표면을 삼각형 리스트로 표현하기 위하여 필요한 인덱스의 개수는 {(nSlices * 3) * 2 +
	(nSlices * (nStacks - 2) * 3 * 2)}이다*/
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	k = 0;
	//구의 위쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	//구의 원기둥의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int j = 0; j < nStacks-2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			//사각형의 첫 번째 삼각형의 인덱스이다. 
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			//사각형의 두 번째 삼각형의 인덱스이다. 
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	//구의 아래쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;



	m_xmBoundingSphere = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), fRadius);
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius,fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CSphereMeshDiffused::~CSphereMeshDiffused()
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UIMesh::UIMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float layer_z, XMFLOAT4 xmf4Color, bool Color_Mix)
	: CMesh(pd3dDevice, pd3dCommandList)
{

	m_nVertices = 4;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = layer_z * 0.5f;

	XMFLOAT4 another_color;
	if (Color_Mix)
		another_color = { 1.0f - xmf4Color.x, 1.0f - xmf4Color.y , 1.0f - xmf4Color.z , 1.0f };
	else
		another_color = xmf4Color;


	m_pVertices_D = new CDiffusedVertex[m_nVertices];
	m_pVertices_D[0] = CDiffusedVertex(XMFLOAT3(+fx, +fy, fz), xmf4Color);
	m_pVertices_D[1] = CDiffusedVertex(XMFLOAT3(-fx, +fy, fz), another_color);
	m_pVertices_D[2] = CDiffusedVertex(XMFLOAT3(-fx, -fy, fz), another_color);
	m_pVertices_D[3] = CDiffusedVertex(XMFLOAT3(+fx, -fy, fz), xmf4Color);


	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices_D, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 6;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 0; m_pnIndices[1] = 2; m_pnIndices[2] = 1;
	m_pnIndices[3] = 0; m_pnIndices[4] = 3; m_pnIndices[5] = 2;


	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

}
UIMesh::~UIMesh()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, float* pfNearHitDistance)
{
	//하나의 메쉬에서 광선은 여러 개의 삼각형과 교차할 수 있다. 교차하는 삼각형들 중 가장 가까운 삼각형을 찾는다. 
	int nIntersections = 0;

	// 정점 형태에 따라서 설정
	BYTE* pbPositions = NULL;
		if (m_pVertices_I != NULL) 
			pbPositions = (BYTE*)m_pVertices_I;
		else 
			pbPositions = (BYTE*)m_pVertices_D;
		
	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;

	/*메쉬의 프리미티브(삼각형)들의 개수이다. 
	삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 
	삼각형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 – 2)이다.*/
	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	
	if (m_nIndices > 0) 
		nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	//광선은 모델 좌표계로 표현된다. 
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);

	//모델 좌표계의 광선과 메쉬의 바운딩 박스(모델 좌표계)와의 교차를 검사한다. 
	bool bIntersected = m_xmBoundingBox.Intersects(xmRayOrigin, xmRayDirection, *pfNearHitDistance);

	//모델 좌표계의 광선이 메쉬의 바운딩 박스와 교차하면 메쉬와의 교차를 검사한다. 
	if (bIntersected)
	{
		//nIntersections++;

		float fNearHitDistance = FLT_MAX;
		/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다.
		충돌하는 모든 삼각형을 찾아 광선의 시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/

		for (int i = 0; i < nPrimitives; i++)
		{
			XMVECTOR v0 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_nStride));
			XMVECTOR v1 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_nStride));
			XMVECTOR v2 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_nStride));
			
			float fHitDistance;
			BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
			
			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance)
				{
					*pfNearHitDistance = fNearHitDistance = fHitDistance;
				}
				nIntersections++;
			}
		}
	}
	return(nIntersections);
}



//===============================================

CMeshIlluminated::CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1]
			= pxmf3Normals[nIndex2]
			= Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals,
	XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],
					pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],
					pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);

			if (pnIndices)
				nIndex0 = pnIndices[nIndex0];

			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);

			if (pnIndices)
				nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);

			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}


//=========================================================================

CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth)
	: CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nIndices = 36;
	UINT* pnIndices = new UINT[m_nIndices];
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	XMFLOAT3 pxmf3Positions[8];
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[8];
	for (int i = 0; i < 8; i++)
		pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);


	CIlluminatedVertex pVertices[8];
	for (int i = 0; i < 8; i++)
		pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);


	//  피킹에 사용하기 위해, 정점 데이터만  별도로 저장
	m_pVertices_I = pVertices;
	m_pnIndices = pnIndices;

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;




	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{

}

//===============================================================================

CSphereMeshIlluminated::CSphereMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float fRadius, int nSlices, int nStacks, float y_scale) : CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(CIlluminatedVertex);
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	m_nVertices = 2 + (nSlices * (nStacks - 1));

	XMFLOAT3* pxmf3Positions = new XMFLOAT3[m_nVertices];
	XMFLOAT3* pxmf3Normals = new XMFLOAT3[m_nVertices];


	float fDeltaPhi = float(XM_PI / nStacks);
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	float theta_i, phi_j;

	int k = 0;


	//구의 위(북극)를 나타내는 정점
	pxmf3Positions[k++] = XMFLOAT3(0.0f, +fRadius * y_scale, 0.0f);

	//원기둥 표면의 정점
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			pxmf3Positions[k++] = XMFLOAT3(
				fRadius * sinf(phi_j) * cosf(theta_i),
				fRadius * cosf(phi_j) * y_scale,
				fRadius * sinf(phi_j) * sinf(theta_i)
				);
		}
	}

	//구의 아래(남극)를 나타내는 정점
	pxmf3Positions[k] = XMFLOAT3(0.0f, -fRadius * y_scale, 0.0f);

	for (int i = 0; i < m_nVertices; i++)
		pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);


	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	UINT* pnIndices = new UINT[m_nIndices];
	
	k = 0;
	//구의 위쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스 
	for (int i = 0; i < nSlices; i++)
	{
		pnIndices[k++] = 0;
		pnIndices[k++] = 1 + ((i + 1) % nSlices);
		pnIndices[k++] = 1 + i;
	}
	//구의 원기둥의 표면을 표현하는 삼각형들의 인덱스
	for (int j = 0; j < nStacks - 2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			//사각형의 첫 번째 삼각형의 인덱스이다. 
			pnIndices[k++] = 1 + (i + (j * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			//사각형의 두 번째 삼각형의 인덱스이다. 
			pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	//구의 아래쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스
	for (int i = 0; i < nSlices; i++)
	{
		pnIndices[k++] = (m_nVertices - 1);
		pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	// 법선 벡터 계산
	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);

	CIlluminatedVertex* pVertices = new CIlluminatedVertex[m_nVertices];
	for (int i = 0; i < m_nVertices; i++)
		pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);

	// 객체에 정점 정보 저장
	m_pVertices_I = pVertices;
	m_pnIndices = pnIndices;

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;


	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;


	m_xmBoundingSphere = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), fRadius);
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius, fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CSphereMeshIlluminated::~CSphereMeshIlluminated()
{
}

//===============================================================================

CPlaneMeshIlluminated::CPlaneMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
	float fWidth, float fDepth, int N_SubRect) : CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	int nSubRects = N_SubRect;

	float fHalfWidth = fWidth * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth / nSubRects;
	float fCellDepth = fDepth / nSubRects;
	
	m_nVertices = 0;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<CIlluminatedVertex> tempVertices;

	for (int i = 0; i < nSubRects; i++) {
		for (int j = 0; j < nSubRects; j++) {
			XMFLOAT3 v0(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v1(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth));
			XMFLOAT3 v2(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth));
			XMFLOAT3 v3(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth));

			XMFLOAT3 v_normal(0.0f, 1.0f, 0.0f);

			tempVertices.push_back(CIlluminatedVertex(v0, v_normal));
			tempVertices.push_back(CIlluminatedVertex(v2, v_normal));
			tempVertices.push_back(CIlluminatedVertex(v1, v_normal));

			tempVertices.push_back(CIlluminatedVertex(v0, v_normal));
			tempVertices.push_back(CIlluminatedVertex(v3, v_normal));
			tempVertices.push_back(CIlluminatedVertex(v2, v_normal));
		}
	}

	m_nVertices = tempVertices.size();
	m_pVertices_I = new CIlluminatedVertex[m_nVertices];
	std::copy(tempVertices.begin(), tempVertices.end(), m_pVertices_I);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices_I, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, 10.0f, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CPlaneMeshIlluminated::~CPlaneMeshIlluminated() {
	if (m_pVertices_I) delete[] m_pVertices_I;
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
}

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	BYTE* pHeightMapPixels = new BYTE[m_nWidth * m_nLength];

	// 파일을 열고 읽는다. 
	// 높이 맵 이미지는 파일 헤더가 없는 RAW 이미지이다.
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	// 이미지의 y-축과 지형의 z-축이 방향이 반대이므로 이미지를 상하대칭 시켜 저장 -> 이미지의 좌표축과 지형의 좌표축의 방향이 일치하게 됨
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
		for (int x = 0; x < m_nWidth; x++)
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] = pHeightMapPixels[x + (y * m_nWidth)];

	if (pHeightMapPixels)
		delete[] pHeightMapPixels;

}
CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	//x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 법선 벡터는 y-축 방향 벡터
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// 높이 맵에서 (x, z) 좌표의 픽셀 값과 인접한 두 개의 점 (x+1, z), (z, z+1)에 대한 픽셀 값을 사용하여 법선 벡터를  계산
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

	//(x, z), (x+1, z), (z, z+1)의 픽셀에서 지형의 높이를 구한다.
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;

	// xmf3Edge1은 (0, y3, m_xmf3Scale.z) - (0, y1, 0) 벡터
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);

	// xmf3Edge2는 (m_xmf3Scale.x, y2, 0) - (0, y1, 0) 벡터
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);

	//법선 벡터는 xmf3Edge1과 xmf3Edge2의 외적을 정규화하면 됨
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMapImage::GetHeight(float fx, float fz)
{
	// 지형의 좌표 (fx, fz)는 이미지 좌표계
	// 높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 높이는 0

	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);

	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.
	int x = (int)fx;
	int z = (int)fz;

	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)];

	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)];

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER

	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft)
	{
		// 지형의 삼각형들이 오른쪽에서 왼쪽 방향으로 나열되는 경우
			if (fzPercent >= fxPercent)
				fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
			else
				fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else 
	{
		// 지형의 삼각형들이 왼쪽에서 오른쪽 방향으로 나열되는 경우 
			if (fzPercent < (1.0f - fxPercent))
				fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
			else
				fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif

	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight);
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, 
	XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext) : CMesh(pd3dDevice, pd3dCommandList)
{
	//격자의 교점(정점)의 개수는 (nWidth * nLength)이다.
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(CDiffusedVertex);
	//격자는 삼각형 스트립으로 구성한다.
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	CDiffusedVertex* pVertices = new CDiffusedVertex[m_nVertices];
	/*xStart와 zStart는 격자의 시작 위치(x-좌표와 z-좌표)를 나타낸다. 커다란 지형은 격자들의 이차원 배열로 만들 필
   요가 있기 때문에 전체 지형에서 각 격자의 시작 위치를 나타내는 정보가 필요하다.*/

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			//정점의 높이와 색상을 높이 맵으로부터 구한다.
			XMFLOAT3 xmf3Position = XMFLOAT3((x * m_xmf3Scale.x), OnGetHeight(x, z, pContext), (z * m_xmf3Scale.z));
			XMFLOAT4 xmf3Color = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			pVertices[i] = CDiffusedVertex(xmf3Position, xmf3Color);

			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	delete[] pVertices;

	// 인덱스를 나열하면 인덱스 버퍼는 ((nWidth*2)*(nLength-1))+((nLength-1)-1)개의 인덱스를 가짐
	// 사각형 줄의 개수는 (nLength-1)이고 한 줄에서 (nWidth*2)개의 인덱스를 가짐
	// 줄이 바뀔 때마다 인덱스를 하나 추가하므로 (nLength-1)-1개의 인덱스가 추가로 필요

	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	UINT* pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향
			for (int x = 0; x < nWidth; x++)
			{
				//첫 번째 줄을 제외하고 줄이 바뀔 때마다(x == 0) 첫 번째 인덱스를 추가
				if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다.
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//줄이 바뀔 때마다(x == (nWidth-1)) 첫 번째 인덱스를 추가
				if (x == (nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

//높이 맵 이미지의 픽셀 값을 지형의 높이로 반환
float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();

	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;

	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext)
{
	//조명의 방향 벡터(정점에서 조명까지의 벡터)
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	//조명의 색상(세기, 밝기)
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	/*정점 (x, z)에서 조명이 반사되는 양(비율)은 정점 (x, z)의 법선 벡터와 조명의 방향 벡터의 내적(cos)과 
	인접한 3개의 정점 (x+1, z), (x, z+1), (x+1, z+1)의 법선 벡터와 조명의 방향 벡터의 내적을 평균하여 구한다. 
	정점 (x, z)의 색상은 조명 색상(세기)과 반사되는 양(비율)을 곱한 값이다.*/

	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z),xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z),xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1),xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1),xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;

	if (fScale > 1.0f) 
		fScale = 1.0f;

	if (fScale < 0.25f)
		fScale = 0.25f;

	//fScale은 조명 색상(밝기)이 반사되는 비율
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);

	return(xmf4Color);
}