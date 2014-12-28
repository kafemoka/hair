#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// �萔
const vec4 pl = vec4(1.0, 6.0, 4.0, 1.0);           // �����ʒu

// ���_����
layout (location = 0) in vec4 position;             // �ߓ_�̃��[�J�����W�n�ł̈ʒu

// uniform �ϐ�
uniform mat4 mc;                                    // �r���[�v���W�F�N�V�����ϊ��s��
uniform samplerBuffer neighbor;                     // �ߖT�̐ߓ_���擾����e�N�X�`���o�b�t�@�I�u�W�F�N�g
uniform ivec2 endpoint;                             // ��{�̔��̖т̍ŏ��ƍŌ�̐ߓ_�̃C���f�b�N�X

// �t���O�����g�V�F�[�_�ɑ���ߓ_�̎����E�����E�ڐ��x�N�g��
out vec3 v, l, t;

void main()
{
  // �X�N���[�����W�n�̍��W�l
  gl_Position = mc * position;

  // �����x�N�g���͒��_�ʒu�̋t�x�N�g��
  v = -normalize(position.xyz);

  // �����x�N�g���͒��_��������Ɍ������x�N�g��
  l = normalize((pl - position * pl.w).xyz);

  // �ڐ��x�N�g���͏����Ώۂ̐ߓ_�̑O�̐ߓ_���玟�̐ߓ_�Ɍ������x�N�g��
  t = normalize(texelFetch(neighbor, max(gl_VertexID + 1, endpoint.s)) -
                texelFetch(neighbor, min(gl_VertexID - 1, endpoint.t))).xyz;
}
