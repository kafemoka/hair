#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// �΂˒萔
const float k0 = 100.0;

// �ߓ_�̃��[�J�����W�n�ł̈ʒu
layout (location = 0) in vec4 position;

// �ߖT�̐ߓ_���擾����e�N�X�`���o�b�t�@�I�u�W�F�N�g
uniform samplerBuffer neighbor;

// ��{�̔��̖т̍ŏ��ƍŌ�̐ߓ_�̃C���f�b�N�X
uniform ivec2 endpoint;

// �΂˂̎��R��
uniform float l0;

// ���̒��S�ʒu
uniform vec3 center;

// ���̔��a
uniform float radius;

// �ߓ_�ɉ����͂�ۑ�����t�B�[�h�o�b�N�o�b�t�@
out vec3 force;

// �אڂ���ߓ_�Ƃ̊Ԃ̐L�k�ɑ΂����R�͂����߂�
vec3 elasticity(const in int i)
{
  vec3 d = (position - texelFetch(neighbor, i)).xyz;
  return (k0 * l0 / length(d) - k0) * d;
}

void main()
{
  vec3 v = position.xyz - center;
  float l = length(v);
  float f = 100.0 * max(radius - l, 0.0);
  force = f * f * v / l;
  if (gl_VertexID > endpoint.s) force += elasticity(gl_VertexID - 1);
  if (gl_VertexID < endpoint.t) force += elasticity(gl_VertexID + 1);
}
