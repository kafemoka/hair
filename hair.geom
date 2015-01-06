#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

layout (lines_adjacency) in;
layout (line_strip, max_vertices = 20) out;

// �萔
const int division = 4;                             // �Ȑ��̕�����
const vec4 pl = vec4(1.0, 6.0, 4.0, 1.0);           // �����ʒu

// uniform �ϐ�
uniform mat4 mc;                                    // �r���[�v���W�F�N�V�����ϊ��s��

// ���X�^���C�U�ɑ��钸�_����
out vec3 v;                                         // �����x�N�g��
out vec3 l;                                         // �����x�N�g��
out vec3 t;                                         // �ڐ��x�N�g��

void main()
{
  vec4 m0 = (gl_in[2].gl_Position - gl_in[0].gl_Position) * 0.5;    // (P1 - P-1) / 2
  vec4 m1 = (gl_in[3].gl_Position - gl_in[1].gl_Position) * 0.5;    // (P2 - P0) / 2
  vec4 dp = gl_in[1].gl_Position - gl_in[2].gl_Position;            // P0 - P1
  vec4 a = 2.0 * dp + m0 + m1;
  vec4 b = -3.0 * dp - 2.0 * m0 - m1;
  vec4 c = m0;
  vec4 d = gl_in[1].gl_Position;

  for (int i = 0; i <= division; ++i)
  {
    float s = float(i) / float(division);

    // Catmull-Rom �Ȑ�
    vec4 position = ((a * s + b) * s + c) * s + d;

    // �X�N���[�����W�n�̍��W�l
    gl_Position = mc * position;

    // �����x�N�g���͒��_�ʒu�̋t�x�N�g��
    v = -normalize(position.xyz);

    // �����x�N�g���͒��_��������Ɍ������x�N�g��
    l = normalize((pl - position * pl.w).xyz);

    // �ڐ��x�N�g���� Catmull-Rom �Ȑ� �̔���
    t = normalize(((3.0 * a * s + 2.0 * b) * s + c).xyz);

    // ���_�̏o��
    EmitVertex();
  }

  // �}�`�̏I��
  EndPrimitive();
}
