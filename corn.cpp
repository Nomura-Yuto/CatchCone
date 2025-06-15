//=============================================================================
//
// �R�[������ [corn.cpp]
// Author : 
//
//=============================================================================
#include "corn.h"
#include "camera.h"
#include "texture.h"
#include "shadow.h"
#include "score.h"
#include "sprite.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(128)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(128)	// 

#define SPAWN_INTERVAL	(0.3f)	// �R�[���̐����Ԋu(�b)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer		*g_VertexBuffer = NULL;	// ���_�o�b�t�@

static CORN		g_Corn[CORN_NUM];	// �R�[���f�[�^
static MATERIAL		g_Material;
static int			g_texNo1 = 0;
static int			g_texNo2 = 0;
static int			g_texNo3 = 0;

int g_cornFrame = 0;
int g_haveCorn = 0;		// �R�[���̏�����
int g_cornScore = 0;	// �����X�R�A

static float					g_w, g_h;					// ���ƍ���
static D3DXVECTOR3				g_Pos;						// �|���S���̍��W
//=============================================================================
// ����������
//=============================================================================
void InitCorn(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		float width = CORN_SIZE;
		float height = CORN_SIZE;

		// ���_���W�̐ݒ�
		vertex[0].Position = D3DXVECTOR3(-width / 2.0f, 0.0f, 0.0f);
		vertex[1].Position = D3DXVECTOR3(-width / 2.0f, height, 0.0f);
		vertex[2].Position = D3DXVECTOR3(width / 2.0f, 0.0f, 0.0f);
		vertex[3].Position = D3DXVECTOR3(width / 2.0f, height, 0.0f);

		// ���_�J���[�̐ݒ�
		vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
		vertex[1].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
		vertex[2].TexCoord = D3DXVECTOR2(1.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	// �e�N�X�`������
	g_texNo1 = LoadTexture((char*)"data/TEXTURE/popcorn_01.png");
	g_texNo2 = LoadTexture((char*)"data/TEXTURE/popcorn_02.png");
	g_texNo3 = LoadTexture((char*)"data/TEXTURE/popcorn_03.png");

	// �}�e���A��������
	ZeroMemory(&g_Material, sizeof(g_Material));
	g_Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ������
	for(int i = 0; i < CORN_NUM; i++)
	{
		g_Corn[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Corn[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Corn[i].size = D3DXVECTOR3(CORN_SIZE, CORN_SIZE, CORN_SIZE);
		g_Corn[i].use = false;
		g_Corn[i].num = Random(1, 3);
	}

	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = D3DXVECTOR3(SCREEN_WIDTH / 2 - TEXTURE_WIDTH * 3, SCREEN_HEIGHT - TEXTURE_HEIGHT, 0.0f);
}

//=============================================================================
// �I������
//=============================================================================
void UninitCorn(void)
{
	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateCorn(void)
{
	for(int i = 0; i < CORN_NUM; i++)
	{
		if(g_Corn[i].use)
		{
			//���ŏ���
			if (g_Corn[i].pos.y - g_Corn[i].size.y / 2 <= -10.0f)
			{
				g_Corn[i].use = false;

				//�e������
				ReleaseShadow(g_Corn[i].shadow);
			}

			//���W�̍X�V
			g_Corn[i].pos.y += g_Corn[i].speed;

			//�e�̍��W���X�V����
			SetPositionShadow(g_Corn[i].shadow, D3DXVECTOR3(g_Corn[i].pos.x, 0.0f, g_Corn[i].pos.z));

			SetSizeShadow(g_Corn[i].shadow, 
				g_Corn[i].shadowSize * ((CORN_SPAWN_HEIGHT - g_Corn[i].pos.y) / 400.0f));
		}
	}

	// �R�[���̐���
	if (g_cornFrame >= 60 * SPAWN_INTERVAL) 
	{
		SetCorn(D3DXVECTOR3(Random(-SPAWN_SIZE_HALF,SPAWN_SIZE_HALF), 
			CORN_SPAWN_HEIGHT, Random(-SPAWN_SIZE_HALF,SPAWN_SIZE_HALF)));
		g_cornFrame = 0;
	}

	g_cornFrame++;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCorn(void)
{
	// ���e�X�g��L����
	SetAlphaTestEnable(true);

	// ���C�e�B���O�𖳌�
	SetLightEnable(false);

	D3DXMATRIX mtxView, mtxScale, mtxTranslate;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	CAMERA *cam = GetCamera();

	for(int i = 0; i < CORN_NUM; i++)
	{
		if(g_Corn[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_Corn[i].mtxWorld);


			// �|���S���𐳖ʂɌ�����
#if 0
			// �r���[�}�g���b�N�X�̋t�s����擾
			g_Corn[i].mtxWorld = cam->mtxInvView;
			g_Corn[i].mtxWorld._41 = 0.0f;
			g_Corn[i].mtxWorld._42 = 0.0f;
			g_Corn[i].mtxWorld._43 = 0.0f;
#else
			// �r���[�}�g���b�N�X���擾
			D3DXMATRIX mtxView = cam->mtxView;

			g_Corn[i].mtxWorld._11 = mtxView._11;
			g_Corn[i].mtxWorld._12 = mtxView._21;
			g_Corn[i].mtxWorld._13 = mtxView._31;
			g_Corn[i].mtxWorld._21 = mtxView._12;
			g_Corn[i].mtxWorld._22 = mtxView._22;
			g_Corn[i].mtxWorld._23 = mtxView._32;
			g_Corn[i].mtxWorld._31 = mtxView._13;
			g_Corn[i].mtxWorld._32 = mtxView._23;
			g_Corn[i].mtxWorld._33 = mtxView._33;
#endif


			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, g_Corn[i].pos.x,
												 g_Corn[i].pos.y,
												 g_Corn[i].pos.z);
			D3DXMatrixMultiply(&g_Corn[i].mtxWorld, &g_Corn[i].mtxWorld, &mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&g_Corn[i].mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_Material);

			// �e�N�X�`���ݒ�
			switch (g_Corn[i].num) {
			case 1:
				GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo1));
				break;
			case 2:
				GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo2));
				break;
			case 3:
				GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo3));
				break;
			default:
				break;
			}

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(false);


	// ��������������
	for (int i = 0; i < g_haveCorn; i++)
	{
		// �̗͂̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x + g_w * i;	// �\���ʒuX
		float py = g_Pos.y;				// �\���ʒuY
		float pw = g_w;					// �\����
		float ph = g_h;					// �\������

		float tw = 1.0f / 1;		// �e�N�X�`���̕�
		float th = 1.0f / 1;		// �e�N�X�`���̍���
		float tx = 0.0f;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo1));

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		DrawSprite(g_texNo1, px, py, pw, ph, tx, ty, tw, th);
	}
}

