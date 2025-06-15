//=============================================================================
//
// �����蔻�菈�� [collision.cpp]
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
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool CollisionBB(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR3 size1, D3DXVECTOR3 size2);
bool CollisionBS(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int			g_AxisLine[4]; //�|�����C���p���ʎq

int g_LineFrame = 0;
bool g_LineFlag = true;

//=============================================================================
// �����蔻�菈��
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
				// �X�R�A�̉��Z
				AddScore(GetCornScore());

				// �����X�R�A�̃N���A
				ClearCornScore();

				// �����R�[���̃N���A
				ClearCorn();
			}
		}
	}

	// �΂ƃv���C���[�̓����蔻��
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

			// �̗͂̌���
			DamageHealth();

			// �����X�R�A�̃N���A
			ClearCornScore();

			// �����R�[���̃N���A
			ClearCorn();
		}		
	}

	// �R�[���ƃv���C���[�̓����蔻��
	for (int i = 0; i < CORN_NUM; i++) 
	{
		if (!corn[i].use) continue;

		if (CollisionBB(corn[i].pos, player->pos, corn[i].size, player->size)) 
		{
			corn[i].use = false;

			ReleaseShadow(corn[i].shadow);

			// �����X�R�A�̉��Z
			AddCornScore(Random(30, 40));

			// �R�[���̏������̉��Z
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
// ���E��(BB)�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���true
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
// ���E��(BS)�ɂ�铖���蔻�菈��
// �T�C�Y�͔��a
// �߂�l�F�������Ă���true
//=============================================================================
bool CollisionBS(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2)
{
	//pos1��pos2���Ȃ��x�N�g�������
	D3DXVECTOR3 dist = pos2 - pos1;

	//������x�N�g���̑傫�����v�Z����
//	float length = D3DXVec3Length(&dist);

	//Sq�����Ă���֐��͕������̌v�Z�����Ȃ��o�[�W����
	float length = D3DXVec3LengthSq(&dist);

	//1��2�̔��a�𑫂�
//	float size = size1 + size2;

	//�������̌v�Z�����Ȃ��̂Ŕ��肷�鑊��̕����悷��
	float size = (size1 + size2) * (size1 + size2);

	//���a�̕����傫���Ƃ��A��̉~�͏d�Ȃ��Ă���
	if (length < size)
		return true;

	return false;
}
