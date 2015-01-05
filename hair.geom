#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

layout (lines_adjacency) in;
layout (line_strip, max_vertices = 16) out;

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
  vec4 dp = gl_in[1].gl_Position - gl_in[2].gl_Position;
  vec4 m1 = (gl_in[2].gl_Position - gl_in[0].gl_Position) * 0.5;
  vec4 m2 = (gl_in[3].gl_Position - gl_in[1].gl_Position) * 0.5;

  for (int i = 0; i <= division; ++i)
  {
    float s = float(i) / float(division);

    // Catmull-Rom �Ȑ�
    vec4 position = (((dp * 2.0f + m1 + m2) * s - dp * 3.0f - m1 * 2.0f - m2) * s + m1) * s + gl_in[1].gl_Position;

    // �X�N���[�����W�n�̍��W�l
    gl_Position = mc * position;

    // �����x�N�g���͒��_�ʒu�̋t�x�N�g��
    v = -normalize(position.xyz);

    // �����x�N�g���͒��_��������Ɍ������x�N�g��
    l = normalize((pl - position * pl.w).xyz);

    // �ڐ��x�N�g���� Catmull-Rom �Ȑ� �̔���
    t = normalize(((dp * 6.0f + (m1 + m2) * 3.0f) * s - dp * 6.0f - m1 * 4.0f - m2 * 2.0f) * s + m1).xyz;

    // ���_�̏o��
    EmitVertex();
  }

  // �}�`�̏I��
  EndPrimitive();
}
