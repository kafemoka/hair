#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main()
{
  fc = vec4(0.3, 0.1, 0.0, 1.0);
}
