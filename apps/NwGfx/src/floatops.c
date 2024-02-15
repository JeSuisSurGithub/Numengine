#include "floatops.h"
#include "commons.h"

float fop_deg2rad(float degree)
{
	return degree * (PI / 180.0f);
}
float fop_clamp(float flt, float min, float max) {
	return fminf(fmaxf(flt, min), max);
}

void fop_mat4x4_mul_vec4(vec4 out, const mat4x4 mat, const vec4 vec) {
	vec4 temp = {0};
	temp[0]  = (mat[0][0]) * vec[0] + (mat[0][1]) * vec[1] + (mat[0][2]) * vec[2] + (mat[0][3]) * vec[3];
	temp[1]  = (mat[1][0]) * vec[0] + (mat[1][1]) * vec[1] + (mat[1][2]) * vec[2] + (mat[1][3]) * vec[3];
	temp[2]  = (mat[2][0]) * vec[0] + (mat[2][1]) * vec[1] + (mat[2][2]) * vec[2] + (mat[2][3]) * vec[3];
	temp[3]  = (mat[3][0]) * vec[0] + (mat[3][1]) * vec[1] + (mat[3][2]) * vec[2] + (mat[3][3]) * vec[3];
	memcpy(out, temp, sizeof(vec4));
	return;
}
void fop_mat4x4_mul_vec3(vec3 out, const mat4x4 mat, const vec3 vec) {
	vec3 temp = {0};
	temp[0]  = (mat[0][0]) * vec[0] + (mat[0][1]) * vec[1] + (mat[0][2]) * vec[2] + (mat[0][3]) * 1.f;
	temp[1]  = (mat[1][0]) * vec[0] + (mat[1][1]) * vec[1] + (mat[1][2]) * vec[2] + (mat[1][3]) * 1.f;
	temp[2]  = (mat[2][0]) * vec[0] + (mat[2][1]) * vec[1] + (mat[2][2]) * vec[2] + (mat[2][3]) * 1.f;
	memcpy(out, temp, sizeof(vec3));
	return;
}
void fop_mat4x4_1mul2(mat4x4 out, const mat4x4 m1, const mat4x4 m2)
{
	mat4x4 temp = {0};
    temp[0][0] = m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0] + m1[0][2] * m2[2][0] + m1[0][3] * m2[3][0];
    temp[0][1] = m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1] + m1[0][2] * m2[2][1] + m1[0][3] * m2[3][1];
    temp[0][2] = m1[0][0] * m2[0][2] + m1[0][1] * m2[1][2] + m1[0][2] * m2[2][2] + m1[0][3] * m2[3][2];
    temp[0][3] = m1[0][0] * m2[0][3] + m1[0][1] * m2[1][3] + m1[0][2] * m2[2][3] + m1[0][3] * m2[3][3];

    temp[1][0] = m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0] + m1[1][2] * m2[2][0] + m1[1][3] * m2[3][0];
    temp[1][1] = m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1] + m1[1][2] * m2[2][1] + m1[1][3] * m2[3][1];
    temp[1][2] = m1[1][0] * m2[0][2] + m1[1][1] * m2[1][2] + m1[1][2] * m2[2][2] + m1[1][3] * m2[3][2];
    temp[1][3] = m1[1][0] * m2[0][3] + m1[1][1] * m2[1][3] + m1[1][2] * m2[2][3] + m1[1][3] * m2[3][3];

    temp[2][0] = m1[2][0] * m2[0][0] + m1[2][1] * m2[1][0] + m1[2][2] * m2[2][0] + m1[2][3] * m2[3][0];
    temp[2][1] = m1[2][0] * m2[0][1] + m1[2][1] * m2[1][1] + m1[2][2] * m2[2][1] + m1[2][3] * m2[3][1];
    temp[2][2] = m1[2][0] * m2[0][2] + m1[2][1] * m2[1][2] + m1[2][2] * m2[2][2] + m1[2][3] * m2[3][2];
    temp[2][3] = m1[2][0] * m2[0][3] + m1[2][1] * m2[1][3] + m1[2][2] * m2[2][3] + m1[2][3] * m2[3][3];

    temp[3][0] = m1[3][0] * m2[0][0] + m1[3][1] * m2[1][0] + m1[3][2] * m2[2][0] + m1[3][3] * m2[3][0];
    temp[3][1] = m1[3][0] * m2[0][1] + m1[3][1] * m2[1][1] + m1[3][2] * m2[2][1] + m1[3][3] * m2[3][1];
    temp[3][2] = m1[3][0] * m2[0][2] + m1[3][1] * m2[1][2] + m1[3][2] * m2[2][2] + m1[3][3] * m2[3][2];
    temp[3][3] = m1[3][0] * m2[0][3] + m1[3][1] * m2[1][3] + m1[3][2] * m2[2][3] + m1[3][3] * m2[3][3];
	memcpy(out, temp, sizeof(mat4x4));
	return;
}

