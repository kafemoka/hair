//
// �����V�~�����[�V����
//
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <array>

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

// �t���[�����[�g
const GLfloat fps(60.0f);

// �v�Z�̌J��Ԃ���
const int loops(5);

namespace
{
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

  // �ߓ_�̈ʒu�̒��_�o�b�t�@�I�u�W�F�N�g���쐬����
  std::array<GLuint, 2> positionBuffer;
  glGenBuffers(positionBuffer.size(), positionBuffer.data());

  // �ߓ_�̈ʒu�̈�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  // �ߓ_�̈ʒu�̈�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�ɏ����ʒu��ݒ肷��
  GLint first[hairNumber];
  GLsizei count[hairNumber];
  GLfloat (*const position)[3](static_cast<GLfloat (*)[3]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
  makeHair(position, hairNumber, hairKnots, first, count);
  glUnmapBuffer(GL_ARRAY_BUFFER);

  // �ߓ_�̈ʒu�̓�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[1]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  // �ߓ_�̑��x�̒��_�o�b�t�@�I�u�W�F�N�g���쐬����
  std::array<GLuint, 2> velocityBuffer;
  glGenBuffers(velocityBuffer.size(), velocityBuffer.data());

  // �ߓ_�̑��x�̈�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
  glBindBuffer(GL_ARRAY_BUFFER, velocityBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  // �ߓ_�̑��x�̈�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�ɏ����x��ݒ肷��
  GLfloat (*const velocity)[3](static_cast<GLfloat (*)[3]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
  std::fill(velocity[0], velocity[0] + pointCount * 3, 0.0f);
  glUnmapBuffer(GL_ARRAY_BUFFER);

  // �ߓ_�̑��x�̓�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
  glBindBuffer(GL_ARRAY_BUFFER, velocityBuffer[1]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  // �ߓ_�̗͂̒��_�o�b�t�@�I�u�W�F�N�g���쐬����
  GLuint forceBuffer;
  glGenBuffers(1, &forceBuffer);

  // �ߓ_�̗͂̒��_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
  glBindBuffer(GL_ARRAY_BUFFER, forceBuffer);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  //
  // ���_�z��I�u�W�F�N�g
  //

  // ���_�z��I�u�W�F�N�g���쐬����
  std::array<GLuint, 2> vao;
  glGenVertexArrays(vao.size(), vao.data());

  // ��ڂ̒��_�z��I�u�W�F�N�g�̐ݒ���J�n����
  glBindVertexArray(vao[0]);

  // �ߓ_�̈ʒu�̈�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�� 0 �Ԃ� attribute �ϐ��Ŏ��o��
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // �ߓ_�̑��x�̈�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�� 1 �Ԃ� attribute �ϐ��Ŏ��o��
  glBindBuffer(GL_ARRAY_BUFFER, velocityBuffer[0]);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // �ߓ_�̗͂̒��_�o�b�t�@�I�u�W�F�N�g�� 2 �Ԃ� attribute �ϐ��Ŏ��o��
  glBindBuffer(GL_ARRAY_BUFFER, forceBuffer);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  // ��ڂ̒��_�z��I�u�W�F�N�g�̐ݒ���J�n����
  glBindVertexArray(vao[1]);

  // �ߓ_�̈ʒu�̓�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�� 0 �Ԃ� attribute �ϐ��Ŏ��o��
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[1]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // �ߓ_�̑��x�̈�ڂ̒��_�o�b�t�@�I�u�W�F�N�g�� 1 �Ԃ� attribute �ϐ��Ŏ��o��
  glBindBuffer(GL_ARRAY_BUFFER, velocityBuffer[1]);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // �ߓ_�̗͂̒��_�o�b�t�@�I�u�W�F�N�g�� 2 �Ԃ� attribute �ϐ��Ŏ��o��
  glBindBuffer(GL_ARRAY_BUFFER, forceBuffer);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  //
  // �e�N�X�`���o�b�t�@�I�u�W�F�N�g
  //

  // �ߓ_�̈ʒu�̃e�N�X�`���I�u�W�F�N�g���쐬����
  std::array<GLuint, 2> positionTexture;
  glGenTextures(positionTexture.size(), positionTexture.data());

  // �ߓ_�̈ʒu�̈�ڂ̃e�N�X�`���I�u�W�F�N�g
  glBindTexture(GL_TEXTURE_BUFFER, positionTexture[0]);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, positionBuffer[0]);

  // �ߓ_�̈ʒu�̓�ڂ̃e�N�X�`���I�u�W�F�N�g
  glBindTexture(GL_TEXTURE_BUFFER, positionTexture[1]);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, positionBuffer[1]);

  //
  // �v���O�����I�u�W�F�N�g
  //

  // �`��p�̃V�F�[�_�v���O������ǂݍ���
  const GLuint hairShader(ggLoadShader("hair.vert", "hair.frag"));
  const GLint hairMcLoc(glGetUniformLocation(hairShader, "mc"));
  const GLint hairNeighborLoc(glGetUniformLocation(hairShader, "neighbor"));
  const GLint hairEndpointLoc(glGetUniformLocation(hairShader, "endpoint"));

  // �ߓ_�ɉ����͂̌v�Z�p�̃V�F�[�_�v���O������ǂݍ���
  const std::array<const char *, 1> forceOut = { "force" };
  const GLuint forceShader(ggLoadShader("force.vert", nullptr, nullptr, forceOut.size(), forceOut.data()));
  const GLint forceNeighborLoc(glGetUniformLocation(forceShader, "neighbor"));
  const GLint forceEndpointLoc(glGetUniformLocation(forceShader, "endpoint"));
  const GLint forceL0Loc(glGetUniformLocation(forceShader, "l0"));
  const GLint forceCenterLoc(glGetUniformLocation(forceShader, "center"));
  const GLint forceRadiusLoc(glGetUniformLocation(forceShader, "radius"));

  // �ߓ_�̈ʒu�̍X�V�p�̃V�F�[�_�v���O������ǂݍ���
  const std::array<const char *, 2> updateOut = { "newPosition", "newVelocity" };
  const GLuint updateShader(ggLoadShader("update.vert", nullptr, nullptr, updateOut.size(), updateOut.data()));
  const GLint updateDtLoc(glGetUniformLocation(updateShader, "dt"));
  const GLint updateAnchorLoc(glGetUniformLocation(updateShader, "anchor"));

  //
  // �`��
  //

  // �`�悷�钸�_�z��o�b�t�@
  int buffer(0);

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
    glBindVertexArray(vao[buffer]);

    // �`��p�̃V�F�[�_�v���O�������g�p����
    glUseProgram(hairShader);

    // �r���[�v���W�F�N�V�����ϊ��s���ݒ肷��
    glUniformMatrix4fv(hairMcLoc, 1, GL_FALSE, mc.get());

    // �ߖT�̒��_�̈ʒu�̃e�N�X�`���I�o�b�t�@�u�W�F�N�g�̃T���v�����w�肷��
    glUniform1i(hairNeighborLoc, 0);

    // �ߖT�̒��_�̈ʒu�̃e�N�X�`���o�b�t�@�I�u�W�F�N�g����������
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, positionTexture[buffer]);

    // ���_�z���`�悷��
    for (int i = 0; i < hairNumber; ++i)
    {
      glUniform2i(hairEndpointLoc, first[i], first[i] + count[i] - 1);
      glDrawArrays(GL_LINE_STRIP, first[i], count[i]);
    }

    // �t���[���o�b�t�@�����ւ���
    window.swapBuffers();

    //
    // �v�Z
    //

    // ���X�^���C�U���~�߂�
    glEnable(GL_RASTERIZER_DISCARD);

    // �v�Z���J��Ԃ�
    for (int l = 0; l < loops; ++l)
    {
      //
      // �ߓ_�ɉ����͂̎Z�o
      //

      // �ߓ_�ɉ����͂̌v�Z�p�̃V�F�[�_�v���O�������g�p����
      glUseProgram(forceShader);

      // �΂˂̎��R����ݒ肷��
      glUniform1f(forceL0Loc, hairLength / GLfloat(hairKnots - 1));

      // ���̒��S�ʒu��ݒ肷��
      glUniform3fv(forceCenterLoc, 1, headCenter);

      // ���̔��a��ݒ肷��
      glUniform1f(forceRadiusLoc, headRadius);

      // �ߖT�̒��_�̈ʒu�̃e�N�X�`���I�o�b�t�@�u�W�F�N�g�̃T���v�����w�肷��
      glUniform1i(forceNeighborLoc, 0);

      // �ߖT�̒��_�̈ʒu�̃e�N�X�`���o�b�t�@�I�u�W�F�N�g����������
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_BUFFER, positionTexture[buffer]);

      // �ߓ_�̗͂̒��_�o�b�t�@�I�u�W�F�N�g�� force �̃^�[�Q�b�g�ɂ���
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, forceBuffer);

