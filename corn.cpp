//=============================================================================
//
// コーン処理 [corn.cpp]
// Author : 
//
//=============================================================================
#include "corn.h"
#include "camera.h"
#include "texture.h"
#include "shadow.h"
#include "score.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(128)	// キャラサイズ
#define TEXTURE_HEIGHT				(128)	// 

#define SPAWN_INTERVAL	(0.3f)	// コーンの生成間隔(秒)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer		*g_VertexBuffer = NULL;	// 頂点バッファ

static CORN		g_Corn[CORN_NUM];	// コーンデータ
static MATERIAL		g_Material;
static int			g_texNo1 = 0;
static int			g_texNo2 = 0;
static int			g_texNo3 = 0;

int g_cornFrame = 0;
int g_haveCorn = 0;		// コーンの所持数
int g_cornScore = 0;	// 所持スコア

static float					g_w, g_h;					// 幅と高さ
static D3DXVECTOR3				g_Pos;						// ポリゴンの座標
//=============================================================================
// 初期化処理
//=============================================================================
void InitCorn(void)
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

		float width = CORN_SIZE;
		float height = CORN_SIZE;

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
	g_texNo1 = LoadTexture((char*)"data/TEXTURE/popcorn_01.png");
	g_texNo2 = LoadTexture((char*)"data/TEXTURE/popcorn_02.png");
	g_texNo3 = LoadTexture((char*)"data/TEXTURE/popcorn_03.png");

	// マテリアル初期化
	ZeroMemory(&g_Material, sizeof(g_Material));
	g_Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// 初期化
	for(int i = 0; i < CORN_NUM; i++)
	{
		g_Corn[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Corn[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Corn[i].size = D3DXVECTOR3(CORN_SIZE, CORN_SIZE, CORN_SIZE);
		g_Corn[i].use = false;
		g_Corn[i].num = Random(1, 3);
	}

	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = D3DXVECTOR3(SCREEN_WIDTH / 2 - TEXTURE_WIDTH * 3, SCREEN_HEIGHT - TEXTURE_HEIGHT, 0.0f);
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCorn(void)
{
	if(g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCorn(void)
{
	for(int i = 0; i < CORN_NUM; i++)
	{
		if(g_Corn[i].use)
		{
			//消滅処理
			if (g_Corn[i].pos.y - g_Corn[i].size.y / 2 <= -10.0f)
			{
				g_Corn[i].use = false;

				//影を消す
				ReleaseShadow(g_Corn[i].shadow);
			}

			//座標の更新
			g_Corn[i].pos.y += g_Corn[i].speed;

			//影の座標を更新する
			SetPositionShadow(g_Corn[i].shadow, D3DXVECTOR3(g_Corn[i].pos.x, 0.0f, g_Corn[i].pos.z));

			SetSizeShadow(g_Corn[i].shadow, 
				g_Corn[i].shadowSize * ((CORN_SPAWN_HEIGHT - g_Corn[i].pos.y) / 400.0f));
		}
	}

	// コーンの生成
	if (g_cornFrame >= 60 * SPAWN_INTERVAL) 
	{
		SetCorn(D3DXVECTOR3(Random(-SPAWN_SIZE_HALF,SPAWN_SIZE_HALF), 
			CORN_SPAWN_HEIGHT, Random(-SPAWN_SIZE_HALF,SPAWN_SIZE_HALF)));
		g_cornFrame = 0;
	}

	g_cornFrame++;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCorn(void)
{
	// αテストを有効に
	SetAlphaTestEnable(true);

	// ライティングを無効
	SetLightEnable(false);

	D3DXMATRIX mtxView, mtxScale, mtxTranslate;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	CAMERA *cam = GetCamera();

	for(int i = 0; i < CORN_NUM; i++)
	{
		if(g_Corn[i].use)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_Corn[i].mtxWorld);


			// ポリゴンを正面に向ける
#if 0
			// ビューマトリックスの逆行列を取得
			g_Corn[i].mtxWorld = cam->mtxInvView;
			g_Corn[i].mtxWorld._41 = 0.0f;
			g_Corn[i].mtxWorld._42 = 0.0f;
			g_Corn[i].mtxWorld._43 = 0.0f;
#else
			// ビューマトリックスを取得
			D3DXMATRIX mtxView = cam->mtxView;

			g_Corn[i].mtxWorld._11 = mtxView._11;
			g_Corn[i].mtxWorld._12 = mtxView._21;
			g_Corn[i].mtxWorld._13 = mtxView._31;
			g_Corn[i].mtxWorld._21 = mtxView._12;
			g_Corn[i].mtxWorld._22 = mtxView._22;
			g_Corn[i].mtxWorld._23 = mtxView._32;
			g_Corn[i].mtxWorld._31 = mtxView._13;
			g_Corn[i].mtxWorld._32 = mtxView._23;
			g_Corn[i].mtxWorld._33 = mtxView._33;
#endif


			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_Corn[i].pos.x,
												 g_Corn[i].pos.y,
												 g_Corn[i].pos.z);
			D3DXMatrixMultiply(&g_Corn[i].mtxWorld, &g_Corn[i].mtxWorld, &mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&g_Corn[i].mtxWorld);

			// マテリアル設定
			SetMaterial(g_Material);

			// テクスチャ設定
			switch (g_Corn[i].num) {
			case 1:
				GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo1));
				break;
			case 2:
				GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo2));
				break;
			case 3:
				GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo3));
				break;
			default:
				break;
			}

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// αテストを無効に
	SetAlphaTestEnable(false);


	// 桁数分処理する
	for (int i = 0; i < g_haveCorn; i++)
	{
		// 体力の位置やテクスチャー座標を反映
		float px = g_Pos.x + g_w * i;	// 表示位置X
		float py = g_Pos.y;				// 表示位置Y
		float pw = g_w;					// 表示幅
		float ph = g_h;					// 表示高さ

		float tw = 1.0f / 1;		// テクスチャの幅
		float th = 1.0f / 1;		// テクスチャの高さ
		float tx = 0.0f;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo1));

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		DrawSprite(g_texNo1, px, py, pw, ph, tx, ty, tw, th);
	}
}

