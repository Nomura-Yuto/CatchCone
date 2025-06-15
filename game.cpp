/*==============================================================================

   �Q�[������ [game.cpp]
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
   �萔��`
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   �\���̐錾
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   �v���g�^�C�v�錾
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
   �O���[�o���ϐ��̒�`
------------------------------------------------------------------------------*/
static int g_BGMNo = 0;
static LIGHT		g_DirectionalLight;	// �f�B���N�V���i�����C�g
static LIGHT		g_PointLight;		// �|�C���g���C�g


/*------------------------------------------------------------------------------
   �������֐�
------------------------------------------------------------------------------*/
void InitGame(void)
{
	// �J�����̏�����
	InitCamera();

	// �e�̏�����
	InitShadow();

	// �|�����C���̏�����
	InitPolyLine();

	// �n�ʂ̏�����
	InitMeshField(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 100, 100, 100.0f, 100.0f);

	// ��̏�����
	InitMeshSky(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 8000.0f, 16, 8);

	// �R�̏�����
	InitMeshMountain(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 16, 1, 5000.0f, 300.0f);

	// �S�[���̏�����
	InitGoal();

	// �v���C���[�̏�����
	InitPlayer();

	// �΂̏�����
	InitStone();

	// �R�[���̏�����
	InitCorn();

	// �X�R�A�̏�����
	InitScore();

	// �̗͂̏�����
	InitHealth();

	SetLightEnable(true);

	SetAlphaTestEnable(true);

	g_BGMNo = LoadSound((char*)"data/BGM/yasumi_jikan.wav");

	SetVolume(g_BGMNo, 0.8f);
	PlaySound(g_BGMNo, -1);

	// ���s�����̐ݒ�i���E���Ƃ炷���j
	g_DirectionalLight.Type = LIGHT_TYPE_DIRECTIONAL;				// ���s����
	g_DirectionalLight.Enable = true;								// ���̃��C�g��ON
	g_DirectionalLight.Direction = D3DXVECTOR4(0.0f, -1.0f, 0.0f, 0.0f);// ���̌���
	g_DirectionalLight.Diffuse = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);	// ���̐F
	SetLight(0, &g_DirectionalLight);								// ����Őݒ肵�Ă���

	// �_�����̐ݒ�
	g_PointLight.Type = LIGHT_TYPE_POINT;						// ���s����
	g_PointLight.Enable = true;									// ���̃��C�g��ON
	g_PointLight.Position = D3DXVECTOR4(-200.0f, 200.0f, -200.0f, 0.0f);// ���̍��W
	g_PointLight.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// ���̐F
	g_PointLight.Attenuation = 500.0f;							// ��������
	SetLight(1, &g_PointLight);									// ����Őݒ肵�Ă���
}

/*------------------------------------------------------------------------------
   �I������������֐�
------------------------------------------------------------------------------*/
void UninitGame()
{
	//�������Ƃ͋t���ɏI���������s��
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
   �X�V����������֐�
------------------------------------------------------------------------------*/
void UpdateGame(void)
{
	// �J�����X�V
	UpdateCamera();

	// �n�ʂ̍X�V����
	UpdateMeshField();
	UpdateMeshSky();
	UpdateMeshMountain();

	// �S�[���̍X�V����
	UpdateGoal();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �΂̍X�V����
	UpdateStone();

	// �R�[���̍X�V����
	UpdateCorn();

	// �e�̍X�V����
	UpdateShadow();

	// �|�����C���̍X�V����
	UpdatePolyLine();

	// �X�R�A�̍X�V����
	UpdateScore();

	// �̗͂̍X�V����
	UpdateHealth();

	// �����蔻��̍X�V����
	UpdateCollision();

	// �̗͂�0�ȉ��ɂȂ����Ƃ�
	if (isAlive() && GetFadeState() == FADE_NONE) 
	{
		SetVolume(g_BGMNo, 0.1f);

		//RESULT�ֈڍs����
		SceneTransition(SCENE_RESULT);
	}

	//�X�y�[�X�L�[��������Ă��āA�t�F�[�h�������ł͂Ȃ��Ƃ�
	if (GetKeyboardTrigger(DIK_RETURN) && GetFadeState() == FADE_NONE) 
	{
		SetVolume(g_BGMNo, 0.1f);

		//RESULT�ֈڍs����
		SceneTransition(SCENE_RESULT);
	}
}

/*------------------------------------------------------------------------------
   �`�揈��������֐�
------------------------------------------------------------------------------*/
void DrawGame(void)
{
	// �v���C���[���_
	D3DXVECTOR3 pos = GetPlayer()->pos;
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();

	// �[�x�e�X�g�L��
	SetDepthEnable(true);

	// ���C�e�B���O�L��
	SetLightEnable(true);

	// �n�ʂ̕`�揈��
	DrawMeshSky();
	DrawMeshField();
	DrawMeshMountain();

	// �e�̕`�揈��
	DrawShadow();

	// �S�[���̕`�揈��
	DrawGoal();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �΂̕`�揈��
	DrawStone();

	// �R�[���̕`�揈��
	DrawCorn();

	// �|�����C���̕`�揈��
	DrawPolyLine();

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();//���W��2D�ϊ�

	// 2D�`��Ȃ̂Ő[�x����
	SetDepthEnable(false);

	// �X�R�A�̕`�揈��
	DrawScore();

	// �̗͂̕`�揈��
	DrawHealth();
}

