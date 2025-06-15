//=============================================================================
//
// �Ώ��� [stone.cpp]
// Author : 
//
//=============================================================================
#include "stone.h"
#include "model.h"
#include "camera.h"
#include "texture.h"
#include "shadow.h"
#include "polyline.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define STONE_SCL		(8.0f)	// �΂̑傫��(�X�P�[��)
#define SPAWN_INTERVAL	(0.5f)	// �΂̐����Ԋu(�b)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static DX11_MODEL	g_Model;	// ���f���ǂݍ���
static ID3D11Buffer		*g_VertexBuffer = NULL;	// ���_�o�b�t�@

static STONE		g_Stone[STONE_NUM];	// �΃f�[�^
static MATERIAL		g_Material;
static int			g_texNo = 0;
int g_stoneFrame = 0;


//=============================================================================
// ����������
//=============================================================================
void InitStone(void)
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

		float width = 50.0f;
		float height = 50.0f;

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
	g_texNo = LoadTexture((char*)"data/TEXTURE/nature_stone_ishi.png");

	// �}�e���A��������
	ZeroMemory(&g_Material, sizeof(g_Material));
	g_Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//obj���f���̓ǂݍ���
	LoadModel((char*)"data/MODEL/stone_op_02.obj", &g_Model);

	// ������
	for(int i = 0; i < STONE_NUM; i++)
	{
		g_Stone[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Stone[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Stone[i].scl = D3DXVECTOR3(STONE_SCL, STONE_SCL, STONE_SCL);
		g_Stone[i].size = D3DXVECTOR3(STONE_SIZE, STONE_SIZE, STONE_SIZE);
		g_Stone[i].use = false;
	}
}

//=============================================================================
// �I������
//=============================================================================
void UninitStone(void)
{
	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	//obj���f���̉��
	UnloadModel(&g_Model);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateStone(void)
{
	for(int i = 0; i < STONE_NUM; i++)
	{
		if(g_Stone[i].use)
		{
			//���ŏ���
			if (g_Stone[i].pos.y - g_Stone[i].size.y / 2 <= -10.0f)
			{
				g_Stone[i].use = false;

				//�e������
				ReleaseShadow(g_Stone[i].shadow);

			}

			//���W�̍X�V
			g_Stone[i].pos.y += g_Stone[i].speed;

			//�e�̍��W���X�V����
			SetPositionShadow(g_Stone[i].shadow, D3DXVECTOR3(g_Stone[i].pos.x, 0.0f, g_Stone[i].pos.z));
			SetSizeShadow(g_Stone[i].shadow, 
				g_Stone[i].shadowSize * ((STONE_SPAWN_HEIGHT - g_Stone[i].pos.y) / 400.0f));
		}
	}

	// �΂̐���
	if (g_stoneFrame >= 60 * SPAWN_INTERVAL) 
	{
		SetStone(D3DXVECTOR3(Random(-SPAWN_SIZE_HALF,SPAWN_SIZE_HALF), 
			STONE_SPAWN_HEIGHT, Random(-SPAWN_SIZE_HALF,SPAWN_SIZE_HALF)));
		g_stoneFrame = 0;
	}

	g_stoneFrame++;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawStone(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	for (int i = 0; i < STONE_NUM; i++) 
	{
		//use�t���O���I�t�̏ꍇ�̓X�L�b�v����
		if (!g_Stone[i].use)
			continue;

		// ���[���h�}�g���b�N�X�̏������i�P�ʍs������֐��j
		D3DXMatrixIdentity(&g_Stone[i].mtxWorld);

		// �X�P�[���𔽉f
		D3DXMatrixScaling(&mtxScl, g_Stone[i].scl.x, g_Stone[i].scl.y, g_Stone[i].scl.z);
		D3DXMatrixMultiply(&g_Stone[i].mtxWorld, &g_Stone[i].mtxWorld, &mtxScl);

		// ��]�𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Stone[i].rot.y, g_Stone[i].rot.x, g_Stone[i].rot.z);
		D3DXMatrixMultiply(&g_Stone[i].mtxWorld, &g_Stone[i].mtxWorld, &mtxRot);

		// �ړ��𔽉f
		D3DXMatrixTranslation(&mtxTranslate, g_Stone[i].pos.x, g_Stone[i].pos.y, g_Stone[i].pos.z);
		D3DXMatrixMultiply(&g_Stone[i].mtxWorld, &g_Stone[i].mtxWorld, &mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&g_Stone[i].mtxWorld);

		//obj���f���̕`��
		DrawModel(&g_Model);
	}

//	// ���e�X�g��L����
//	SetAlphaTestEnable(true);
//
//	// ���C�e�B���O�𖳌�
//	SetLightEnable(false);
//
//	D3DXMATRIX mtxView, mtxScale, mtxTranslate;
//
//	// ���_�o�b�t�@�ݒ�
//	UINT stride = sizeof(VERTEX_3D);
//	UINT offset = 0;
//	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
//
//	// �v���~�e�B�u�g�|���W�ݒ�
//	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//	CAMERA *cam = GetCamera();
//
//	for(int i = 0; i < STONE_NUM; i++)
//	{
//		if(g_Stone[i].use)
//		{
//			// ���[���h�}�g���b�N�X�̏�����
//			D3DXMatrixIdentity(&g_Stone[i].mtxWorld);
//
//
//			// �|���S���𐳖ʂɌ�����
//#if 0
//			// �r���[�}�g���b�N�X�̋t�s����擾
//			g_Stone[i].mtxWorld = cam->mtxInvView;
//			g_Stone[i].mtxWorld._41 = 0.0f;
//			g_Stone[i].mtxWorld._42 = 0.0f;
//			g_Stone[i].mtxWorld._43 = 0.0f;
//#else
//			// �r���[�}�g���b�N�X���擾
//			D3DXMATRIX mtxView = cam->mtxView;
//
//			g_Stone[i].mtxWorld._11 = mtxView._11;
//			g_Stone[i].mtxWorld._12 = mtxView._21;
//			g_Stone[i].mtxWorld._13 = mtxView._31;
//			g_Stone[i].mtxWorld._21 = mtxView._12;
//			g_Stone[i].mtxWorld._22 = mtxView._22;
//			g_Stone[i].mtxWorld._23 = mtxView._32;
//			g_Stone[i].mtxWorld._31 = mtxView._13;
//			g_Stone[i].mtxWorld._32 = mtxView._23;
//			g_Stone[i].mtxWorld._33 = mtxView._33;
//#endif
//
//
//			// �ړ��𔽉f
//			D3DXMatrixTranslation(&mtxTranslate, g_Stone[i].pos.x,
//												 g_Stone[i].pos.y,
//												 g_Stone[i].pos.z);
//			D3DXMatrixMultiply(&g_Stone[i].mtxWorld, &g_Stone[i].mtxWorld, &mtxTranslate);
//
//			// ���[���h�}�g���b�N�X�̐ݒ�
//			SetWorldMatrix(&g_Stone[i].mtxWorld);
//
//			// �}�e���A���ݒ�
//			SetMaterial(g_Material);
//
//			// �e�N�X�`���ݒ�
//			GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(g_texNo));
//
//			// �|���S���̕`��
//			GetDeviceContext()->Draw(4, 0);
//		}
//	}
//
//	// ���C�e�B���O��L����
//	SetLightEnable(true);
//
//	// ���e�X�g�𖳌���
//	SetAlphaTestEnable(false);
}

//=============================================================================
// �΂̃p�����[�^���Z�b�g
//=============================================================================
void SetStone(D3DXVECTOR3 pos)
{
	for(int i = 0; i < STONE_NUM; i++)
	{
		//�܂��\������Ă��Ȃ��ꏊ��T��
		if(g_Stone[i].use == false)
		{
			g_Stone[i].pos = pos;

			g_Stone[i].speed = -Random(3,6);

			// �����蔻��p�T�C�Y��ݒ�
			g_Stone[i].size = D3DXVECTOR3(STONE_SIZE, STONE_SIZE, STONE_SIZE);

			// �e�̐ݒ�
			g_Stone[i].shadowSize = 80.0f;

			g_Stone[i].shadow = SetShadow(D3DXVECTOR3(g_Stone[i].pos.x, 0.0f, g_Stone[i].pos.z), g_Stone[i].shadowSize);

			g_Stone[i].use = true;

			break;
		}
	}
}


STONE* GetStone(void)
{
	return &g_Stone[0];
}
