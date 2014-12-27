//
// �����V�~�����[�V����
//
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// ���f�[�^�̐ݒ�
const int hairNumber(10000);
const int hairKnots(11);
const GLfloat hairLength(1.0f);
const GLfloat headRadius(0.5f);
const GLfloat headCenter[] = { 0.0f, 0.4f, 0.0f };

// �w�i�F
const GLfloat back[] = { 1.0f, 1.0f, 0.9f, 0.0f };

namespace
{
  //
  // �z��̗v�f���̎擾
  //
  template <typename TYPE, std::size_t SIZE>
  GLsizei array_sizeof(const TYPE (&)[SIZE])
  {
    return GLsizei(SIZE);
  }

  //
  // �����f�[�^�̍쐬
  //
  void makeHair(GLfloat (*position)[3], int number, int knots, GLint *first, GLsizei *count)
  {
    for (int i = 0; i < number; ++i)
    {
      // ���ʏ�Ƀ����_���ɓ_�𐶐�����
      const GLfloat t(6.2831853f * GLfloat(rand()) / (GLfloat(RAND_MAX) + 1.0f));
      const GLfloat cp(GLfloat(rand()) / GLfloat(RAND_MAX));
      const GLfloat sp(sqrt(1.0f - cp * cp));
      const GLfloat ct(cos(t)), st(sin(t));
      const GLfloat x(sp * ct), y(cp), z(sp * st);

      // ��{�̔��̖т̍ŏ��̐ߓ_�̔ԍ��Ɛߓ_�̐����L�^����
      *first++ = knots * i;
      *count++ = knots;

      // ���ʂ̖@�������ɐߓ_���꒼���ɔz�u����
      for (int k = 0; k < knots; ++k)
      {
        const GLfloat o(hairLength * GLfloat(k) / GLfloat(knots - 1) + headRadius);
        (*position)[0] = o * x + headCenter[0];
        (*position)[1] = o * y + headCenter[1];
        (*position)[2] = o * z + headCenter[2];
        ++position;
      }
    }
  }

  //
  // �I������
  //
  void cleanup()
  {
    // GLFW �̏I������
    glfwTerminate();
  }
}

//
// ���C���v���O����
//
int main()
{
  // GLFW ������������
  if (glfwInit() == GL_FALSE)
  {
    // GLFW �̏������Ɏ��s����
    std::cerr << "Can't initialize GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  // �v���O�����I�����̏�����o�^����
  atexit(cleanup);

  // OpenGL Version 3.2 Core Profile ��I������
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // �E�B���h�E���[�h�ŃE�B���h�E���J��
  Window window(1280, 800, "Hair Simulator");

  if (!window.get())
  {
    // �E�B���h�E���쐬�ł��Ȃ�����
    std::cerr << "Can't open GLFW window" << std::endl;
    exit(1);
  }

  // �B�ʏ���������L���ɂ���
  glEnable(GL_DEPTH_TEST);

  // �w�ʂ̓J�����O���Ȃ�
  glDisable(GL_CULL_FACE);

  // �w�i�F��ݒ肷��
  glClearColor(back[0], back[1], back[2], back[3]);

  //
  // ���_�o�b�t�@�I�u�W�F�N�g
  //

  // ���̖{���Ɛߓ_�̐�
  const GLsizeiptr pointCount(hairNumber * hairKnots);

  // �ߓ_�̈ʒu�̒��_�o�b�t�@�I�u�W�F�N�g���쐬����
  GLuint positionBuffer[1];
  glGenBuffers(array_sizeof(positionBuffer), positionBuffer);

  // �ߓ_�̈ʒu�̒��_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STATIC_DRAW);

  // �ߓ_�̈ʒu�̈�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�ɏ����ʒu��ݒ肷��
  GLint first[hairNumber];
  GLsizei count[hairNumber];
  GLfloat (*const position)[3](static_cast<GLfloat (*)[3]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
  makeHair(position, hairNumber, hairKnots, first, count);
  glUnmapBuffer(GL_ARRAY_BUFFER);

  //
  // ���_�z��I�u�W�F�N�g
  //

  // ���_�z��I�u�W�F�N�g���쐬����
  GLuint vao[1];
  glGenVertexArrays(array_sizeof(vao), vao);

  // ���_�z��I�u�W�F�N�g�̐ݒ���J�n����
  glBindVertexArray(vao[0]);

  // �ߓ_�̈ʒu�̒��_�o�b�t�@�I�u�W�F�N�g�� 0 �Ԃ� attribute �ϐ��Ŏ��o��
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  //
  // �v���O�����I�u�W�F�N�g
  //

  // �`��p�̃V�F�[�_�v���O������ǂݍ���
  const GLuint hairShader(ggLoadShader("hair.vert", "hair.frag"));
  const GLint hairMcLoc(glGetUniformLocation(hairShader, "mc"));

  //
  // �`��
  //

  // �E�B���h�E���J���Ă���Ԃ���Ԃ��`�悷��
  while (!window.shouldClose())
  {
    // ��ʃN���A
    window.clear();

    // �r���[�v���W�F�N�V�����ϊ��s��
    const GgMatrix mc(window.getMp() * window.getMv());

    //
    // �ʏ�̕`��
    //

    // ���_�z��I�u�W�F�N�g��I������
    glBindVertexArray(vao[0]);

    // �`��p�̃V�F�[�_�v���O�������g�p����
    glUseProgram(hairShader);

    // �r���[�v���W�F�N�V�����ϊ��s���ݒ肷��
    glUniformMatrix4fv(hairMcLoc, 1, GL_FALSE, mc.get());

    // ���_�z���`�悷��
    glMultiDrawArrays(GL_LINE_STRIP, first, count, hairNumber);

    // �t���[���o�b�t�@�����ւ���
    window.swapBuffers();
  }
}
