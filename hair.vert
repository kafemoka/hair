#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// 頂点属性
layout (location = 0) in vec4 position;             // 節点のローカル座標系での位置

// uniform 変数
uniform mat4 mc;                                    // ビュープロジェクション変換行列

void main()
{
  // スクリーン座標系の座標値
  gl_Position = mc * position;
}
