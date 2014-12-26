#version 150
#extension GL_ARB_explicit_attrib_location : enable

// ���_����
layout (location = 0) in vec4 position;             // �ߓ_�̃��[�J�����W�n�ł̈ʒu
layout (location = 1) in vec3 velocity;             // �ߓ_�̃��[�J�����W�n�ł̑��x
layout (location = 2) in vec3 force;                // �ߓ_���󂯂��

// uniform �ϐ�
uniform int anchor;                                 // �є��̍����̐ߓ_�̒��_�ԍ�
uniform float dt;                                   // �^�C���X�e�b�v

// �t�B�[�h�o�b�N�o�b�t�@
out vec3 newPosition;                               // �X�V���ꂽ�ߓ_�̃��[�J�����W�n�ł̈ʒu
out vec3 newVelocity;                               // �X�V���ꂽ�ߓ_�̃��[�J�����W�n�ł̑��x

// �萔
const float m = 0.05;                               // ���_�̎���
const vec3 g = vec3(0.0, -9.8, 0.0);                // �d�͉����x
const float c = 1.0;                                // ��C��R

void main()
{
  // �����̓_�͍X�V���Ȃ�
  if (gl_VertexID <= anchor)
  {
      newPosition = position.xyz;
      newVelocity = velocity;
      return;
  }

  // ���݂̉����x
  vec3 a = (force - c * velocity) / m + g;

  // �X�V��̑��x
  vec3 v = velocity + a * dt;

  // �ʒu�Ƒ��x�̍X�V (Heun �@)
  newPosition = position.xyz + 0.5 * (velocity + v) * dt;
  newVelocity = v;
}
