//=============================================================================
//
// 石処理 [stone.cpp]
// Author : 
//
//=============================================================================
#include "stone.h"
#include "model.h"
#include "camera.h"
#include "texture.h"
#include "shadow.h"
#include "polyline.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define STONE_SCL		(8.0f)	// 石の大きさ(スケール)
#define SPAWN_INTERVAL	(0.5f)	// 石の生成間隔(秒)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_MODEL	g_Model;	// モデル読み込み
static ID3D11Buffer		*g_VertexBuffer = NULL;	// 頂点バッファ

static STONE		g_Stone[STONE_NUM];	// 石データ
static MATERIAL		g_Material;
static int			g_texNo = 0;
int g_stoneFrame = 0;


//=============================================================================
// 初期化処理
//=============================================================================
void InitStone(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 頂点バッファに値をセットする
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		float width = 50.0f;
		float height = 50.0f;

		// 頂点座標の設定
		vertex[0].Position = D3DXVECTOR3(-width / 2.0f, 0.0f, 0.0f);
		vertex[1].Position = D3DXVECTOR3(-width / 2.0f, height, 0.0f);
		vertex[2].Position = D3DXVECTOR3(width / 2.0f, 0.0f, 0.0f);
		vertex[3].Position = D3DXVECTOR3(width / 2.0f, height, 0.0f);

		// 頂点カラーの設定
		vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
		vertex[1].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
		vertex[2].TexCoord = D3DXVECTOR2(1.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	// テクスチャ生成
	g_texNo = LoadTexture((char*)"data/TEXTURE/nature_stone_ishi.png");

	// マテリアル初期化
	ZeroMemory(&g_Material, sizeof(g_Material));
	g_Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//objモデルの読み込み
	LoadModel((char*)"data/MODEL/stone_op_02.obj", &g_Model);

	// 初期化
	for(int i = 0; i < STONE_NUM; i++)
	{
		g_Stone[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Stone[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Stone[i].scl = D3DXVECTOR3(STONE_SCL, STONE_SCL, STONE_SCL);
		g_Stone[i].size = D3DXVECTOR3(STONE_SIZE, STONE_SIZE, STONE_SIZE);
		g_Stone[i].use = false;
	}
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStone(void)
{
	if(g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	//objモデルの解放
	UnloadModel(&g_Model);
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStone(void)
{
	for(int i = 0; i < STONE_NUM; i++)
	{
		if(g_Stone[i].use)
		{
			//消滅処理
			if (g_Stone[i].pos.y - g_Stone[i].size.y / 2 <= -10.0f)
			{
				g_Stone[i].use = false;

				//影を消す
				ReleaseShadow(g_Stone[i].shadow);

			}

			//座標の更新
			g_Stone[i].pos.y += g_Stone[i].speed;

			//影の座標を更新する
			SetPositionShadow(g_Stone[i].shadow, D3DXVECTOR3(g_Stone[i].pos.x, 0.0f, g_Stone[i].pos.z));
			SetSizeShadow(g_Stone[i].shadow, 
				g_Stone[i].shadowSize * ((STONE_SPAWN_HEIGHT - g_Stone[i].pos.y) / 400.0f));
		}
	}

	// 石の生成
	if (g_stoneFrame >= 60 * SPAWN_INTERVAL) 
	{
		SetStone(D3DXVECTOR3(Random(-SPAWN_SIZE_HALF,SPAWN_SIZE_HALF), 
			STONE_SPAWN_HEIGHT, Random(-SPAWN_SIZE_HALF,SPAWN_SIZE_HALF)));
		g_stoneFrame = 0;
	}

	g_stoneFrame++;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawStone(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	for (int i = 0; i < STONE_NUM; i++) 
	{
		//useフラグがオフの場合はスキップする
		if (!g_Stone[i].use)
			continue;

		// ワールドマトリックスの初期化（単位行列を作る関数）
		D3DXMatrixIdentity(&g_Stone[i].mtxWorld);

		// スケールを反映
		D3DXMatrixScaling(&mtxScl, g_Stone[i].scl.x, g_Stone[i].scl.y, g_Stone[i].scl.z);
		D3DXMatrixMultiply(&g_Stone[i].mtxWorld, &g_Stone[i].mtxWorld, &mtxScl);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Stone[i].rot.y, g_Stone[i].rot.x, g_Stone[i].rot.z);
		D3DXMatrixMultiply(&g_Stone[i].mtxWorld, &g_Stone[i].mtxWorld, &mtxRot);

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, g_Stone[i].pos.x, g_Stone[i].pos.y, g_Stone[i].pos.z);
		D3DXMatrixMultiply(&g_Stone[i].mtxWorld, &g_Stone[i].mtxWorld, &mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&g_Stone[i].mtxWorld);

		//objモデルの描画
		DrawModel(&g_Model);
	}

//	// αテストを有効に
//	SetAlphaTestEnable(true);
//
//	// ライティングを無効
//	SetLightEnable(false);
//
//	D3DXMATRIX mtxView, mtxScale, mtxTranslate;
//
//	// 頂点バッファ設定
//	UINT stride = sizeof(VERTEX_3D);
//	UINT offset = 0;
//	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
//
//	// プリミティブトポロジ設定
//	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//	CAMERA *cam = GetCamera();
//
//	for(int i = 0; i < STONE_NUM; i++)
//	{
//		if(g_Stone[i].use)
//		{
//			// ワールドマトリックスの初期化
//			D3DXMatrixIdentity(&g_Stone[i].mtxWorld);
//
//
//			// ポリゴンを正面に向ける
//#if 0
//			// ビューマトリックスの逆行列を取得
//			g_Stone[i].mtxWorld = cam->mtxInvView;
//			g_Stone[i].mtxWorld._41 = 0.0f;
//			g_Stone[i].mtxWorld._42 = 0.0f;
//			g_Stone[i].mtxWorld._43 = 0.0f;
//#else
//			// ビューマトリックスを取得
//			D3DXMATRIX mtxView = cam->mtxView;
//
//			g_Stone[i].mtxWorld._11 = mtxView._11;
//			g_Stone[i].mtxWorld._12 = mtxView._21;
//			g_Stone[i].mtxWorld._13 = mtxView._31;
//			g_Stone[i].mtxWorld._21 = mtxView._12;
//			g_Stone[i].mtxWorld._22 = mtxView._22;
//			g_Stone[i].mtxWorld._23 = mtxView._32;
//			g_Stone[i].mtxWorld._31 = mtxView._13;
//			g_Stone[i].mtxWorld._32 = mtxView._23;
//			g_Stone[i].mtxWorld._33 = mtxView._33;
//#endif
//
//
//			// 移動を反映
//			D3DXMatrixTranslation(&mtxTranslate, g_Stone[i].pos.x,
//												 g_Stone[i].pos.y,
//												 g_Stone[i].pos.z);
//			D3DXMatrixMultiply(&g_Stone[i].mtxWorld, &g_Stone[i].mtxWorld, &mtxTranslate);
//
//			// ワールドマトリックスの設定
//			SetWorldMatrix(&g_Stone[i].mtxWorld);
//
//			// マテリアル設定
//			SetMaterial(g_Material);
//
//			// テクスチャ設定
//			GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo));
//
//			// ポリゴンの描画
//			GetDeviceContext()->Draw(4, 0);
//		}
//	}
//
//	// ライティングを有効に
//	SetLightEnable(true);
//
//	// αテストを無効に
//	SetAlphaTestEnable(false);
}

//=============================================================================
// 石のパラメータをセット
//=============================================================================
void SetStone(D3DXVECTOR3 pos)
{
	for(int i = 0; i < STONE_NUM; i++)
	{
		//まだ表示されていない場所を探す
		if(g_Stone[i].use == false)
		{
			g_Stone[i].pos = pos;

			g_Stone[i].speed = -Random(3,6);

			// 当たり判定用サイズを設定
			g_Stone[i].size = D3DXVECTOR3(STONE_SIZE, STONE_SIZE, STONE_SIZE);

			// 影の設定
			g_Stone[i].shadowSize = 80.0f;

			g_Stone[i].shadow = SetShadow(D3DXVECTOR3(g_Stone[i].pos.x, 0.0f, g_Stone[i].pos.z), g_Stone[i].shadowSize);

			g_Stone[i].use = true;

			break;
		}
	}
}


STONE* GetStone(void)
{
	return &g_Stone[0];
}
