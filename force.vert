#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// �萔
const float k0 = 100.0;                             // �΂˒萔

// ���_����
layout (location = 0) in vec4 position;             // �ߓ_�̃��[�J�����W�n�ł̈ʒu

// uniform �ϐ�
uniform samplerBuffer neighbor;                     // �ߖT�̐ߓ_���擾����e�N�X�`���o�b�t�@�I�u�W�F�N�g
uniform ivec2 endpoint;                             // ��{�̔��̖т̍ŏ��ƍŌ�̐ߓ_�̃C���f�b�N�X
uniform float l0;                                   // �΂˂̎��R��
uniform vec3 center;                                // ���̒��S�ʒu
uniform float radius;                               // ���̔��a

// �t�B�[�h�o�b�N�o�b�t�@
out vec3 force;                                     // �ߓ_�ɉ�����

// �אڂ���ߓ_�Ƃ̊Ԃ̐L�k�ɑ΂����R�͂����߂�
vec3 elasticity(const in int i)
{
  // �ߓ_�̗אڂ���ߓ_����̕ψ�
  vec3 d = (position - texelFetch(neighbor, i)).xyz;

  // �ߓ_�̕ψʂɂ��ƂÂ���
  return k0 * (l0  - length(d)) * normalize(d);
}

void main()
{
  // �ߓ_�̓��̒��S�ʒu����̕ψ�
  vec3 v = position.xyz - center;

  // �ߓ_�̓��̒��S����̋���
  float l = length(v);

  // ���ɂ߂荞�񂾐ߓ_�𓪂̕\�ʂɉ����o����
  float f = max(radius - l, 0.0);

  // �����Ɛ�[�ȊO�̐ߓ_�ł͗אڂ���ߓ_����󂯂�͂�������
  force = 10000.0 * f * f * v / l
        + step(0.0, float(gl_VertexID - 1 - endpoint.s)) * elasticity(gl_VertexID - 1)
        + step(0.0, float(endpoint.t - gl_VertexID - 1)) * elasticity(gl_VertexID + 1);
}
