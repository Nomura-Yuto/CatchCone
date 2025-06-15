//=============================================================================
//
// メッシュ地面の処理 [meshfield.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitMeshField(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
	int numBlockX, int numBlockZ, float sizeX, float sizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

