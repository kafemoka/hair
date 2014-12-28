#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// 定数
const float k0 = 100.0;                             // ばね定数

// 頂点属性
layout (location = 0) in vec4 position;             // 節点のローカル座標系での位置

// uniform 変数
uniform samplerBuffer neighbor;                     // 近傍の節点を取得するテクスチャバッファオブジェクト
uniform ivec2 endpoint;                             // 一本の髪の毛の最初と最後の節点のインデックス
uniform float l0;                                   // ばねの自然長
uniform vec3 center;                                // 頭の中心位置
uniform float radius;                               // 頭の半径

// フィードバックバッファ
out vec3 force;                                     // 節点に加わる力

// 隣接する節点との間の伸縮に対する抵抗力を求める
vec3 elasticity(const in int i)
{
  // 節点の隣接する節点からの変位
  vec3 d = (position - texelFetch(neighbor, i)).xyz;

  // 節点の変位にもとづく力
  return (k0 * l0 / length(d) - k0) * d;
}

void main()
{
  // 節点の頭の中心位置からの変位
  vec3 v = position.xyz - center;

  // 節点の頭の中心からの距離
  float l = length(v);

  // 頭にめり込んだ節点を頭の表面に押し出す力
  float f = max(radius - l, 0.0);
  force = 10000.0 * f * f * v / l;

  // 節点が根元でなければ隣接する節点から受ける力を加える
  if (gl_VertexID > endpoint.s) force += elasticity(gl_VertexID - 1);

  // 節点が先端でなければ隣接する節点から受ける力を加える
  if (gl_VertexID < endpoint.t) force += elasticity(gl_VertexID + 1);
}
