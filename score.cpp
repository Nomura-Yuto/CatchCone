//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : 
//
//=============================================================================
#include "score.h"
#include "texture.h"
#include "sprite.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(64)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(128)	// 


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static bool						g_Use;						// true:�g���Ă���  false:���g�p
static float					g_w, g_h;					// ���ƍ���
static D3DXVECTOR3				g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static int g_SENo = 0;

static int						g_Score;					// �X�R�A

//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
{
	g_SENo = LoadSound((char*)"data/SE/laser000.wav");

	//�e�N�X�`������
	g_TexNo = LoadTexture((char*)"data/TEXTURE/number000.png");

	//������
	g_Use   = true;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = D3DXVECTOR3(1800.0f, 60.0f, 0.0f);

	g_Score = 0;	// �X�R�A�̏�����

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
{

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateScore(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScore(void)
{
	//�L�����t���O��ON�Ȃ�`�悷��
	if (g_Use)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_TexNo));

		// ��������������
		int number = g_Score;
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Pos.x - g_w * i;	// �\���ʒuX
			float py = g_Pos.y;				// �\���ʒuY
			float pw = g_w;					// �\����
			float ph = g_h;					// �\������

			float tw = 1.0f / 10;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			DrawSprite(g_TexNo, px, py, pw, ph, tx, ty, tw, th);

			// ���̌���
			number /= 10;
		}
	}
}

int GetScore(void)
{
	return g_Score;
}

//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
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
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_TexNo));

	// ��������������
	int number = score;
	for (int i = 0; i < SCORE_DIGIT; i++) {
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = pos.x - size.x * i;	// �\���ʒuX
		float py = pos.y;				// �\���ʒuY
		float pw = size.x;				// �\����
		float ph = size.y;				// �\������

		float tw = 1.0f / 10;		// �e�N�X�`���̕�
		float th = 1.0f / 1;		// �e�N�X�`���̍���
		float tx = x * tw;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		DrawSprite(g_TexNo, px, py, pw, ph, tx, ty, tw, th);

		// ���̌���
		number /= 10;
	}
}

