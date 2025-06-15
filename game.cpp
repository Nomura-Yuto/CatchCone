/*==============================================================================

   ゲーム処理 [game.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/

#include "game.h"
#include "main.h"
#include "input.h"
#include "fade.h"
#include "player.h"
#include "meshfield.h"
#include "meshsky.h"
#include "meshmountain.h"
#include "collision.h"
#include "corn.h"
#include "goal.h"
#include "stone.h"
#include "sound.h"
#include "score.h"
#include "health.h"
#include "camera.h"
#include "shadow.h"
#include "polyline.h"


/*------------------------------------------------------------------------------
   定数定義
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   構造体宣言
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   プロトタイプ宣言
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
   グローバル変数の定義
------------------------------------------------------------------------------*/
static int g_BGMNo = 0;
static LIGHT		g_DirectionalLight;	// ディレクショナルライト
static LIGHT		g_PointLight;		// ポイントライト


/*------------------------------------------------------------------------------
   初期化関数
------------------------------------------------------------------------------*/
void InitGame(void)
{
	// カメラの初期化
	InitCamera();

	// 影の初期化
	InitShadow();

	// ポリラインの初期化
	InitPolyLine();

	// 地面の初期化
	InitMeshField(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 100, 100, 100.0f, 100.0f);

	// 空の初期化
	InitMeshSky(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 8000.0f, 16, 8);

	// 山の初期化
	InitMeshMountain(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 16, 1, 5000.0f, 300.0f);

	// ゴールの初期化
	InitGoal();

	// プレイヤーの初期化
	InitPlayer();

	// 石の初期化
	InitStone();

	// コーンの初期化
	InitCorn();

	// スコアの初期化
	InitScore();

	// 体力の初期化
	InitHealth();

	SetLightEnable(true);

	SetAlphaTestEnable(true);

	g_BGMNo = LoadSound((char*)"data/BGM/yasumi_jikan.wav");

	SetVolume(g_BGMNo, 0.8f);
	PlaySound(g_BGMNo, -1);

	// 並行光源の設定（世界を照らす光）
	g_DirectionalLight.Type = LIGHT_TYPE_DIRECTIONAL;				// 並行光源
	g_DirectionalLight.Enable = true;								// このライトをON
	g_DirectionalLight.Direction = D3DXVECTOR4(0.0f, -1.0f, 0.0f, 0.0f);// 光の向き
	g_DirectionalLight.Diffuse = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);	// 光の色
	SetLight(0, &g_DirectionalLight);								// これで設定している

	// 点光源の設定
	g_PointLight.Type = LIGHT_TYPE_POINT;						// 並行光源
	g_PointLight.Enable = true;									// このライトをON
	g_PointLight.Position = D3DXVECTOR4(-200.0f, 200.0f, -200.0f, 0.0f);// 光の座標
	g_PointLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 光の色
	g_PointLight.Attenuation = 500.0f;							// 減衰距離
	SetLight(1, &g_PointLight);									// これで設定している
}

/*------------------------------------------------------------------------------
   終了処理をする関数
------------------------------------------------------------------------------*/
void UninitGame()
{
	//初期化とは逆順に終了処理を行う
	UninitHealth();
	UninitScore();
	UninitPolyLine();
	UninitShadow();
	UninitCorn();
	UninitStone();
	UninitPlayer();
	UninitGoal();
	UninitMeshMountain();
	UninitMeshSky();
	UninitMeshField();
}

/*------------------------------------------------------------------------------
   更新処理をする関数
------------------------------------------------------------------------------*/
void UpdateGame(void)
{
	// カメラ更新
	UpdateCamera();

	// 地面の更新処理
	UpdateMeshField();
	UpdateMeshSky();
	UpdateMeshMountain();

	// ゴールの更新処理
	UpdateGoal();

	// プレイヤーの更新処理
	UpdatePlayer();

	// 石の更新処理
	UpdateStone();

	// コーンの更新処理
	UpdateCorn();

	// 影の更新処理
	UpdateShadow();

	// ポリラインの更新処理
	UpdatePolyLine();

	// スコアの更新処理
	UpdateScore();

	// 体力の更新処理
	UpdateHealth();

	// 当たり判定の更新処理
	UpdateCollision();

	// 体力が0以下になったとき
	if (isAlive() && GetFadeState() == FADE_NONE) 
	{
		SetVolume(g_BGMNo, 0.1f);

		//RESULTへ移行する
		SceneTransition(SCENE_RESULT);
	}

	//スペースキーが押されていて、フェード処理中ではないとき
	if (GetKeyboardTrigger(DIK_RETURN) && GetFadeState() == FADE_NONE) 
	{
		SetVolume(g_BGMNo, 0.1f);

		//RESULTへ移行する
		SceneTransition(SCENE_RESULT);
	}
}

/*------------------------------------------------------------------------------
   描画処理をする関数
------------------------------------------------------------------------------*/
void DrawGame(void)
{
	// プレイヤー視点
	D3DXVECTOR3 pos = GetPlayer()->pos;
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	// 深度テスト有効
	SetDepthEnable(true);

	// ライティング有効
	SetLightEnable(true);

	// 地面の描画処理
	DrawMeshSky();
	DrawMeshField();
	DrawMeshMountain();

	// 影の描画処理
	DrawShadow();

	// ゴールの描画処理
	DrawGoal();

	// プレイヤーの描画処理
	DrawPlayer();

	// 石の描画処理
	DrawStone();

	// コーンの描画処理
	DrawCorn();

	// ポリラインの描画処理
	DrawPolyLine();

	// マトリクス設定
	SetWorldViewProjection2D();//座標の2D変換

	// 2D描画なので深度無効
	SetDepthEnable(false);

	// スコアの描画処理
	DrawScore();

	// 体力の描画処理
	DrawHealth();
}

