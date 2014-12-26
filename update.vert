#version 150
#extension GL_ARB_explicit_attrib_location : enable

// 頂点属性
layout (location = 0) in vec4 position;             // 節点のローカル座標系での位置
layout (location = 1) in vec3 velocity;             // 節点のローカル座標系での速度
layout (location = 2) in vec3 force;                // 節点が受ける力

// uniform 変数
uniform int anchor;                                 // 毛髪の根元の節点の頂点番号
uniform float dt;                                   // タイムステップ

// フィードバックバッファ
out vec3 newPosition;                               // 更新された節点のローカル座標系での位置
out vec3 newVelocity;                               // 更新された節点のローカル座標系での速度

// 定数
const float m = 0.05;                               // 質点の質量
const vec3 g = vec3(0.0, -9.8, 0.0);                // 重力加速度
const float c = 1.0;                                // 空気抵抗

void main()
{
  // 根元の点は更新しない
  if (gl_VertexID <= anchor)
  {
      newPosition = position.xyz;
      newVelocity = velocity;
      return;
  }

  // 現在の加速度
  vec3 a = (force - c * velocity) / m + g;

  // 更新後の速度
  vec3 v = velocity + a * dt;

  // 位置と速度の更新 (Heun 法)
  newPosition = position.xyz + 0.5 * (velocity + v) * dt;
  newVelocity = v;
}
