//
// ウィンドウ関連の処理
//
#include <iostream>
#include <cmath>

// クラス定義
#include "Window.h"

// カメラの初期状態 (単位 m)
const GLfloat cameraDistance(2.0f);   // カメラの初期位置
const GLfloat cameraFovy(1.0f);       // カメラの画角
const GLfloat cameraNear(0.1f);       // 前方面までの距離
const GLfloat cameraFar(50.0f);       // 後方面までの距離
const GLfloat cameraFactor(0.1f);     // カメラの移動係数

//
// コンストラクタ
//
Window::Window(int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share)
  : window(glfwCreateWindow(width, height, title, monitor, share))
  , distance(cameraDistance)
  , run(false)
{
  if (!window) return;

  // 現在のウィンドウを処理対象にする
  glfwMakeContextCurrent(window);

  // 作成したウィンドウに対する設定
  glfwSwapInterval(1);

  // ウィンドウのサイズ変更時に呼び出す処理の登録
  glfwSetFramebufferSizeCallback(window, resize);

  // マウスボタンを操作したときの処理
  glfwSetMouseButtonCallback(window, mouse);

  // マウスホイール操作時に呼び出す処理
  glfwSetScrollCallback(window, wheel);

  // キーボードを操作した時の処理
  glfwSetKeyCallback(window, keyboard);

  // マウスカーソルを表示する
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // このインスタンスの this ポインタを記録しておく
  glfwSetWindowUserPointer(window, this);

  // ゲームグラフィックス特論の都合にもとづく初期化
	ggInit();

  // 投影変換行列・ビューポートの初期化
  resize(window, width, height);
}

//
// デストラクタ
//
Window::~Window()
{
  // ウィンドウ破棄
  glfwDestroyWindow(window);
}

//
// ウィンドウを閉じるべきかを判定する
//
//   ・描画ループの継続条件として使う
//
bool Window::shouldClose()
{
  // ウィンドウを閉じるか ESC キーがタイプされていれば真
  return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
}

//
// 画面クリア
//
//   ・図形の描画開始前に呼び出す
//   ・画面の消去などを行う
//
void Window::clear()
{
  // カラーバッファとデプスバッファを消去
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // モデル変換行列を設定する
  mm = tbR.getMatrix();

  // ビュー変換行列を設定する
  mv = ggTranslate(0.0f, 0.0f, -distance) * tbL.getMatrix();
}

//
// カラーバッファを入れ替えてイベントを取り出す
//
//   ・図形の描画終了後に呼び出す
//   ・ダブルバッファリングのバッファの入れ替えを行う
//   ・キーボード操作等のイベントを取り出す
//
void Window::swapBuffers()
{
  // エラーチェック
  ggError("SwapBuffers");

  // カラーバッファを入れ替える
  glfwSwapBuffers(window);

  // イベントを取り出す
  glfwPollEvents();

  // マウスの位置を調べる
  double x, y;
  glfwGetCursorPos(window, &x, &y);

  // 左ボタンドラッグ
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
  {
    tbL.motion(float(x), float(y));
  }

  // 右ボタンドラッグ
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
  {
    tbR.motion(float(x), float(y));
  }
}

//
// ウィンドウのサイズ変更時の処理
//
//   ・ウィンドウのサイズ変更時にコールバック関数として呼び出される
//   ・ウィンドウの作成時には明示的に呼び出す
//
void Window::resize(GLFWwindow *window, int width, int height)
{
  // このインスタンスの this ポインタを得る
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // ウィンドウ全体をビューポートにする
    glViewport(0, 0, width, height);

    // トラックボール処理の範囲を設定する
    instance->tbL.region(width, height);
    instance->tbR.region(width, height);

    // 投影変換行列を求める
    instance->mp.loadPerspective(cameraFovy, GLfloat(width) / GLfloat(height), cameraNear, cameraFar);
  }
}

//
// マウスボタンを操作したときの処理
//
//   ・マウスボタンを押したときにコールバック関数として呼び出される
//
void Window::mouse(GLFWwindow *window, int button, int action, int mods)
{
  // このインスタンスの this ポインタを得る
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // マウスの現在位置を取り出す
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    switch (button)
    {
    case GLFW_MOUSE_BUTTON_1:
      if (action)
      {
        // ドラッグ開始
        instance->tbL.start(float(x), float(y));
      }
      else
      {
        // ドラッグ終了
        instance->tbL.stop(float(x), float(y));
      }
      break;
    case GLFW_MOUSE_BUTTON_2:
      if (action)
      {
        // ドラッグ開始
        instance->tbR.start(float(x), float(y));
      }
      else
      {
        // ドラッグ終了
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
// マウスホイール操作時の処理
//
//   ・マウスホイールを操作した時にコールバック関数として呼び出される
//
void Window::wheel(GLFWwindow *window, double x, double y)
{
  // このインスタンスの this ポインタを得る
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    instance->distance += cameraFactor * float(y);
  }
}

//
// キーボードをタイプした時の処理
//
//   ．キーボードをタイプした時にコールバック関数として呼び出される
//
void Window::keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  // このインスタンスの this ポインタを得る
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
      case GLFW_KEY_R:
        // カメラの位置をリセットする
        instance->distance = cameraDistance;
      case GLFW_KEY_O:
        // 物体の回転をリセットする
        instance->tbL.reset();
        instance->tbR.reset();
        break;
      case GLFW_KEY_SPACE:
        // 計算の実行許可を反転する
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
