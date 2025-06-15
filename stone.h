//=============================================================================
//
// 石処理 [stone.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define STONE_NUM			(30)		// 石のMax数
#define STONE_SPEED			(-4.0f)		// 石の落下スピード
#define STONE_SIZE			(80.0f)		// 石の当たり判定サイズ
#define STONE_SPAWN_HEIGHT	(500.0f)	// 石の生成高さ


// 石構造体
struct STONE
{
	D3DXMATRIX				mtxWorld;
	bool					use;	// true:使っている  false:未使用
	D3DXVECTOR3				pos;	// 石の座標
	D3DXVECTOR3				rot;	// 石の向き(回転)
	D3DXVECTOR3				scl;	// 石の大きさ(スケール)
	float					speed;	// 石の落下スピード
	float					shadowSize;
	int						shadow;	// 影の識別番号

	D3DXVECTOR3     size;		// 当たり判定用サイズ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitStone(void);
void UninitStone(void);
void UpdateStone(void);
void DrawStone(void);

void SetStone(D3DXVECTOR3 pos);
STONE *GetStone(void);