//=============================================================================
// コーンのパラメータをセット
//=============================================================================
void SetCorn(D3DXVECTOR3 pos)
{
	for(int i = 0; i < CORN_NUM; i++)
	{
		//まだ表示されていない場所を探す
		if(g_Corn[i].use == false)
		{
			g_Corn[i].pos = pos;

			g_Corn[i].speed = -Random(2,7);

			// 当たり判定用サイズを設定
			g_Corn[i].size = D3DXVECTOR3(CORN_SIZE, CORN_SIZE, CORN_SIZE);

			// 影の設定
			g_Corn[i].shadowSize = 80.0f;

			g_Corn[i].shadow = SetShadow(D3DXVECTOR3(g_Corn[i].pos.x, 0.0f, g_Corn[i].pos.z), g_Corn[i].shadowSize);

			g_Corn[i].use = true;

			break;
		}
	}
}


CORN* GetCorn(void)
{
	return &g_Corn[0];
}

//=============================================================================
// コーンの所持数の加算・減算
//=============================================================================
void AddCorn(int add)
{
	g_haveCorn += add;

	if (g_haveCorn >= CORN_HAVE_MAX) g_haveCorn = CORN_HAVE_MAX;
}

//=============================================================================
// コーンの所持数のクリア
//=============================================================================
void ClearCorn(void)
{
	g_haveCorn = 0;
}

//=============================================================================
// 所持スコアの取得
//=============================================================================
int GetCornScore(void) 
{
	return g_cornScore;
}

//=============================================================================
// 所持スコアの加算・減算
//=============================================================================
void AddCornScore(int add)
{
	if (g_haveCorn >= CORN_HAVE_MAX) return;
	g_cornScore += add;
}

//=============================================================================
// 所持スコアのクリア
//=============================================================================
void ClearCornScore(void)
{
	g_cornScore = 0;
}

