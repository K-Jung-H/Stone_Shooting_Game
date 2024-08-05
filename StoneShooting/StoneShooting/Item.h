#pragma once
#include "GameObject.h"

extern CMaterial* material_color_item_outer;
extern CMaterial* material_color_item_inner_red;
extern CMaterial* material_color_item_inner_green;
extern CMaterial* material_color_item_inner_blue;

class Item : public CRotatingObject
{
private:
	Item_Type item_type = Item_Type::ETC;

protected:
	CRotatingObject* outer_frame = NULL;
	CGameObject* inner_frame = NULL;

public:

	static CMesh* outer_Mesh;
	static CMesh* inner_Mesh;
	static void Prepare_Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Item_Type type = Item_Type::ETC);
	virtual ~Item();

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader);
};