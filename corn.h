//=============================================================================
//
// コーン処理 [corn.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CORN_HAVE_MAX		(6)
#define CORN_NUM			(30)		// コーンのMax数
#define CORN_SPEED			(-4.0f)		// コーンの落下スピード
#define CORN_SIZE			(80.0f)		// コーンの当たり判定サイズ
#define CORN_SPAWN_HEIGHT	(500.0f)	// コーンの生成高さ


// コーン構造体
struct CORN
{
	D3DXMATRIX		mtxWorld;
	bool			use;		// true:使っている  false:未使用
	D3DXVECTOR3		pos;		// コーンの座標
	D3DXVECTOR3		rot;		// コーンの向き(回転)
	float			speed;		// コーンの落下スピード
	float			shadowSize;
	int				shadow;		// 影の識別番号
	int				num;		// コーンの種類

	D3DXVECTOR3     size;		// 当たり判定用サイズ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCorn(void);
void UninitCorn(void);
void UpdateCorn(void);
void DrawCorn(void);

void SetCorn(D3DXVECTOR3 pos);
CORN *GetCorn(void);

void AddCorn(int add);
void ClearCorn(void);
int GetCornScore(void);
void AddCornScore(int add);
void ClearCornScore(void);
