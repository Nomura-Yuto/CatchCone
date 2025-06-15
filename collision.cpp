//=============================================================================
//
// 当たり判定処理 [collision.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "sound.h"
#include "input.h"
#include "collision.h"
#include "player.h"
#include "shadow.h"
#include "score.h"
#include "health.h"
#include "stone.h"
#include "corn.h"
#include "goal.h"
#include "polyline.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool CollisionBB(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR3 size1, D3DXVECTOR3 size2);
bool CollisionBS(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int			g_AxisLine[4]; //ポリライン用識別子

int g_LineFrame = 0;
bool g_LineFlag = true;

//=============================================================================
// 当たり判定処理
//=============================================================================
void UpdateCollision(void)
{
	PLAYER *player = GetPlayer();
	STONE *stone = GetStone();
	CORN* corn = GetCorn();
	GOAL* goal = GetGoal();

	if (goal->use) 
	{
		if (CollisionBB(goal->pos, player->pos, goal->size, player->size)) 
		{
			if (GetKeyboardTrigger(DIK_SPACE)) 
			{
				// スコアの加算
				AddScore(GetCornScore());

				// 所持スコアのクリア
				ClearCornScore();

				// 所持コーンのクリア
				ClearCorn();
			}
		}
	}

	// 石とプレイヤーの当たり判定
	for (int i = 0; i < STONE_NUM; i++) 
	{
		if (!stone[i].use) continue;

		if (CollisionBB(stone[i].pos, player->pos, stone[i].size, player->size)) 
		{
			if (!g_LineFlag) 
			{
				g_AxisLine[0] = SetPolyLine(stone[i].pos + D3DXVECTOR3(-25.0f, 0.0f, -25.0f), 
					stone[i].pos + D3DXVECTOR3(25.0f, 0.0f, 25.0f), 15.0f);
				SetColorPolyLine(g_AxisLine[0], D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.8f));

				g_AxisLine[1] = SetPolyLine(stone[i].pos + D3DXVECTOR3(25.0f, 0.0f, -25.0f), 
					stone[i].pos + D3DXVECTOR3(-25.0f, 0.0f, 25.0f), 15.0f);
				SetColorPolyLine(g_AxisLine[1], D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.8f));

				g_AxisLine[2] = SetPolyLine(stone[i].pos + D3DXVECTOR3(-25.0f, 0.0f, 0.0f), 
					stone[i].pos + D3DXVECTOR3(25.0f, 0.0f, 0.0f), 15.0f);
				SetColorPolyLine(g_AxisLine[2], D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.8f));

				g_AxisLine[3] = SetPolyLine(stone[i].pos + D3DXVECTOR3(0.0f, 0.0f, 25.0f), 
					stone[i].pos + D3DXVECTOR3(0.0f, 0.0f, -25.0f), 15.0f);
				SetColorPolyLine(g_AxisLine[3], D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.8f));

				g_LineFlag = true;
			}

			stone[i].use = false;
			ReleaseShadow(stone[i].shadow);		

			// 体力の減少
			DamageHealth();

			// 所持スコアのクリア
			ClearCornScore();

			// 所持コーンのクリア
			ClearCorn();
		}		
	}

	// コーンとプレイヤーの当たり判定
	for (int i = 0; i < CORN_NUM; i++) 
	{
		if (!corn[i].use) continue;

		if (CollisionBB(corn[i].pos, player->pos, corn[i].size, player->size)) 
		{
			corn[i].use = false;

			ReleaseShadow(corn[i].shadow);

			// 所持スコアの加算
			AddCornScore(Random(30, 40));

			// コーンの所持数の加算
			AddCorn(1);
		}		
	}

	if (g_LineFrame == 30) 
	{
		ReleasePolyLine(g_AxisLine[0]);
		ReleasePolyLine(g_AxisLine[1]);
		ReleasePolyLine(g_AxisLine[2]);
		ReleasePolyLine(g_AxisLine[3]);
		g_LineFrame = 0;
		g_LineFlag = false;
	}

	if (g_LineFlag)g_LineFrame++;
}

//=============================================================================
// 境界箱(BB)による当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
bool CollisionBB(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR3 size1, D3DXVECTOR3 size2)
{
	float Axmax = pos1.x + (size1.x / 2);
	float Axmin = pos1.x - (size1.x / 2);
	float Aymax = pos1.y + (size1.y / 2);
	float Aymin = pos1.y - (size1.y / 2);
	float Azmax = pos1.z + (size1.z / 2);
	float Azmin = pos1.z - (size1.z / 2);

	float Bxmax = pos2.x + (size2.x / 2);
	float Bxmin = pos2.x - (size2.x / 2);
	float Bymax = pos2.y + (size2.y / 2);
	float Bymin = pos2.y - (size2.y / 2);
	float Bzmax = pos2.z + (size2.z / 2);
	float Bzmin = pos2.z - (size2.z / 2);

	if ((Axmax > Bxmin) && (Axmin < Bxmax))
	{
		if ((Aymin < Bymax) && (Aymax > Bymin))
		{
			if ((Azmax > Bzmin) && (Azmin < Bzmax))
			{
				return true;
			}
		}
	}

	return false;
}

//=============================================================================
// 境界球(BS)による当たり判定処理
// サイズは半径
// 戻り値：当たってたらtrue
//=============================================================================
bool CollisionBS(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2)
{
	//pos1とpos2をつなぐベクトルを作る
	D3DXVECTOR3 dist = pos2 - pos1;

	//作ったベクトルの大きさを計算する
//	float length = D3DXVec3Length(&dist);

	//Sqがついている関数は平方根の計算をしないバージョン
	float length = D3DXVec3LengthSq(&dist);

	//1と2の半径を足す
//	float size = size1 + size2;

	//平方根の計算をしないので判定する相手の方を二乗する
	float size = (size1 + size2) * (size1 + size2);

	//半径の方が大きいとき、二つの円は重なっている
	if (length < size)
		return true;

	return false;
}
