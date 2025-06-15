//=============================================================================
//
// �Ώ��� [stone.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define STONE_NUM			(30)		// �΂�Max��
#define STONE_SPEED			(-4.0f)		// �΂̗����X�s�[�h
#define STONE_SIZE			(80.0f)		// �΂̓����蔻��T�C�Y
#define STONE_SPAWN_HEIGHT	(500.0f)	// �΂̐�������


// �΍\����
struct STONE
{
	D3DXMATRIX				mtxWorld;
	bool					use;	// true:�g���Ă���  false:���g�p
	D3DXVECTOR3				pos;	// �΂̍��W
	D3DXVECTOR3				rot;	// �΂̌���(��])
	D3DXVECTOR3				scl;	// �΂̑傫��(�X�P�[��)
	float					speed;	// �΂̗����X�s�[�h
	float					shadowSize;
	int						shadow;	// �e�̎��ʔԍ�

	D3DXVECTOR3     size;		// �����蔻��p�T�C�Y
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitStone(void);
void UninitStone(void);
void UpdateStone(void);
void DrawStone(void);

void SetStone(D3DXVECTOR3 pos);
STONE *GetStone(void);

