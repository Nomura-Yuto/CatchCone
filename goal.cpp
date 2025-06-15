//=============================================================================
//
// �S�[��(�[�i�ꏊ)���� [goal.cpp]
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
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static DX11_MODEL	g_Model;	// ���f���ǂݍ���
static GOAL		g_Goal;			// �S�[�����		


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGoal(void)
{
	// �����ݒ�
	g_Goal.pos = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	g_Goal.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Goal.scl = D3DXVECTOR3(5.0f, 5.0f, 5.0f);
	g_Goal.use = true;

	//�����蔻��p�T�C�Y�̏����ݒ�
	g_Goal.size = D3DXVECTOR3(50.0f, 50.0f, 50.0f);

	//obj���f���̓ǂݍ���
	LoadModel((char*)"data/MODEL/torus.obj", &g_Model);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGoal(void)
{
	//obj���f���̉��
	UnloadModel(&g_Model);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGoal(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGoal(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ���[���h�}�g���b�N�X�̏������i�P�ʍs������֐��j
	D3DXMatrixIdentity(&g_Goal.mtxWorld);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, g_Goal.scl.x, g_Goal.scl.y, g_Goal.scl.z);
	D3DXMatrixMultiply(&g_Goal.mtxWorld, &g_Goal.mtxWorld, &mtxScl);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Goal.rot.y, g_Goal.rot.x, g_Goal.rot.z);
	D3DXMatrixMultiply(&g_Goal.mtxWorld, &g_Goal.mtxWorld, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_Goal.pos.x, g_Goal.pos.y, g_Goal.pos.z);
	D3DXMatrixMultiply(&g_Goal.mtxWorld, &g_Goal.mtxWorld, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&g_Goal.mtxWorld);

	//obj���f���̕`��
	DrawModel(&g_Model);
}


//=============================================================================
// �S�[�������擾
//=============================================================================
GOAL *GetGoal(void)
{
	return &g_Goal;
}
