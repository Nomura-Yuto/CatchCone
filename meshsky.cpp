//=============================================================================
//
// メッシュ空の処理 [meshsky.cpp]
// Author : 
//
//=============================================================================
#include "meshsky.h"
#include "Texture.h"
#include "input.h"
#include "camera.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	SKY_HEIGHT_RATE			(2.0f)		// 空ドームの高さ係数
#define	TEX_COUNT_LOOP			(1)			// テクスチャの繰り返し回数

struct SKY_MESH
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
static int			g_TexNo;	// テクスチャ読み込み場所

static SKY_MESH		g_Sky;		//周回部分
static SKY_MESH		g_SkyTop;	//天頂部分

static bool			g_TransParent;
static bool			g_Reverse;

//=============================================================================
// 初期化処理
//=============================================================================
void InitMeshSky(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float radius,
	int numBlockH, int numBlockV, bool reverse)
{
	ID3D11Device* pDevice = GetDevice();

	g_TransParent = false;
	g_Reverse = reverse;

	// ポリゴン表示位置の中心座標を設定
	g_Sky.pos = pos;
	g_Sky.rot = rot;
	g_SkyTop.pos = pos;
	g_SkyTop.rot = rot;


	// テクスチャの読み込み
	g_TexNo = LoadTexture((char*)"data/TEXTURE/sky000.jpg");

	//マテリアルの初期化
	ZeroMemory(&g_Sky.material, sizeof(g_Sky.material));
	g_Sky.material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	ZeroMemory(&g_SkyTop.material, sizeof(g_SkyTop.material));
	g_SkyTop.material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// 頂点数の設定
	g_Sky.numVertex = (numBlockH + 1) * (numBlockV + 1);
	g_SkyTop.numVertex = numBlockH + 1;

	// インデックス数の設定
	g_Sky.numIndex = (numBlockH * (numBlockV * 2) + numBlockV * 2 + (numBlockV - 1) * 2);
	g_SkyTop.numIndex = numBlockH * 3;

	// 一時頂点配列を生成
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_Sky.numVertex];
	VERTEX_3D* pVertexTopWk = new VERTEX_3D[g_SkyTop.numVertex];

	// 一時インデックス配列を生成
	int* pIndexWk = new int[g_Sky.numIndex];
	int* pIndexTopWk = new int[g_SkyTop.numIndex];

	// 頂点配列の中身を埋める
	VERTEX_3D* pVtx = pVertexWk;
	const float ANGLE_H = (D3DX_PI * 2) / numBlockH;
	const float ANGLE_V = (D3DX_PI / 8) / (numBlockV + 1);
	const float WIDTH = 1.0f / numBlockH;
	const float HEIGHT = 1.0f / numBlockV;
	float lengthXZ;

	for (int vt = 0; vt < (numBlockV + 1); vt++) {
		lengthXZ = cosf(ANGLE_V * vt) * radius;

		for (int hr = 0; hr < (numBlockH + 1); hr++) {
			// 頂点座標の設定
			pVtx->Position.x = sinf(ANGLE_H * hr) * lengthXZ;
			if (g_Reverse) {
				pVtx->Position.y = -sinf(ANGLE_V * vt) * radius * SKY_HEIGHT_RATE;
			} else {
				pVtx->Position.y = sinf(ANGLE_V * vt) * radius * SKY_HEIGHT_RATE;
			}
			pVtx->Position.z = cosf(ANGLE_H * hr) * lengthXZ;

			// 法線の設定
			pVtx->Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 反射光の設定
			if (g_Reverse) {
				pVtx->Diffuse = D3DXCOLOR(0.0625f, 0.0625f, 0.375f, 0.5f);
			} else {
				pVtx->Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}

			// テクスチャ座標の設定
			pVtx->TexCoord.x = hr * WIDTH * TEX_COUNT_LOOP;
			if (vt == numBlockV) {
				pVtx->TexCoord.y = 0.01f;
			} else {
				pVtx->TexCoord.y = 1.0f - vt * HEIGHT;
			}

			pVtx++;
		}
	}

	pVtx = pVertexTopWk;

	// 頂点座標の設定
	pVtx->Position.x = 0.0f;
	if (g_Reverse) {
		pVtx->Position.y = -sinf(ANGLE_V * (numBlockV + 1)) * radius * SKY_HEIGHT_RATE;
	} else {
		pVtx->Position.y = sinf(ANGLE_V * (numBlockV + 1)) * radius * SKY_HEIGHT_RATE;
	}
	pVtx->Position.z = 0.0f;

	// 法線の設定
	pVtx->Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 頂点カラーの設定
	if (g_Reverse) {
		pVtx->Diffuse = D3DXCOLOR(0.0675f, 0.0675f, 0.375f, 0.5f);
	} else {
		pVtx->Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// テクスチャ座標の設定
	pVtx->TexCoord.x = 0.01f;
	pVtx->TexCoord.y = 0.01f;

	pVtx++;

	lengthXZ = cosf(ANGLE_V * numBlockV) * radius;
	for (int hr = 0; hr < numBlockH; hr++ ) {
		// 頂点座標の設定
		pVtx->Position.x = -sinf(ANGLE_H * hr) * lengthXZ;
		if (g_Reverse) {
			pVtx->Position.y = -sinf(ANGLE_V * numBlockV) * radius * SKY_HEIGHT_RATE;
		} else {
			pVtx->Position.y = sinf(ANGLE_V * numBlockV) * radius * SKY_HEIGHT_RATE;
		}
		pVtx->Position.z = cosf(ANGLE_H * hr) * lengthXZ;

		// 法線の設定
		pVtx->Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラーの設定
		if (g_Reverse) {
			pVtx->Diffuse = D3DXCOLOR(0.0675f, 0.0675f, 0.375f, 0.5f);
		} else {
			pVtx->Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// テクスチャ座標の設定
		pVtx->TexCoord.x = 0.01f;
		pVtx->TexCoord.y = 0.01f;

		pVtx++;
	}

	// インデックス配列の中身を埋める
	int* pIdx = pIndexWk;

	for (int vt = 0; vt < numBlockV; vt++) {
		if (vt != 0) {
			*pIdx = vt * (numBlockH + 1);
			pIdx++;
		}

		for (int hr = 0; hr < numBlockH + 1; hr++) {
			*pIdx = vt * (numBlockH + 1) + hr;
			pIdx++;

			*pIdx = (vt + 1) * (numBlockH + 1) + hr;
			pIdx++;
		}

		if (vt != (numBlockV - 1)) {
			*pIdx = (vt + 1) * (numBlockH + 1);
			pIdx++;
		}
	}

	pIdx = pIndexTopWk;
	for (int hr = 1; hr < numBlockH + 1; hr++) {
		*pIdx = 0;
		++pIdx;
		*pIdx = hr;
		++pIdx;
		if (hr < numBlockH) {
			*pIdx = hr + 1;
		} else {
			*pIdx = 1;
		}
		++pIdx;
	}

	//周回部分の頂点バッファ・インデックスバッファ作成
	{
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_Sky.numVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pVertexWk;

		GetDevice()->CreateBuffer(&bd, &sd, &g_Sky.vertexBuffer);

		// インデックスバッファ生成
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(int) * g_Sky.numIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pIndexWk;

		GetDevice()->CreateBuffer(&bd, &sd, &g_Sky.indexBuffer);
	}

	//天頂部分の頂点バッファ・インデックスバッファ作成
	{
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_SkyTop.numVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pVertexTopWk;

		GetDevice()->CreateBuffer(&bd, &sd, &g_SkyTop.vertexBuffer);

		// インデックスバッファ生成
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * g_SkyTop.numIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pIndexTopWk;

		GetDevice()->CreateBuffer(&bd, &sd, &g_SkyTop.indexBuffer);
	}

	// 一時配列を解放
	delete[] pIndexTopWk;
	delete[] pIndexWk;
	delete[] pVertexTopWk;
	delete[] pVertexWk;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshSky(void)
{
	// 周回部分メッシュの解放
	if (g_Sky.indexBuffer != NULL)
	{
		g_Sky.indexBuffer->Release();
		g_Sky.indexBuffer = NULL;
	}

	if (g_Sky.vertexBuffer != NULL)
	{
		g_Sky.vertexBuffer->Release();
		g_Sky.vertexBuffer = NULL;
	}

	// 天頂部分メッシュの解放
	if (g_SkyTop.indexBuffer != NULL)
	{
		g_SkyTop.indexBuffer->Release();
		g_SkyTop.indexBuffer = NULL;
	}

	if (g_SkyTop.vertexBuffer != NULL)
	{
		g_SkyTop.vertexBuffer->Release();
		g_SkyTop.vertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshSky(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshSky(void)
{
	if (g_Reverse) 
	{
		SetCullingMode(CULL_MODE_FRONT);	// 前面カリング
	}
	else 
	{
		SetCullingMode(CULL_MODE_BACK);		// 背面カリング
	}

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_TexNo));

	//周回部分の描画
	{
		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_Sky.vertexBuffer, &stride, &offset);

		// インデックスバッファ設定
		GetDeviceContext()->IASetIndexBuffer(g_Sky.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// マテリアル設定
		SetMaterial(g_Sky.material);


		D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_Sky.mtxWorld);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Sky.rot.y, g_Sky.rot.x, g_Sky.rot.z);
		D3DXMatrixMultiply(&g_Sky.mtxWorld, &g_Sky.mtxWorld, &mtxRot);

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, g_Sky.pos.x, g_Sky.pos.y, g_Sky.pos.z);
		D3DXMatrixMultiply(&g_Sky.mtxWorld, &g_Sky.mtxWorld, &mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&g_Sky.mtxWorld);

		// ポリゴンの描画
		GetDeviceContext()->DrawIndexed(g_Sky.numIndex, 0, 0);
	}

	//天頂部分の描画
	{
		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_SkyTop.vertexBuffer, &stride, &offset);

		// インデックスバッファ設定
		GetDeviceContext()->IASetIndexBuffer(g_SkyTop.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// マテリアル設定
		SetMaterial(g_SkyTop.material);


		D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_SkyTop.mtxWorld);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_SkyTop.rot.y, g_SkyTop.rot.x, g_SkyTop.rot.z);
		D3DXMatrixMultiply(&g_SkyTop.mtxWorld, &g_SkyTop.mtxWorld, &mtxRot);

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, g_SkyTop.pos.x, g_SkyTop.pos.y, g_SkyTop.pos.z);
		D3DXMatrixMultiply(&g_SkyTop.mtxWorld, &g_SkyTop.mtxWorld, &mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&g_SkyTop.mtxWorld);

		// ポリゴンの描画
		GetDeviceContext()->DrawIndexed(g_SkyTop.numIndex, 0, 0);
	}

	SetCullingMode(CULL_MODE_BACK);		// 背面カリング
}
