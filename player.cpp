//=============================================================================
//
// ポリゴン表示処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "player.h"
#include "texture.h"
#include "model.h"
#include "shadow.h"
#include "polyline.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE		(5.0f)						// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)			// 回転量
#define SHOT_INTERVAL	(0.25f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_MODEL	g_Model;	// モデル読み込み
static PLAYER		g_Player;	// プレイヤー情報		
static float        g_Rot;		// キャラクターの向き


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	// 初期設定
	g_Player.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.scl = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
	g_Player.spd = 0.0f;
	g_Player.use = true;

	//当たり判定用サイズの初期設定
	g_Player.size = D3DXVECTOR3(50.0f, 50.0f, 50.0f);

	//objモデルの読み込み
	LoadModel((char*)"data/MODEL/popcornBox_op_01.obj", &g_Model);

	// 影のセット（影は高さを0にして表示させる）
	g_Player.shadow = SetShadow(D3DXVECTOR3(g_Player.pos.x, 0.0f, g_Player.pos.z), 80.0f);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	//objモデルの解放
	UnloadModel(&g_Model);
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	//移動速度の減衰処理
	g_Player.spd *= 0.9f;

	if (GetKeyboardPress(DIK_A))
	{
		g_Player.spd = VALUE_MOVE;
		g_Rot = -D3DX_PI * 0.5f;
	}
	if (GetKeyboardPress(DIK_D))
	{
		g_Player.spd = VALUE_MOVE;
		g_Rot = D3DX_PI * 0.5f;
	}

	if (GetKeyboardPress(DIK_W))
	{
		g_Player.spd = VALUE_MOVE;
		g_Rot = 0.0f;

		if (GetKeyboardPress(DIK_A))
		{
			g_Rot = -D3DX_PI * 0.25f;
		}
		if (GetKeyboardPress(DIK_D))
		{
			g_Rot = D3DX_PI * 0.25f;
		}
	}
	if (GetKeyboardPress(DIK_S))
	{
		g_Player.spd = VALUE_MOVE;
		g_Rot = D3DX_PI;

		if (GetKeyboardPress(DIK_A))
		{
			g_Rot = -D3DX_PI * 0.75f;
		}
		if (GetKeyboardPress(DIK_D))
		{
			g_Rot = D3DX_PI * 0.75f;
		}
	}

	{	// 押した方向にプレイヤーを移動させる
		// 押した方向にプレイヤーを向かせている所
		g_Player.rot.y = g_Rot + cam->rot.y;

		g_Player.pos.x += sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z += cosf(g_Player.rot.y) * g_Player.spd;
	}

	if (g_Player.pos.x > SPAWN_SIZE_HALF + 100.0f)
		g_Player.pos.x = SPAWN_SIZE_HALF + 100.0f;

	if (g_Player.pos.x < -SPAWN_SIZE_HALF - 100.0f)
		g_Player.pos.x = -SPAWN_SIZE_HALF - 100.0f;

	if (g_Player.pos.z > SPAWN_SIZE_HALF + 100.0f)
		g_Player.pos.z = SPAWN_SIZE_HALF + 100.0f;

	if (g_Player.pos.z < -SPAWN_SIZE_HALF - 100.0f)
		g_Player.pos.z = -SPAWN_SIZE_HALF - 100.0f;

	// 影の位置を更新
	SetPositionShadow(g_Player.shadow, D3DXVECTOR3(g_Player.pos.x, 0.0f, g_Player.pos.z));

#ifdef _DEBUG	// デバッグ情報を表示する
	char* str = GetDebugStr();
	sprintf(&str[strlen(str)], " rotX:%.2f rotY:%.2f", g_Player.rot.x, g_Player.rot.y);

#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化（単位行列を作る関数）
	D3DXMatrixIdentity(&g_Player.mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Player.rot.y, g_Player.rot.x, g_Player.rot.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&g_Player.mtxWorld);

	//カリングなし(ミクさんモデルの髪の対応)
	SetCullingMode(CULL_MODE_NONE);

	//objモデルの描画
	DrawModel(&g_Model);

	//カリングを元に戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}
