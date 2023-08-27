#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "commons.h"
#include "renderer.h"

void tsf_mat3x3_vec3_(const mat3x3 mat, const vec3 in, vec3 out);
void tsf_translate(vec2 translation, const vec3 in, vec3 out);
void tsf_rotation(float rotation, vec2 pivot, const vec3 in, vec3 out);
void tsf_scale(float scale, const vec3 in, vec3 out);
void tsf_center(const mesh_ndc* mesh, vec2 out);

#endif /* TRANSFORM_H */