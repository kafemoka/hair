//
// 頭髪シミュレーション
//
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>

// ウィンドウ関連の処理
#include "Window.h"

// 髪データの設定
const int hairNumber(10000);
const int hairKnots(11);
const GLfloat hairLength(1.0f);
const GLfloat headRadius(0.5f);
const GLfloat headCenter[] = { 0.0f, 0.4f, 0.0f };

// 背景色
const GLfloat back[] = { 1.0f, 1.0f, 0.9f, 0.0f };

namespace
{
  //
  // 配列の要素数の取得
  //
  template <typename TYPE, std::size_t SIZE>
  GLsizei array_sizeof(const TYPE (&)[SIZE])
  {
    return GLsizei(SIZE);
  }

  //
  // 頭髪データの作成
  //
  void makeHair(GLfloat (*position)[3], int number, int knots, GLint *first, GLsizei *count)
  {
    for (int i = 0; i < number; ++i)
    {
      // 球面上にランダムに点を生成する
      const GLfloat t(6.2831853f * GLfloat(rand()) / (GLfloat(RAND_MAX) + 1.0f));
      const GLfloat cp(GLfloat(rand()) / GLfloat(RAND_MAX));
      const GLfloat sp(sqrt(1.0f - cp * cp));
      const GLfloat ct(cos(t)), st(sin(t));
      const GLfloat x(sp * ct), y(cp), z(sp * st);

      // 一本の髪の毛の最初の節点の番号と節点の数を記録する
      *first++ = knots * i;
      *count++ = knots;

      // 球面の法線方向に節点を一直線に配置する
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
  // 終了処理
  //
  void cleanup()
  {
    // GLFW の終了処理
    glfwTerminate();
  }
}

//
// メインプログラム
//
int main()
{
  // GLFW を初期化する
  if (glfwInit() == GL_FALSE)
  {
    // GLFW の初期化に失敗した
    std::cerr << "Can't initialize GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  // プログラム終了時の処理を登録する
  atexit(cleanup);

  // OpenGL Version 3.2 Core Profile を選択する
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // ウィンドウモードでウィンドウを開く
  Window window(1280, 800, "Hair Simulator");

  if (!window.get())
  {
    // ウィンドウが作成できなかった
    std::cerr << "Can't open GLFW window" << std::endl;
    exit(1);
  }

  // 隠面消去処理を有効にする
  glEnable(GL_DEPTH_TEST);

  // 背面はカリングしない
  glDisable(GL_CULL_FACE);

  // 背景色を設定する
  glClearColor(back[0], back[1], back[2], back[3]);

  //
  // 頂点バッファオブジェクト
  //

  // 髪の本数と節点の数
  const GLsizeiptr pointCount(hairNumber * hairKnots);

  // 節点の位置の頂点バッファオブジェクトを作成する
  GLuint positionBuffer[1];
  glGenBuffers(array_sizeof(positionBuffer), positionBuffer);

  // 節点の位置の頂点バッファオブジェクトのメモリを確保する
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STATIC_DRAW);

  // 節点の位置の一つ目の頂点バッファオブジェクトに初期位置を設定する
  GLint first[hairNumber];
  GLsizei count[hairNumber];
  GLfloat (*const position)[3](static_cast<GLfloat (*)[3]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
  makeHair(position, hairNumber, hairKnots, first, count);
  glUnmapBuffer(GL_ARRAY_BUFFER);

  //
  // 頂点配列オブジェクト
  //

  // 頂点配列オブジェクトを作成する
  GLuint vao[1];
  glGenVertexArrays(array_sizeof(vao), vao);

  // 頂点配列オブジェクトの設定を開始する
  glBindVertexArray(vao[0]);

  // 節点の位置の頂点バッファオブジェクトを 0 番の attribute 変数で取り出す
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  //
  // プログラムオブジェクト
  //

  // 描画用のシェーダプログラムを読み込む
  const GLuint hairShader(ggLoadShader("hair.vert", "hair.frag"));
  const GLint hairMcLoc(glGetUniformLocation(hairShader, "mc"));

  //
  // 描画
  //

  // ウィンドウが開いている間くり返し描画する
  while (!window.shouldClose())
  {
    // 画面クリア
    window.clear();

    // ビュープロジェクション変換行列
    const GgMatrix mc(window.getMp() * window.getMv());

    //
    // 通常の描画
    //

    // 頂点配列オブジェクトを選択する
    glBindVertexArray(vao[0]);

    // 描画用のシェーダプログラムを使用する
    glUseProgram(hairShader);

    // ビュープロジェクション変換行列を設定する
    glUniformMatrix4fv(hairMcLoc, 1, GL_FALSE, mc.get());

    // 頂点配列を描画する
    glMultiDrawArrays(GL_LINE_STRIP, first, count, hairNumber);

    // フレームバッファを入れ替える
    window.swapBuffers();
  }
}
