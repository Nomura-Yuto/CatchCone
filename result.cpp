//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : 
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"
#include "score.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_TextureNo = 0;	// テクスチャ情報
static int	g_BGMNo = 0;		// BGM識別子
int g_FinishScore = 0;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
{
	g_TextureNo = LoadTexture((char*)"data/TEXTURE/result01.png");

	g_BGMNo = LoadSound((char*)"data/BGM/megasameta.wav");

	SetVolume(g_BGMNo, 0.8f);
	PlaySound(g_BGMNo, -1);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
{
	StopSoundAll();
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateResult(void)
{
	if (GetKeyboardTrigger(DIK_RETURN) && GetFadeState() == FADE_NONE)
	{
		SceneTransition(SCENE_TITLE);
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
{
	// １枚のポリゴンの頂点とテクスチャ座標を設定
	DrawSpriteLeftTop(g_TextureNo, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

	DispScore(GetScore(), D3DXVECTOR2(SCREEN_WIDTH / 5 * 3, SCREEN_HEIGHT / 5 * 4), D3DXVECTOR2(96, 192));
}
