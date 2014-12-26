#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// 節点のローカル座標系での位置
layout (location = 0) in vec4 position;

// ビュープロジェクション変換行列
uniform mat4 mc;

void main()
{
  // スクリーン座標系の座標値
  gl_Position = mc * position;
}