void fop_vec3_mul_flt(vec3 out, const vec3 vec, const float flt) {
	out[0] = vec[0] * flt;
	out[1] = vec[1] * flt;
	out[2] = vec[2] * flt;
	return;
}
void fop_vec3_1add2(vec3 out, const vec3 v1, const vec3 v2) {
	out[0] = v1[0] + v2[0];
	out[1] = v1[1] + v2[1];
	out[2] = v1[2] + v2[2];
	return;
}
void fop_vec3_1sub2(vec3 out, const vec3 v1, const vec3 v2) {
	out[0] = v1[0] - v2[0];
	out[1] = v1[1] - v2[1];
	out[2] = v1[2] - v2[2];
	return;
}
void fop_vec3_cross(vec3 out, const vec3 v1, const vec3 v2) {
	vec3 temp = {0};
	temp[0] = v1[1] * v2[2] - v1[2] * v2[1];
	temp[1] = v1[2] * v2[0] - v1[0] * v2[2];
	temp[2] = v1[0] * v2[1] - v1[1] * v2[0];
	memcpy(out, temp, sizeof(vec3));
	return;
}
void fop_vec3_normalize(vec3 out, const vec3 vec) {
	float length = fop_vec3_len(vec);
	if (length != 0.f) {
		out[0] = vec[0] / length;
		out[1] = vec[1] / length;
		out[2] = vec[2] / length;
	}
	return;
}

float fop_vec3_dot(const vec3 v1, const vec3 v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
float fop_vec3_len(const vec3 vec) {
	return sqrtf(powf(vec[0], 2) + powf(vec[1], 2) + powf(vec[2], 2));
}

void fop_vec3_forward(vec3 out, const float pitch, const float yaw)
{
    out[0] = cosf(pitch) * sinf(yaw);
    out[1] = sinf(pitch);
    out[2] = cosf(pitch) * cosf(yaw);
}
void fop_vec3_right(vec3 out, const float yaw)
{
	out[0] = sinf(yaw - (PI / 2));
	out[1] = 0.f;
	out[2] = cosf(yaw - (PI / 2));
}

void fop_mat4_identity(mat4x4 out)
{
	memcpy(out, (mat4x4){
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}, sizeof(mat4x4));
	return;
}
void fop_mat4_roll(mat4x4 out, const float theta)
{
	memcpy(out, (mat4x4){
		{cosf(theta), -sinf(theta), 0, 0},
		{sinf(theta), cosf(theta), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}, sizeof(mat4x4));
	return;
}
void fop_mat4_pitch(mat4x4 out, const float theta)
{
	memcpy(out, (mat4x4){
		{1, 0, 0, 0},
		{0, cosf(theta), -sinf(theta), 0},
		{0, sinf(theta), cosf(theta), 0},
		{0, 0, 0, 1},
	}, sizeof(mat4x4));
	return;
}
void fop_mat4_yaw(mat4x4 out, const float theta)
{
	memcpy(out, (mat4x4){
		{cosf(theta), 0, sinf(theta), 0},
		{0, 1, 0, 0},
		{-sinf(theta), 0, cosf(theta), 0},
		{0, 0, 0, 1},
	}, sizeof(mat4x4));
	return;
}
void fop_mat4_scale(mat4x4 out, const vec3 scale)
{
	memcpy(out, (mat4x4){
		{scale[0], 0, 0, 0},
		{0, scale[1], 0, 0},
		{0, 0, scale[2], 0},
		{0, 0, 0, 1},
	}, sizeof(mat4x4));
	return;
}
void fop_mat4_translate(mat4x4 out, const vec3 translation)
{
	memcpy(out, (mat4x4){
		{1, 0, 0, translation[0]},
		{0, 1, 0, translation[1]},
		{0, 0, 1, translation[2]},
		{0, 0, 0, 1},
	}, sizeof(mat4x4));
	return;
}

void fop_mat4_projection(mat4x4 out, const float znear, const float zfar, const float fov_degree, const float ratio)
{
	float fov = (1.f / tanf(fop_deg2rad(fov_degree) * 0.5f));
	memcpy(out, (mat4x4){
		{fov / ratio, 0.f, 0.f, 0.f},
		{0.f, fov, 0.f, 0.f},
		{0.f, 0.f, (zfar + znear) / (zfar - znear), 1.f},
		{0.f, 0.f, (-zfar * znear) / (zfar - znear), 0.f},
	}, sizeof(mat4x4));
	return;
}
void fop_mat4_camera(mat4x4 out, const float pitch, const float yaw, const vec3 view_pos)
{
	vec3 forward = {0};
	vec3 right = {0};
	fop_vec3_forward(forward, pitch, yaw);
	fop_vec3_right(right, yaw);

	fop_vec3_normalize(forward, forward);
	fop_vec3_normalize(right, right);

	vec3 up = {0};
	fop_vec3_cross(up, right, forward);
	fop_vec3_normalize(up, up);

	memcpy(out, (mat4x4){
		{right[0], right[1], right[2], -fop_vec3_dot(right, view_pos)},
		{up[0], up[1], up[2], -fop_vec3_dot(up, view_pos)},
		{forward[0], forward[1], forward[2], -fop_vec3_dot(forward, view_pos)},
		{0.f, 0.f, 0.f, 1.f}
	}, sizeof(mat4x4));
	return;
}