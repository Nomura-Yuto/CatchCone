//=============================================================================
//
// スコア処理 [score.cpp]
// Author : 
//
//=============================================================================
#include "score.h"
#include "texture.h"
#include "sprite.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(64)	// キャラサイズ
#define TEXTURE_HEIGHT				(128)	// 


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static bool						g_Use;						// true:使っている  false:未使用
static float					g_w, g_h;					// 幅と高さ
static D3DXVECTOR3				g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static int g_SENo = 0;

static int						g_Score;					// スコア

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
{
	g_SENo = LoadSound((char*)"data/SE/laser000.wav");

	//テクスチャ生成
	g_TexNo = LoadTexture((char*)"data/TEXTURE/number000.png");

	//初期化
	g_Use   = true;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = D3DXVECTOR3(1800.0f, 60.0f, 0.0f);

	g_Score = 0;	// スコアの初期化

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateScore(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
	//有効化フラグがONなら描画する
	if (g_Use)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_TexNo));

		// 桁数分処理する
		int number = g_Score;
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);

			// スコアの位置やテクスチャー座標を反映
			float px = g_Pos.x - g_w * i;	// 表示位置X
			float py = g_Pos.y;				// 表示位置Y
			float pw = g_w;					// 表示幅
			float ph = g_h;					// 表示高さ

			float tw = 1.0f / 10;		// テクスチャの幅
			float th = 1.0f / 1;		// テクスチャの高さ
			float tx = x * tw;			// テクスチャの左上X座標
			float ty = 0.0f;			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			DrawSprite(g_TexNo, px, py, pw, ph, tx, ty, tw, th);

			// 次の桁へ
			number /= 10;
		}
	}
}

int GetScore(void)
{
	return g_Score;
}

//=============================================================================
// スコアを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddScore(int add)
{
	if (add != 0) 
	{
		SetVolume(g_SENo, 0.5f);
		PlaySound(g_SENo, 0);
	}

	g_Score += add;

	if (g_Score > SCORE_MAX)
	{
		g_Score = SCORE_MAX;
	}
}

void DispScore(int score, D3DXVECTOR2 pos, D3DXVECTOR2 size)
{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_TexNo));

	// 桁数分処理する
	int number = score;
	for (int i = 0; i < SCORE_DIGIT; i++) {
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = pos.x - size.x * i;	// 表示位置X
		float py = pos.y;				// 表示位置Y
		float pw = size.x;				// 表示幅
		float ph = size.y;				// 表示高さ

		float tw = 1.0f / 10;		// テクスチャの幅
		float th = 1.0f / 1;		// テクスチャの高さ
		float tx = x * tw;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		DrawSprite(g_TexNo, px, py, pw, ph, tx, ty, tw, th);

		// 次の桁へ
		number /= 10;
	}
}

