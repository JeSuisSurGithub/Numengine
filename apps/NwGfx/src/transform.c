#include "transform.h"

void tsf_mat3x3_vec3_(const mat3x3 mat, const vec3 in, vec3 out) {
	out[0] = (mat[0][0]) * in[0] + (mat[0][1]) * in[1] + (mat[0][2]) * in[2];
	out[1] = (mat[1][0]) * in[0] + (mat[1][1]) * in[1] + (mat[1][2]) * in[2];
	out[2] = (mat[2][0]) * in[0] + (mat[2][1]) * in[1] + (mat[2][2]) * in[2];
	return;
}

void tsf_translate(vec2 translation, const vec3 in, vec3 out)
{
	mat3x3 tmat = {
		{1, 0, translation[0]},
		{0, 1, translation[1]},
		{0, 0, 1},
	};
	tsf_mat3x3_vec3_(tmat, in, out);
	return;
}

void tsf_rotation(float rotation, vec2 pivot, const vec3 in, vec3 out)
{
	mat3x3 rmat = {
		{cos(rotation), -sin(rotation),  pivot[0] * (1 - cos(rotation)) + pivot[1] * (sin(rotation))},
		{sin(rotation), cos(rotation), pivot[1] * (1 - cos(rotation)) - pivot[0] * (sin(rotation))},
		{0, 0, 1},
	};
	tsf_mat3x3_vec3_(rmat, in, out);
	return;
}

void tsf_scale(float scale, const vec3 in, vec3 out)
{
	mat3x3 smat = {
		{scale, 0, 0},
		{0, scale, 0},
		{0, 0, 1},
	};
	tsf_mat3x3_vec3_(smat, in, out);
	return;
}

void tsf_center(const mesh_ndc* mesh, vec2 out)
{
	out = (vec2){0.f, 0.f};
	for (i16 k = 0; k < mesh->n_vertices; k++) {
		out[0] += mesh->vertices[k].xyz[0];
		out[1] += mesh->vertices[k].xyz[1];
	}
	out[0] = out[0] / mesh->n_vertices;
	out[1] = out[1] / mesh->n_vertices;
	return;
}