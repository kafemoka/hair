#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// ���_����
layout (location = 0) in vec4 position;             // �ߓ_�̃��[�J�����W�n�ł̈ʒu

// uniform �ϐ�
uniform mat4 mc;                                    // �r���[�v���W�F�N�V�����ϊ��s��

void main()
{
  // �X�N���[�����W�n�̍��W�l
  gl_Position = mc * position;
}
