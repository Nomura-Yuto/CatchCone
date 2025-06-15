//=============================================================================
//
// メッシュ山処理 [meshmountain.cpp]
// Author :
//
//=============================================================================
#include "meshmountain.h"
#include "Texture.h"
#include "input.h"
#include "camera.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct MOUNTAIN_MESH
{
	ID3D11Buffer	*vertexBuffer;	// 頂点バッファ
	ID3D11Buffer	*indexBuffer;	// インデックスバッファ
	int				numVertex;		//頂点数
	int				numIndex;		//インデックス数

	D3DXMATRIX		mtxWorld;
	D3DXVECTOR3		pos;
	D3DXVECTOR3		rot;

	MATERIAL		material;
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static MOUNTAIN_MESH	g_Mountain;				// メッシュ情報
static int				g_TexNo;				// テクスチャ識別子


//=============================================================================
// 初期化処理
//=============================================================================
void InitMeshMountain(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
	int numBlockH, int numBlockV, float sizeBlockH, float sizeBlockV,
	float texSizeX, float texSizeZ)
{
	ID3D11Device* pDevice = GetDevice();

	// ポリゴン表示位置の中心座標を設定
	g_Mountain.pos = pos;
	g_Mountain.rot = rot;

	// テクスチャの読み込み
	g_TexNo = LoadTexture((char*)"data/TEXTURE/mountain000.png");

	//マテリアルの初期化
	ZeroMemory(&g_Mountain.material, sizeof(g_Mountain.material));
	g_Mountain.material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// 頂点数の設定
	g_Mountain.numVertex = (numBlockH + 1) * (numBlockV + 1);

	// インデックス数の設定
	g_Mountain.numIndex = (numBlockH + 1) * (numBlockV * 2) + (numBlockV - 1);

	// 一時頂点配列を生成
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_Mountain.numVertex];

	// 一時インデックス配列を生成
	int* pIndexWk = new int[g_Mountain.numIndex];

	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;
	const float ANGLE_H = (D3DX_PI * 2) / numBlockH;

	for (int vt = 0; vt < (numBlockV + 1); vt++) {
		for (int hr = 0; hr < (numBlockH + 1); hr++) {
			// 頂点座標の設定
			pVtx->Position.x = sinf(ANGLE_H * hr) * sizeBlockH;
			pVtx->Position.y = vt * sizeBlockV;
			pVtx->Position.z = cosf(ANGLE_H * hr) * sizeBlockH;

			// 法線の設定
			pVtx->Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 反射光の設定
			pVtx->Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx->TexCoord.x = hr * texSizeX;
			pVtx->TexCoord.y = 0.975f - vt * 0.95f * texSizeZ;

			pVtx++;
		}
	}

	// インデックス配列の中身を埋める
	int* pIdx = pIndexWk;

	for (int vt = 0; vt < numBlockV; vt++) {
		for (int hr = 0; hr < (numBlockH + 1); hr++) {
			*pIdx = vt * (numBlockH + 1) + hr;
			pIdx++;

			*pIdx = (vt + 1) * (numBlockH + 1) + hr;
			pIdx++;
		}
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_Mountain.numVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pVertexWk;

	GetDevice()->CreateBuffer(&bd, &sd, &g_Mountain.vertexBuffer);


	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * g_Mountain.numIndex;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pIndexWk;

	GetDevice()->CreateBuffer(&bd, &sd, &g_Mountain.indexBuffer);


	// 一時配列の解放
	delete[] pIndexWk;
	delete[] pVertexWk;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshMountain(void)
{
	//インデックスバッファの解放
	if (g_Mountain.indexBuffer != NULL)
	{
		g_Mountain.indexBuffer->Release();
		g_Mountain.indexBuffer = NULL;
	}

	//頂点バッファの解放
	if (g_Mountain.vertexBuffer != NULL)
	{
		g_Mountain.vertexBuffer->Release();
		g_Mountain.vertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshMountain(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshMountain(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_Mountain.vertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_Mountain.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	SetMaterial(g_Mountain.material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_TexNo));


	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_Mountain.mtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Mountain.rot.y, g_Mountain.rot.x, g_Mountain.rot.z);
	D3DXMatrixMultiply(&g_Mountain.mtxWorld, &g_Mountain.mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_Mountain.pos.x, g_Mountain.pos.y, g_Mountain.pos.z);
	D3DXMatrixMultiply(&g_Mountain.mtxWorld, &g_Mountain.mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&g_Mountain.mtxWorld);

	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_Mountain.numIndex, 0, 0);
}
