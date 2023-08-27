#ifndef FLOATOPS_H
#define FLOATOPS_H

#include "commons.h"

float fop_degree2radians(float degrees);
float fop_clamp(float f, float min, float max);
void  fop_mat3x3_vec3_(const mat3x3 mat, const vec3 in, vec3 out);
void  fop_mat4x4_vec4_(const mat4x4 mat, const vec4 in, vec4 out);

void  fop_vec3_1sub2(const vec3 v1, const vec3 v2, vec3 out);
void  fop_vec3_cross(const vec3 v1, const vec3 v2, vec3 out);
float fop_vec3_dot(const vec3 v1, const vec3 v2);
void  fop_vec3_normalize(vec3 vec);

void  fop_2d_translate(vec2 translation, const vec3 in, vec3 out);
void  fop_2d_rotation(float rotation, vec2 pivot, const vec3 in, vec3 out);
void  fop_2d_scale(float scale, const vec3 in, vec3 out);

void  fop_3d_translate(vec3 translation, const vec3 in, vec3 out);
void  fop_3d_scale(float scale, const vec3 in, vec3 out);
void  fop_3d_roll_rotation(float rotation, vec3 pivot, const vec3 in, vec3 out);
void  fop_3d_pitch_rotation(float rotation, vec3 pivot, const vec3 in, vec3 out);
void  fop_3d_yaw_rotation(float rotation, vec3 pivot, const vec3 in, vec3 out);

#endif /* FLOATOPS_H */