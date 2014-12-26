#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// ����
const vec4 lamb   = vec4(0.1, 0.1, 0.1, 1.0);       // ���������̋��x
const vec4 ldiff  = vec4(1.0, 1.0, 1.0, 0.0);       // �g�U���ː����̋��x
const vec4 lspec  = vec4(1.0, 1.0, 1.0, 0.0);       // ���ʔ��ː����̋��x

// �ގ�
const vec4 kamb   = vec4(0.3, 0.1, 0.0, 1.0);       // �����̔��ˌW��
const vec4 kdiff  = vec4(0.3, 0.1, 0.0, 1.0);       // �g�U���ˌW��
const vec4 kspec  = vec4(0.6, 0.6, 0.6, 1.0);       // ���ʔ��ˌW��
const float kshi  = 80.0;                           // �P���W��

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec3 v;                                          // ��Ԃ��ꂽ�����x�N�g��
in vec3 l;                                          // ��Ԃ��ꂽ�����x�N�g��
in vec3 t;                                          // ��Ԃ��ꂽ�ڐ��x�N�g��

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main()
{
  vec3 nv = normalize(v);                           // �����x�N�g��
  vec3 nl = normalize(l);                           // �����x�N�g��
  vec3 nt = normalize(t);                           // �ڐ��x�N�g��
  
  // Kajiya-Kay ���f���ɂ��A�e�t��
  float lt = dot(nl, nt);
  float lt2 = sqrt(1.0 - lt * lt);
  float vt = dot(nv, nt);
  float vt2 = sqrt(1.0 - vt * vt);
  vec4 iamb = kamb * lamb;
  vec4 idiff = lt2 * kdiff * ldiff;
  vec4 ispec = pow(max(lt2 * vt2 - lt * vt, 0.0), kshi) * kspec * lspec;
  
  fc = iamb + idiff + ispec;
}