//=============================================================================
// �R�[���̃p�����[�^���Z�b�g
//=============================================================================
void SetCorn(D3DXVECTOR3 pos)
{
	for(int i = 0; i < CORN_NUM; i++)
	{
		//�܂��\������Ă��Ȃ��ꏊ��T��
		if(g_Corn[i].use == false)
		{
			g_Corn[i].pos = pos;

			g_Corn[i].speed = -Random(2,7);

			// �����蔻��p�T�C�Y��ݒ�
			g_Corn[i].size = D3DXVECTOR3(CORN_SIZE, CORN_SIZE, CORN_SIZE);

			// �e�̐ݒ�
			g_Corn[i].shadowSize = 80.0f;

			g_Corn[i].shadow = SetShadow(D3DXVECTOR3(g_Corn[i].pos.x, 0.0f, g_Corn[i].pos.z), g_Corn[i].shadowSize);

			g_Corn[i].use = true;

			break;
		}
	}
}


CORN* GetCorn(void)
{
	return &g_Corn[0];
}

//=============================================================================
// �R�[���̏������̉��Z�E���Z
//=============================================================================
void AddCorn(int add)
{
	g_haveCorn += add;

	if (g_haveCorn >= CORN_HAVE_MAX) g_haveCorn = CORN_HAVE_MAX;
}

//=============================================================================
// �R�[���̏������̃N���A
//=============================================================================
void ClearCorn(void)
{
	g_haveCorn = 0;
}

//=============================================================================
// �����X�R�A�̎擾
//=============================================================================
int GetCornScore(void) 
{
	return g_cornScore;
}

//=============================================================================
// �����X�R�A�̉��Z�E���Z
//=============================================================================
void AddCornScore(int add)
{
	if (g_haveCorn >= CORN_HAVE_MAX) return;
	g_cornScore += add;
}

//=============================================================================
// �����X�R�A�̃N���A
//=============================================================================
void ClearCornScore(void)
{
	g_cornScore = 0;
}

