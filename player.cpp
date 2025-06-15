//=============================================================================
//
// �|���S���\������ [player.cpp]
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
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE		(5.0f)						// �ړ���
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)			// ��]��
#define SHOT_INTERVAL	(0.25f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static DX11_MODEL	g_Model;	// ���f���ǂݍ���
static PLAYER		g_Player;	// �v���C���[���		
static float        g_Rot;		// �L�����N�^�[�̌���


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	// �����ݒ�
	g_Player.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.scl = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
	g_Player.spd = 0.0f;
	g_Player.use = true;

	//�����蔻��p�T�C�Y�̏����ݒ�
	g_Player.size = D3DXVECTOR3(50.0f, 50.0f, 50.0f);

	//obj���f���̓ǂݍ���
	LoadModel((char*)"data/MODEL/popcornBox_op_01.obj", &g_Model);

	// �e�̃Z�b�g�i�e�͍�����0�ɂ��ĕ\��������j
	g_Player.shadow = SetShadow(D3DXVECTOR3(g_Player.pos.x, 0.0f, g_Player.pos.z), 80.0f);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	//obj���f���̉��
	UnloadModel(&g_Model);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	//�ړ����x�̌�������
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

	{	// �����������Ƀv���C���[���ړ�������
		// �����������Ƀv���C���[���������Ă��鏊
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

	// �e�̈ʒu���X�V
	SetPositionShadow(g_Player.shadow, D3DXVECTOR3(g_Player.pos.x, 0.0f, g_Player.pos.z));

#ifdef _DEBUG	// �f�o�b�O����\������
	char* str = GetDebugStr();
	sprintf(&str[strlen(str)], " rotX:%.2f rotY:%.2f", g_Player.rot.x, g_Player.rot.y);

#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ���[���h�}�g���b�N�X�̏������i�P�ʍs������֐��j
	D3DXMatrixIdentity(&g_Player.mtxWorld);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxScl);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Player.rot.y, g_Player.rot.x, g_Player.rot.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&g_Player.mtxWorld);

	//�J�����O�Ȃ�(�~�N���񃂃f���̔��̑Ή�)
	SetCullingMode(CULL_MODE_NONE);

	//obj���f���̕`��
	DrawModel(&g_Model);

	//�J�����O�����ɖ߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}