      // Transform Feedback ��L���ɂ��Čv�Z�����s����
      glBeginTransformFeedback(GL_POINTS);
      for (int i = 0; i < hairNumber; ++i)
      {
        glUniform2i(forceEndpointLoc, first[i], first[i] + count[i] - 1);
        glDrawArrays(GL_POINTS, first[i], count[i]);
      }
      glEndTransformFeedback();

      //
      // �ߓ_�̈ʒu�Ƒ��x�̍X�V
      //

      // �X�V�p�̃V�F�[�_�v���O�����̎g�p�J�n
      glUseProgram(updateShader);

      // �^�C���X�e�b�v��ݒ肷��
      glUniform1f(updateDtLoc, 1.0f / (fps * GLfloat(loops)));

      // ��������̐ߓ_�̈ʒu�̒��_�o�b�t�@�I�u�W�F�N�g�� newPosition �̃^�[�Q�b�g�ɂ���
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, positionBuffer[1 - buffer]);
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velocityBuffer[1 - buffer]);

      // Transform Feedback ��L���ɂ��Čv�Z�����s����
      glBeginTransformFeedback(GL_POINTS);
      for (int i = 0; i < hairNumber; ++i)
      {
        glUniform1i(updateAnchorLoc, first[i]);
        glDrawArrays(GL_POINTS, first[i], count[i]);
      }
      glEndTransformFeedback();

      // ���_�z��I�u�W�F�N�g�̑I�������ւ���
      glBindVertexArray(vao[buffer = 1 - buffer]);
    }

    // ���X�^���C�U���g�p����
    glDisable(GL_RASTERIZER_DISCARD);
  }
}
