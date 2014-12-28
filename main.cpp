//
// 頭髪シミュレーション
//
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <array>

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

// フレームレート
const GLfloat fps(60.0f);

// 計算の繰り返し回数
const int loops(5);

namespace
{
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

  // 節点の位置の頂点バッファオブジェクトを二つ作成する
  std::array<GLuint, 2> positionBuffer;
  glGenBuffers(positionBuffer.size(), positionBuffer.data());

  // 節点の位置の一つ目の頂点バッファオブジェクトのメモリを確保する
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  // 節点の位置の一つ目の頂点バッファオブジェクトに初期位置を設定する
  GLint first[hairNumber];
  GLsizei count[hairNumber];
  GLfloat (*const position)[3](static_cast<GLfloat (*)[3]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
  makeHair(position, hairNumber, hairKnots, first, count);
  glUnmapBuffer(GL_ARRAY_BUFFER);

  // 節点の位置の二つ目の頂点バッファオブジェクトのメモリを確保する
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[1]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  // 節点の速度の頂点バッファオブジェクトを二つ作成する
  std::array<GLuint, 2> velocityBuffer;
  glGenBuffers(velocityBuffer.size(), velocityBuffer.data());

  // 節点の速度の一つ目の頂点バッファオブジェクトのメモリを確保する
  glBindBuffer(GL_ARRAY_BUFFER, velocityBuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  // 節点の速度の一つ目の頂点バッファオブジェクトに初速度を設定する
  GLfloat (*const velocity)[3](static_cast<GLfloat (*)[3]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
  std::fill(velocity[0], velocity[0] + pointCount * 3, 0.0f);
  glUnmapBuffer(GL_ARRAY_BUFFER);

  // 節点の速度の二つ目の頂点バッファオブジェクトのメモリを確保する
  glBindBuffer(GL_ARRAY_BUFFER, velocityBuffer[1]);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  // 節点の力の頂点バッファオブジェクトを作成する
  GLuint forceBuffer;
  glGenBuffers(1, &forceBuffer);

  // 節点の力の頂点バッファオブジェクトのメモリを確保する
  glBindBuffer(GL_ARRAY_BUFFER, forceBuffer);
  glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof (GLfloat[3]), 0, GL_STREAM_COPY);

  //
  // 頂点配列オブジェクト
  //

  // 頂点配列オブジェクトを二つ作成する
  std::array<GLuint, 2> vao;
  glGenVertexArrays(vao.size(), vao.data());

  // 一つ目の頂点配列オブジェクトの設定を開始する
  glBindVertexArray(vao[0]);

  // 節点の位置の一つ目の頂点バッファオブジェクトを 0 番の attribute 変数で取り出す
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // 節点の速度の一つ目の頂点バッファオブジェクトを 1 番の attribute 変数で取り出す
  glBindBuffer(GL_ARRAY_BUFFER, velocityBuffer[0]);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // 節点の力の頂点バッファオブジェクトを 2 番の attribute 変数で取り出す
  glBindBuffer(GL_ARRAY_BUFFER, forceBuffer);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  // 二つ目の頂点配列オブジェクトの設定を開始する
  glBindVertexArray(vao[1]);

  // 節点の位置の二つ目の頂点バッファオブジェクトを 0 番の attribute 変数で取り出す
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[1]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // 節点の速度の一つ目の頂点バッファオブジェクトを 1 番の attribute 変数で取り出す
  glBindBuffer(GL_ARRAY_BUFFER, velocityBuffer[1]);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // 節点の力の頂点バッファオブジェクトを 2 番の attribute 変数で取り出す
  glBindBuffer(GL_ARRAY_BUFFER, forceBuffer);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  //
  // テクスチャバッファオブジェクト
  //

  // 節点の位置のテクスチャオブジェクトを二つ作成する
  std::array<GLuint, 2> positionTexture;
  glGenTextures(positionTexture.size(), positionTexture.data());

  // 節点の位置の一つ目のテクスチャオブジェクト
  glBindTexture(GL_TEXTURE_BUFFER, positionTexture[0]);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, positionBuffer[0]);

  // 節点の位置の二つ目のテクスチャオブジェクト
  glBindTexture(GL_TEXTURE_BUFFER, positionTexture[1]);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, positionBuffer[1]);

  //
  // プログラムオブジェクト
  //

  // 描画用のシェーダプログラムを読み込む
  const GLuint hairShader(ggLoadShader("hair.vert", "hair.frag"));
  const GLint hairMcLoc(glGetUniformLocation(hairShader, "mc"));
  const GLint hairNeighborLoc(glGetUniformLocation(hairShader, "neighbor"));
  const GLint hairEndpointLoc(glGetUniformLocation(hairShader, "endpoint"));

