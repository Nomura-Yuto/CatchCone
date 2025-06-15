//=============================================================================
//
// ポリライン処理 [polyline.cpp]
// Author : 
//
//=============================================================================
#include "polyline.h"
#include "texture.h"
#include "camera.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_POLYLINE	(512)	// ポリライン最大数


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct POLYLINE
{
	D3DXMATRIX	mtxWorld;	// ワールドマトリックス

	D3DXVECTOR3 begin;		// ポリライン始点
	D3DXVECTOR3 end;		// ポリライン終点
	float		width;		// ポリラインの幅
	MATERIAL	material;	// マテリアル

	bool		use;		// 使用しているかどうか
};


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*	g_VertexBuffer;				// 頂点バッファ
static POLYLINE			g_Polyline[MAX_POLYLINE];	// ポリライン配列
static int				g_TexNo;					// テクスチャ識別子


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void CalcVertexBuffer(int idx);


//=============================================================================
// 初期化処理
//=============================================================================
void InitPolyLine()
{
	ID3D11Device* pDevice = GetDevice();

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 6;//頂点は6個で固定
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// テクスチャ読み込み
	g_TexNo = LoadTexture((char*)"data/TEXTURE/polyline000.png");

	// ポリライン配列初期化
	for (int i = 0; i < MAX_POLYLINE; i++)
	{
		D3DXMatrixIdentity(&g_Polyline[i].mtxWorld);

		g_Polyline[i].begin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Polyline[i].end = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Polyline[i].width = 1.0f;

		g_Polyline[i].use = false;

		// マテリアルの初期設定
		ZeroMemory(&g_Polyline[i].material, sizeof(MATERIAL));
		g_Polyline[i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPolyLine()
{
	//頂点バッファの解放
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePolyLine(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPolyLine(void)
{
	//カリングなし
	SetCullingMode(CULL_MODE_NONE);

	//加算合成
	SetBlendState(BLEND_MODE_ADD);

	// Z比較なし
	SetDepthEnable(false);

	//ライティング無し
	SetLightEnable(false);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_TexNo));

	for (int i = 0; i < MAX_POLYLINE; i++)
	{
		// 使用中のものだけ実行する
		if (g_Polyline[i].use != true)
			continue;

		// 頂点バッファ更新
		CalcVertexBuffer(i);

		// マテリアル設定
		SetMaterial(g_Polyline[i].material);

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_Polyline[i].mtxWorld);

		// ワールドマトリックスの設定
		SetWorldMatrix(&g_Polyline[i].mtxWorld);

		// ポリゴンの描画
		GetDeviceContext()->Draw(6, 0);
	}

	//各設定を元に戻す
	SetCullingMode(CULL_MODE_BACK);
	SetDepthEnable(true);
	SetLightEnable(true);
	SetBlendState(BLEND_MODE_ALPHABLEND);
}

//=============================================================================
// ポリラインの作成
//=============================================================================
int SetPolyLine(D3DXVECTOR3 begin, D3DXVECTOR3 end, float width)
{
	int index = -1;

	for (int i = 0; i < MAX_POLYLINE; i++)
	{
		// useフラグが未使用（false）だったらそれを使う
		if (g_Polyline[i].use == false)
		{
			g_Polyline[i].begin = begin;
			g_Polyline[i].end = end;
			g_Polyline[i].width = width;

			g_Polyline[i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			g_Polyline[i].use = true;

			index = i;	// 今回使用されたIndex番号
			break;
		}
	}

	return index;	// Index番号を返す（-1なら登録失敗となる）
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorPolyLine(int index, D3DXCOLOR col)
{
	if (index >= 0 && index < MAX_POLYLINE)
	{
		g_Polyline[index].material.Diffuse = col;
	}
}

//=============================================================================
// 座標の再設定
//=============================================================================
void SetPositionPolyLine(int index, D3DXVECTOR3 begin, D3DXVECTOR3 end)
{
	if (index >= 0 && index < MAX_POLYLINE)
	{
		g_Polyline[index].begin = begin;
		g_Polyline[index].end = end;
	}
}

//=============================================================================
// ポリライン破棄
//=============================================================================
void ReleasePolyLine(int index)
{
	if (index >= 0 && index < MAX_POLYLINE)
	{
		g_Polyline[index].use = false;
	}
}

//=============================================================================
// 頂点バッファ更新
//=============================================================================
void CalcVertexBuffer(int idx)
{
	VERTEX_3D vertex[6];

	//始点の頂点データを作成する
	{
		//始点からカメラへのベクトルを求める
		D3DXVECTOR3 eye = GetCamera()->pos - g_Polyline[idx].begin;

		//始点から終点へのベクトルを求める
		D3DXVECTOR3 line = g_Polyline[idx].end - g_Polyline[idx].begin;

		//上記2つのベクトルから始点となる頂点データを作成する
		//　  ・    ・
		//〇   | ＼  | ＼・
		//  ＼・    ・
		D3DXVECTOR3 cross;
		D3DXVec3Cross(&cross, &line, &eye);
		D3DXVec3Cross(&cross, &eye, &cross);
		D3DXVec3Normalize(&cross, &cross);

		vertex[0].Position.x = g_Polyline[idx].begin.x - cross.x * g_Polyline[idx].width;
		vertex[0].Position.y = g_Polyline[idx].begin.y - cross.y * g_Polyline[idx].width;
		vertex[0].Position.z = g_Polyline[idx].begin.z - cross.z * g_Polyline[idx].width;
		vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord.x = 0.0f;
		vertex[0].TexCoord.y = 0.0f;
		vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// 始点側上下と終点側上下の頂点データを作成する
	{
		//始点からカメラへのベクトルを求める
		D3DXVECTOR3 eye = GetCamera()->pos - g_Polyline[idx].begin;

		//始点から終点へのベクトルを求める
		D3DXVECTOR3 line = g_Polyline[idx].end - g_Polyline[idx].begin;

		// 視点ベクトルから直交した単位ベクトルを作成する
		D3DXVECTOR3 cross;
		D3DXVec3Cross(&cross, &line, &eye);
		D3DXVec3Normalize(&cross, &cross);

		// 始点側上下の頂点データ作成
		//　  〇    ・
		//・   | ＼  | ＼・
		//  ＼〇    ・
		vertex[1].Position.x = g_Polyline[idx].begin.x + cross.x * g_Polyline[idx].width;
		vertex[1].Position.y = g_Polyline[idx].begin.y + cross.y * g_Polyline[idx].width;
		vertex[1].Position.z = g_Polyline[idx].begin.z + cross.z * g_Polyline[idx].width;
		vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord.x = 0.5f;
		vertex[1].TexCoord.y = 0.0f;
		vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		vertex[2].Position.x = g_Polyline[idx].begin.x - cross.x * g_Polyline[idx].width;
		vertex[2].Position.y = g_Polyline[idx].begin.y - cross.y * g_Polyline[idx].width;
		vertex[2].Position.z = g_Polyline[idx].begin.z - cross.z * g_Polyline[idx].width;
		vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord.x = 0.0f;
		vertex[2].TexCoord.y = 0.5f;
		vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 終点側上下の頂点データ作成
		//　  ・    〇
		//・   | ＼  | ＼・
		//  ＼・    〇
		vertex[3].Position.x = g_Polyline[idx].end.x + cross.x * g_Polyline[idx].width;
		vertex[3].Position.y = g_Polyline[idx].end.y + cross.y * g_Polyline[idx].width;
		vertex[3].Position.z = g_Polyline[idx].end.z + cross.z * g_Polyline[idx].width;
		vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord.x = 1.0f;
		vertex[3].TexCoord.y = 0.5f;
		vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		vertex[4].Position.x = g_Polyline[idx].end.x - cross.x * g_Polyline[idx].width;
		vertex[4].Position.y = g_Polyline[idx].end.y - cross.y * g_Polyline[idx].width;
		vertex[4].Position.z = g_Polyline[idx].end.z - cross.z * g_Polyline[idx].width;
		vertex[4].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[4].TexCoord.x = 0.5f;
		vertex[4].TexCoord.y = 1.0f;
		vertex[4].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	//終点の頂点データを作成する
	{
		//終点からカメラへのベクトルを求める
		D3DXVECTOR3 eye = GetCamera()->pos - g_Polyline[idx].end;

		//始点から終点へのベクトルを求める
		D3DXVECTOR3 line = g_Polyline[idx].end - g_Polyline[idx].begin;

		//上記2つのベクトルから終点となる頂点データを作成する
		//　  ・    ・
		//・   | ＼  | ＼〇
		//  ＼・    ・
		D3DXVECTOR3 cross;
		D3DXVec3Cross(&cross, &line, &eye);
		D3DXVec3Cross(&cross, &eye, &cross);
		D3DXVec3Normalize(&cross, &cross);

		vertex[5].Position.x = g_Polyline[idx].end.x + cross.x * g_Polyline[idx].width;
		vertex[5].Position.y = g_Polyline[idx].end.y + cross.y * g_Polyline[idx].width;
		vertex[5].Position.z = g_Polyline[idx].end.z + cross.z * g_Polyline[idx].width;
		vertex[5].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[5].TexCoord.x = 1.0f;
		vertex[5].TexCoord.y = 1.0f;
		vertex[5].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	D3D11_MAPPED_SUBRESOURCE msr;
	HRESULT hr = GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	if (SUCCEEDED(hr)) {
		// 頂点データを上書き
		rsize_t len = sizeof(VERTEX_3D) * 6;
		memcpy_s(msr.pData, len, vertex, len);
		// 頂点データをアンロックする
		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}
}

