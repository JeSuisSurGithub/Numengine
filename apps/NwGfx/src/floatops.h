#ifndef FLOATOPS_H
#define FLOATOPS_H

#include "commons.h"

float fop_deg2rad(float degree);
float fop_clamp(float flt, float min, float max);

void fop_mat4x4_mul_vec4(vec4 out, const mat4x4 mat, const vec4 vec);
void fop_mat4x4_mul_vec3(vec3 out, const mat4x4 mat, const vec3 vec);
void fop_mat4x4_1mul2(mat4x4 out, const mat4x4 m1, const mat4x4 m2);

void fop_vec3_mul_flt(vec3 out, const vec3 vec, const float flt);
void fop_vec3_1add2(vec3 out, const vec3 v1, const vec3 v2);
void fop_vec3_1sub2(vec3 out, const vec3 v1, const vec3 v2);
void fop_vec3_cross(vec3 out, const vec3 v1, const vec3 v2);
void fop_vec3_normalize(vec3 out, const vec3 vec);

float fop_vec3_dot(const vec3 v1, const vec3 v2);
float fop_vec3_len(const vec3 vec);

void fop_vec3_forward(vec3 out, const float pitch, const float yaw);
void fop_vec3_right(vec3 out, const float yaw);

void fop_mat4_identity(mat4x4 out);
void fop_mat4_roll(mat4x4 out, const float rotation);
void fop_mat4_pitch(mat4x4 out, const float rotation);
void fop_mat4_yaw(mat4x4 out, const float rotation);
void fop_mat4_scale(mat4x4 out, const vec3 scale);
void fop_mat4_translate(mat4x4 out, const vec3 translation);

void fop_mat4_projection(mat4x4 out, const float znear, const float zfar, const float fov_degree, const float ratio);
void fop_mat4_camera(mat4x4 out, const float pitch, const float yaw, const vec3 view_pos);

#endif /* FLOATOPS_H */