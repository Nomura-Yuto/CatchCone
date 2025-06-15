//=============================================================================
//
// ゴール(納品場所)処理 [goal.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "goal.h"
#include "texture.h"
#include "model.h"
#include "shadow.h"
#include "polyline.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_MODEL	g_Model;	// モデル読み込み
static GOAL		g_Goal;			// ゴール情報		


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGoal(void)
{
	// 初期設定
	g_Goal.pos = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	g_Goal.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Goal.scl = D3DXVECTOR3(5.0f, 5.0f, 5.0f);
	g_Goal.use = true;

	//当たり判定用サイズの初期設定
	g_Goal.size = D3DXVECTOR3(50.0f, 50.0f, 50.0f);

	//objモデルの読み込み
	LoadModel((char*)"data/MODEL/torus.obj", &g_Model);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGoal(void)
{
	//objモデルの解放
	UnloadModel(&g_Model);
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGoal(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGoal(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化（単位行列を作る関数）
	D3DXMatrixIdentity(&g_Goal.mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, g_Goal.scl.x, g_Goal.scl.y, g_Goal.scl.z);
	D3DXMatrixMultiply(&g_Goal.mtxWorld, &g_Goal.mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Goal.rot.y, g_Goal.rot.x, g_Goal.rot.z);
	D3DXMatrixMultiply(&g_Goal.mtxWorld, &g_Goal.mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_Goal.pos.x, g_Goal.pos.y, g_Goal.pos.z);
	D3DXMatrixMultiply(&g_Goal.mtxWorld, &g_Goal.mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&g_Goal.mtxWorld);

	//objモデルの描画
	DrawModel(&g_Model);
}


//=============================================================================
// ゴール情報を取得
//=============================================================================
GOAL *GetGoal(void)
{
	return &g_Goal;
}
