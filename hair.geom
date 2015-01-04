#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

layout (lines_adjacency) in;
layout (line_strip, max_vertices = 16) out;

// バーテックスシェーダから受け取る節点の視線・光線・接線ベクトル
in vec3 v, l, t;


//
// Catmull-Rom Spline
//
vec3 catmull_rom(const in vec3 p0, const in vec3 p1, const in p2, const in vec3 p3, float t)
{
  vec3 dp = p1 - p2, m1 = (p2 - p0) * 0.5f, m2 = (p3 - p1) * 0.5f;
  return (((dp * 2.0f + m1 + m2) * t - dp * 3.0f - m1 * 2.0f - m2) * t + m1) * t + p1;
}