  // 節点に加わる力の計算用のシェーダプログラムを読み込む
  const std::array<const char *, 1> forceOut = { "force" };
  const GLuint forceShader(ggLoadShader("force.vert", nullptr, nullptr, forceOut.size(), forceOut.data()));
  const GLint forceNeighborLoc(glGetUniformLocation(forceShader, "neighbor"));
  const GLint forceEndpointLoc(glGetUniformLocation(forceShader, "endpoint"));
  const GLint forceL0Loc(glGetUniformLocation(forceShader, "l0"));
  const GLint forceCenterLoc(glGetUniformLocation(forceShader, "center"));
  const GLint forceRadiusLoc(glGetUniformLocation(forceShader, "radius"));

  // 節点の位置の更新用のシェーダプログラムを読み込む
  const std::array<const char *, 2> updateOut = { "newPosition", "newVelocity" };
  const GLuint updateShader(ggLoadShader("update.vert", nullptr, nullptr, updateOut.size(), updateOut.data()));
  const GLint updateDtLoc(glGetUniformLocation(updateShader, "dt"));
  const GLint updateAnchorLoc(glGetUniformLocation(updateShader, "anchor"));

  //
  // 描画
  //

  // 描画する頂点配列バッファ
  int buffer(0);

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
    glBindVertexArray(vao[buffer]);

    // 描画用のシェーダプログラムを使用する
    glUseProgram(hairShader);

    // ビュープロジェクション変換行列を設定する
    glUniformMatrix4fv(hairMcLoc, 1, GL_FALSE, mc.get());

    // 近傍の頂点の位置のテクスチャオバッファブジェクトのサンプラを指定する
    glUniform1i(hairNeighborLoc, 0);

    // 近傍の頂点の位置のテクスチャバッファオブジェクトを結合する
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, positionTexture[buffer]);

    // 頂点配列を描画する
    for (int i = 0; i < hairNumber; ++i)
    {
      glUniform2i(hairEndpointLoc, first[i], first[i] + count[i] - 1);
      glDrawArrays(GL_LINE_STRIP, first[i], count[i]);
    }

    // フレームバッファを入れ替える
    window.swapBuffers();

    //
    // 計算
    //

    // ラスタライザを止める
    glEnable(GL_RASTERIZER_DISCARD);

    // 計算を繰り返す
    for (int l = 0; l < loops; ++l)
    {
      //
      // 節点に加わる力の算出
      //

      // 節点に加わる力の計算用のシェーダプログラムを使用する
      glUseProgram(forceShader);

      // ばねの自然長を設定する
      glUniform1f(forceL0Loc, hairLength / GLfloat(hairKnots - 1));

      // 頭の中心位置を設定する
      glUniform3fv(forceCenterLoc, 1, headCenter);

      // 頭の半径を設定する
      glUniform1f(forceRadiusLoc, headRadius);

      // 近傍の頂点の位置のテクスチャオバッファブジェクトのサンプラを指定する
      glUniform1i(forceNeighborLoc, 0);

      // 近傍の頂点の位置のテクスチャバッファオブジェクトを結合する
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_BUFFER, positionTexture[buffer]);

      // 節点の力の頂点バッファオブジェクトを force のターゲットにする
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, forceBuffer);

      // Transform Feedback を有効にして計算を実行する
      glBeginTransformFeedback(GL_POINTS);
      for (int i = 0; i < hairNumber; ++i)
      {
        glUniform2i(forceEndpointLoc, first[i], first[i] + count[i] - 1);
        glDrawArrays(GL_POINTS, first[i], count[i]);
      }
      glEndTransformFeedback();

      //
      // 節点の位置と速度の更新
      //

      // 更新用のシェーダプログラムの使用開始
      glUseProgram(updateShader);

      // タイムステップを設定する
      glUniform1f(updateDtLoc, 1.0f / (fps * GLfloat(loops)));

      // もう一方の節点の位置の頂点バッファオブジェクトを newPosition のターゲットにする
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, positionBuffer[1 - buffer]);
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velocityBuffer[1 - buffer]);

      // Transform Feedback を有効にして計算を実行する
      glBeginTransformFeedback(GL_POINTS);
      for (int i = 0; i < hairNumber; ++i)
      {
        glUniform1i(updateAnchorLoc, first[i]);
        glDrawArrays(GL_POINTS, first[i], count[i]);
      }
      glEndTransformFeedback();

      // 頂点配列オブジェクトの選択を入れ替える
      glBindVertexArray(vao[buffer = 1 - buffer]);
    }

    // ラスタライザを使用する
    glDisable(GL_RASTERIZER_DISCARD);
  }
}
