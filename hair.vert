#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// �ߓ_�̃��[�J�����W�n�ł̈ʒu
layout (location = 0) in vec4 position;

// �r���[�v���W�F�N�V�����ϊ��s��
uniform mat4 mc;

void main()
{
  // �X�N���[�����W�n�̍��W�l
  gl_Position = mc * position;
}
