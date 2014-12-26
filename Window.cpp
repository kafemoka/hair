//
// �E�B���h�E�֘A�̏���
//
#include <iostream>
#include <cmath>

// �N���X��`
#include "Window.h"

// �J�����̏������ (�P�� m)
const GLfloat cameraDistance(2.0f);   // �J�����̏����ʒu
const GLfloat cameraFovy(1.0f);       // �J�����̉�p
const GLfloat cameraNear(0.1f);       // �O���ʂ܂ł̋���
const GLfloat cameraFar(50.0f);       // ����ʂ܂ł̋���
const GLfloat cameraFactor(0.1f);     // �J�����̈ړ��W��

//
// �R���X�g���N�^
//
Window::Window(int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share)
  : window(glfwCreateWindow(width, height, title, monitor, share))
  , distance(cameraDistance)
  , run(false)
{
  if (!window) return;

  // ���݂̃E�B���h�E�������Ώۂɂ���
  glfwMakeContextCurrent(window);

  // �쐬�����E�B���h�E�ɑ΂���ݒ�
  glfwSwapInterval(1);

  // �E�B���h�E�̃T�C�Y�ύX���ɌĂяo�������̓o�^
  glfwSetFramebufferSizeCallback(window, resize);

  // �}�E�X�{�^���𑀍삵���Ƃ��̏���
  glfwSetMouseButtonCallback(window, mouse);

  // �}�E�X�z�C�[�����쎞�ɌĂяo������
  glfwSetScrollCallback(window, wheel);

  // �L�[�{�[�h�𑀍삵�����̏���
  glfwSetKeyCallback(window, keyboard);

  // �}�E�X�J�[�\����\������
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // ���̃C���X�^���X�� this �|�C���^���L�^���Ă���
  glfwSetWindowUserPointer(window, this);

  // �Q�[���O���t�B�b�N�X���_�̓s���ɂ��ƂÂ�������
	ggInit();

  // ���e�ϊ��s��E�r���[�|�[�g�̏�����
  resize(window, width, height);
}

//
// �f�X�g���N�^
//
Window::~Window()
{
  // �E�B���h�E�j��
  glfwDestroyWindow(window);
}

//
// �E�B���h�E�����ׂ����𔻒肷��
//
//   �E�`�惋�[�v�̌p�������Ƃ��Ďg��
//
bool Window::shouldClose()
{
  // �E�B���h�E����邩 ESC �L�[���^�C�v����Ă���ΐ^
  return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
}

//
// ��ʃN���A
//
//   �E�}�`�̕`��J�n�O�ɌĂяo��
//   �E��ʂ̏����Ȃǂ��s��
//
void Window::clear()
{
  // �J���[�o�b�t�@�ƃf�v�X�o�b�t�@������
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ���f���ϊ��s���ݒ肷��
  mm = tbR.getMatrix();

  // �r���[�ϊ��s���ݒ肷��
  mv = ggTranslate(0.0f, 0.0f, -distance) * tbL.getMatrix();
}

//
// �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
//
//   �E�}�`�̕`��I����ɌĂяo��
//   �E�_�u���o�b�t�@�����O�̃o�b�t�@�̓���ւ����s��
//   �E�L�[�{�[�h���쓙�̃C�x���g�����o��
//
void Window::swapBuffers()
{
  // �G���[�`�F�b�N
  ggError("SwapBuffers");

  // �J���[�o�b�t�@�����ւ���
  glfwSwapBuffers(window);

  // �C�x���g�����o��
  glfwPollEvents();

  // �}�E�X�̈ʒu�𒲂ׂ�
  double x, y;
  glfwGetCursorPos(window, &x, &y);

  // ���{�^���h���b�O
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
  {
    tbL.motion(float(x), float(y));
  }

  // �E�{�^���h���b�O
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
  {
    tbR.motion(float(x), float(y));
  }
}

//
// �E�B���h�E�̃T�C�Y�ύX���̏���
//
//   �E�E�B���h�E�̃T�C�Y�ύX���ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
//   �E�E�B���h�E�̍쐬���ɂ͖����I�ɌĂяo��
//
void Window::resize(GLFWwindow *window, int width, int height)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // �E�B���h�E�S�̂��r���[�|�[�g�ɂ���
    glViewport(0, 0, width, height);

    // �g���b�N�{�[�������͈̔͂�ݒ肷��
    instance->tbL.region(width, height);
    instance->tbR.region(width, height);

    // ���e�ϊ��s������߂�
    instance->mp.loadPerspective(cameraFovy, GLfloat(width) / GLfloat(height), cameraNear, cameraFar);
  }
}

//
// �}�E�X�{�^���𑀍삵���Ƃ��̏���
//
//   �E�}�E�X�{�^�����������Ƃ��ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
//
void Window::mouse(GLFWwindow *window, int button, int action, int mods)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // �}�E�X�̌��݈ʒu�����o��
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    switch (button)
    {
    case GLFW_MOUSE_BUTTON_1:
      if (action)
      {
        // �h���b�O�J�n
        instance->tbL.start(float(x), float(y));
      }
      else
      {
        // �h���b�O�I��
        instance->tbL.stop(float(x), float(y));
      }
      break;
    case GLFW_MOUSE_BUTTON_2:
      if (action)
      {
        // �h���b�O�J�n
        instance->tbR.start(float(x), float(y));
      }
      else
      {
        // �h���b�O�I��
        instance->tbR.stop(float(x), float(y));
      }
      break;
    case GLFW_MOUSE_BUTTON_3:
      break;
    default:
      break;
    }
  }
}

//
// �}�E�X�z�C�[�����쎞�̏���
//
//   �E�}�E�X�z�C�[���𑀍삵�����ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
//
void Window::wheel(GLFWwindow *window, double x, double y)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    instance->distance += cameraFactor * float(y);
  }
}

//
// �L�[�{�[�h���^�C�v�������̏���
//
//   �D�L�[�{�[�h���^�C�v�������ɃR�[���o�b�N�֐��Ƃ��ČĂяo�����
//
void Window::keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
      case GLFW_KEY_R:
        // �J�����̈ʒu�����Z�b�g����
        instance->distance = cameraDistance;
      case GLFW_KEY_O:
        // ���̂̉�]�����Z�b�g����
        instance->tbL.reset();
        instance->tbR.reset();
        break;
      case GLFW_KEY_SPACE:
        // �v�Z�̎��s���𔽓]����
        instance->run = !instance->run;
        break;
      case GLFW_KEY_BACKSPACE:
      case GLFW_KEY_DELETE:
        break;
      case GLFW_KEY_UP:
        break;
      case GLFW_KEY_DOWN:
        break;
      case GLFW_KEY_RIGHT:
        break;
      case GLFW_KEY_LEFT:
        break;
      default:
        break;
      }
    }
  }
}
