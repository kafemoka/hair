#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// 定数
const vec4 pl = vec4(1.0, 6.0, 4.0, 1.0);           // 光源位置

// 頂点属性
layout (location = 0) in vec4 position;             // 節点のローカル座標系での位置

// uniform 変数
uniform mat4 mc;                                    // ビュープロジェクション変換行列
uniform samplerBuffer neighbor;                     // 近傍の節点を取得するテクスチャバッファオブジェクト
uniform ivec2 endpoint;                             // 一本の髪の毛の最初と最後の節点のインデックス

// フラグメントシェーダに送る節点の視線・光線・接線ベクトル
out vec3 v, l, t;

void main()
{
  // スクリーン座標系の座標値
  gl_Position = mc * position;

  // 視線ベクトルは頂点位置の逆ベクトル
  v = -normalize(position.xyz);

  // 光線ベクトルは頂点から光源に向かうベクトル
  l = normalize((pl - position * pl.w).xyz);

  // 接線ベクトルは処理対象の節点の前の節点から次の節点に向かうベクトル
  t = normalize(texelFetch(neighbor, max(gl_VertexID + 1, endpoint.s)) -
                texelFetch(neighbor, min(gl_VertexID - 1, endpoint.t))).xyz;
}
