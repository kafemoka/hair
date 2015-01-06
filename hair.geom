#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

layout (lines_adjacency) in;
layout (line_strip, max_vertices = 20) out;

// 定数
const int division = 4;                             // 曲線の分割数
const vec4 pl = vec4(1.0, 6.0, 4.0, 1.0);           // 光源位置

// uniform 変数
uniform mat4 mc;                                    // ビュープロジェクション変換行列

// ラスタライザに送る頂点属性
out vec3 v;                                         // 視線ベクトル
out vec3 l;                                         // 光線ベクトル
out vec3 t;                                         // 接線ベクトル

void main()
{
  vec4 m0 = (gl_in[2].gl_Position - gl_in[0].gl_Position) * 0.5;    // (P1 - P-1) / 2
  vec4 m1 = (gl_in[3].gl_Position - gl_in[1].gl_Position) * 0.5;    // (P2 - P0) / 2
  vec4 dp = gl_in[1].gl_Position - gl_in[2].gl_Position;            // P0 - P1
  vec4 a = 2.0 * dp + m0 + m1;
  vec4 b = -3.0 * dp - 2.0 * m0 - m1;
  vec4 c = m0;
  vec4 d = gl_in[1].gl_Position;

  for (int i = 0; i <= division; ++i)
  {
    float s = float(i) / float(division);

    // Catmull-Rom 曲線
    vec4 position = ((a * s + b) * s + c) * s + d;

    // スクリーン座標系の座標値
    gl_Position = mc * position;

    // 視線ベクトルは頂点位置の逆ベクトル
    v = -normalize(position.xyz);

    // 光線ベクトルは頂点から光源に向かうベクトル
    l = normalize((pl - position * pl.w).xyz);

    // 接線ベクトルは Catmull-Rom 曲線 の微分
    t = normalize(((3.0 * a * s + 2.0 * b) * s + c).xyz);

    // 頂点の出力
    EmitVertex();
  }

  // 図形の終了
  EndPrimitive();
}
