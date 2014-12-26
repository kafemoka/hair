#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// ばね定数
const float k0 = 100.0;

// 節点のローカル座標系での位置
layout (location = 0) in vec4 position;

// 近傍の節点を取得するテクスチャバッファオブジェクト
uniform samplerBuffer neighbor;

// 一本の髪の毛の最初と最後の節点のインデックス
uniform ivec2 endpoint;

// ばねの自然長
uniform float l0;

// 頭の中心位置
uniform vec3 center;

// 頭の半径
uniform float radius;

// 節点に加わる力を保存するフィードバックバッファ
out vec3 force;

// 隣接する節点との間の伸縮に対する抵抗力を求める
vec3 elasticity(const in int i)
{
  vec3 d = (position - texelFetch(neighbor, i)).xyz;
  return (k0 * l0 / length(d) - k0) * d;
}

void main()
{
  vec3 v = position.xyz - center;
  float l = length(v);
  float f = 100.0 * max(radius - l, 0.0);
  force = f * f * v / l;
  if (gl_VertexID > endpoint.s) force += elasticity(gl_VertexID - 1);
  if (gl_VertexID < endpoint.t) force += elasticity(gl_VertexID + 1);
}
