#include "floatops.h"

float fop_degree2radians(float degrees)
{
	return degrees * (3.14159f / 180.0f);
}

float fop_clamp(float f, float min, float max) {
	return fminf(fmaxf(f, min), max);
}

void fop_mat3x3_vec3_(const mat3x3 mat, const vec3 in, vec3 out) {
	out[0] = (mat[0][0]) * in[0] + (mat[0][1]) * in[1] + (mat[0][2]) * in[2];
	out[1] = (mat[1][0]) * in[0] + (mat[1][1]) * in[1] + (mat[1][2]) * in[2];
	out[2] = (mat[2][0]) * in[0] + (mat[2][1]) * in[1] + (mat[2][2]) * in[2];
	return;
}
void fop_mat4x4_vec4_(const mat4x4 mat, const vec4 in, vec4 out) {
	out[0]  = (mat[0][0]) * in[0] + (mat[0][1]) * in[1] + (mat[0][2]) * in[2] + (mat[0][3]) * in[3];
	out[1]  = (mat[1][0]) * in[0] + (mat[1][1]) * in[1] + (mat[1][2]) * in[2] + (mat[1][3]) * in[3];
	out[2]  = (mat[2][0]) * in[0] + (mat[2][1]) * in[1] + (mat[2][2]) * in[2] + (mat[2][3]) * in[3];
	out[3]  = (mat[3][0]) * in[0] + (mat[3][1]) * in[1] + (mat[3][2]) * in[2] + (mat[2][3]) * in[3];
}

void fop_vec3_1sub2(const vec3 v1, const vec3 v2, vec3 out) {
	out[0] = v1[0] - v2[0];
	out[1] = v1[1] - v2[1];
	out[2] = v1[2] - v2[2];
}

void fop_vec3_cross(const vec3 v1, const vec3 v2, vec3 out) {
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

float fop_vec3_dot(const vec3 v1, const vec3 v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void fop_vec3_normalize(vec3 vec) {
	if (vec[0] != 0) { vec[0] = vec[0] / fabsf(vec[0]); }
	if (vec[1] != 0) { vec[1] = vec[1] / fabsf(vec[1]); }
	if (vec[2] != 0) { vec[2] = vec[2] / fabsf(vec[2]); }
}

void fop_2d_translate(vec2 translation, const vec3 in, vec3 out)
{
	mat3x3 tmat = {
		{1, 0, translation[0]},
		{0, 1, translation[1]},
		{0, 0, 1},
	};
	fop_mat3x3_vec3_(tmat, in, out);
	return;
}

void fop_2d_rotation(float rotation, vec2 pivot, const vec3 in, vec3 out)
{
	mat3x3 rmat = {
		{cosf(rotation), -sinf(rotation),  pivot[0] * (1 - cosf(rotation)) + pivot[1] * (sinf(rotation))},
		{sinf(rotation), cosf(rotation), pivot[1] * (1 - cosf(rotation)) - pivot[0] * (sinf(rotation))},
		{0, 0, 1},
	};
	fop_mat3x3_vec3_(rmat, in, out);
	return;
}

void fop_2d_scale(vec2 scale, const vec3 in, vec3 out)
{
	mat3x3 smat = {
		{scale[0], 0, 0},
		{0, scale[1], 0},
		{0, 0, 1},
	};
	fop_mat3x3_vec3_(smat, in, out);
	return;
}

void fop_3d_translate(vec3 translation, const vec3 in, vec3 out)
{
	mat4x4 tmat = {
		{1, 0, 0, translation[0]},
		{0, 1, 0, translation[1]},
		{0, 0, 1, translation[2]},
		{0, 0, 0, 1},
	};
	vec4 in4 = {in[0], in[1], in[2], 1};
	vec4 out4 = {0};
	fop_mat4x4_vec4_(tmat, in4, out4);
	out[0] = out4[0];
	out[1] = out4[1];
	out[2] = out4[2];
	return;
}

void fop_3d_scale(vec3 scale, const vec3 in, vec3 out)
{
	mat4x4 tmat = {
		{scale[0], 0, 0, 0},
		{0, scale[1], 0, 0},
		{0, 0, scale[2], 0},
		{0, 0, 0, 1},
	};
	vec4 in4 = {in[0], in[1], in[2], 1};
	vec4 out4 = {0};
	fop_mat4x4_vec4_(tmat, in4, out4);
	out[0] = out4[0];
	out[1] = out4[1];
	out[2] = out4[2];
	return;
}

void  fop_3d_roll_rotation(float rotation, vec3 pivot, const vec3 in, vec3 out)
{
	fop_3d_translate((vec3){-pivot[0], -pivot[1], -pivot[2]}, in, out);
	mat4x4 rmat = {
		{cosf(rotation), -sinf(rotation), 0, 0},
		{sinf(rotation), cosf(rotation), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	};
	vec4 in4 = {in[0], in[1], in[2], 1};
	vec4 out4 = {0};
	fop_mat4x4_vec4_(rmat, in4, out4);
	out[0] = out4[0];
	out[1] = out4[1];
	out[2] = out4[2];
	fop_3d_translate((vec3){pivot[0], pivot[1], pivot[2]}, in, out);
	return;
}

void  fop_3d_pitch_rotation(float rotation, vec3 pivot, const vec3 in, vec3 out)
{
	fop_3d_translate((vec3){-pivot[0], -pivot[1], -pivot[2]}, in, out);
	mat4x4 rmat = {
		{1, 0, 0, 0},
		{0, cosf(rotation), -sinf(rotation), 0},
		{0, sinf(rotation), cosf(rotation), 0},
		{0, 0, 0, 1},
	};
	vec4 in4 = {in[0], in[1], in[2], 1};
	vec4 out4 = {0};
	fop_mat4x4_vec4_(rmat, in4, out4);
	out[0] = out4[0];
	out[1] = out4[1];
	out[2] = out4[2];
	fop_3d_translate((vec3){pivot[0], pivot[1], pivot[2]}, in, out);
	return;
}

void  fop_3d_yaw_rotation(float rotation, vec3 pivot, const vec3 in, vec3 out)
{
	fop_3d_translate((vec3){-pivot[0], -pivot[1], -pivot[2]}, in, out);
	mat4x4 rmat = {
		{cosf(rotation), 0, sinf(rotation), 0},
		{0, 1, 0, 0},
		{-sinf(rotation), 0, cosf(rotation), 0},
		{0, 0, 0, 1},
	};
	vec4 in4 = {in[0], in[1], in[2], 1};
	vec4 out4 = {0};
	fop_mat4x4_vec4_(rmat, in4, out4);
	out[0] = out4[0];
	out[1] = out4[1];
	out[2] = out4[2];
	fop_3d_translate((vec3){pivot[0], pivot[1], pivot[2]}, in, out);
	return;
}