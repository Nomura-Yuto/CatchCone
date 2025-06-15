//=============================================================================
//
// �R�[������ [corn.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CORN_HAVE_MAX		(6)
#define CORN_NUM			(30)		// �R�[����Max��
#define CORN_SPEED			(-4.0f)		// �R�[���̗����X�s�[�h
#define CORN_SIZE			(80.0f)		// �R�[���̓����蔻��T�C�Y
#define CORN_SPAWN_HEIGHT	(500.0f)	// �R�[���̐�������


// �R�[���\����
struct CORN
{
	D3DXMATRIX		mtxWorld;
	bool			use;		// true:�g���Ă���  false:���g�p
	D3DXVECTOR3		pos;		// �R�[���̍��W
	D3DXVECTOR3		rot;		// �R�[���̌���(��])
	float			speed;		// �R�[���̗����X�s�[�h
	float			shadowSize;
	int				shadow;		// �e�̎��ʔԍ�
	int				num;		// �R�[���̎��

	D3DXVECTOR3     size;		// �����蔻��p�T�C�Y
};

//*****************************************************************************
// �v���g�^�C�v�錾
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
