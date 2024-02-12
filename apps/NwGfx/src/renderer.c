#include "commons.h"
#include "floatops.h"
#include "renderer.h"

bool WIREFRAME = false;

void rdr_toggle_wireframe() {
	WIREFRAME = (WIREFRAME == true) ? false : true;
}

void rdr_project_(const mat4x4 mat, const vec3 in, vec3 out)
{
	vec4 in4 = {in[0], in[1], in[2], 1.f};
	vec4 out4 = {0};
	fop_mat4x4_vec4(mat, in4, out4);
	if (out4[3] != 0)	{
		out[0] = out4[0] / out4[3];
		out[1] = out4[1] / out4[3];
		out[2] = out4[2] / out4[3];
	}
	return;
}

void rdr_projection_mat_(mat4x4 out, float znear, float zfar, float fov_degree, float ratio)
{
	float fov = (1.f / tanf(fop_deg2rad(fov_degree) * 0.5f));
	memcpy(out, (mat4x4){
		{fov / ratio, 0.f, 0.f, 0.f},
		{0.f, fov, 0.f, 0.f},
		{0.f, 0.f, (zfar + znear) / (zfar - znear), 1.f},
		{0.f, 0.f, (-zfar * znear) / (zfar - znear), 0.f},
	}, sizeof(float) * 4 * 4);
	return;
}

void rdr_viewfrom_(const mat4x4 mat, const vec3 in, vec3 out)
{
	vec4 in4 = {in[0], in[1], in[2], 1.f};
	vec4 out4 = {0};
	fop_mat4x4_vec4(mat, in4, out4);
	out[0] = out4[0];
	out[1] = out4[1];
	out[2] = out4[2];
	return;
}

void rdr_camera_mat_(const camera* cam, mat4x4 out) {
	vec3 forward = {0};
	vec3 right = {0};
	fop_forward(cam->pitch, cam->yaw, forward);
	fop_right(cam->yaw, right);

	fop_vec3_normalize(forward);
	fop_vec3_normalize(right);

	vec3 up = {0};
	fop_vec3_cross(right, forward, up);
	fop_vec3_normalize(up);

	memcpy(out, (mat4x4){
		{right[0], right[1], right[2], -fop_vec3_dot(right, cam->xyz)},
		{up[0], up[1], up[2], -fop_vec3_dot(up, cam->xyz)},
		{forward[0], forward[1], forward[2], -fop_vec3_dot(forward, cam->xyz)},
		{0.f, 0.f, 0.f, 1.f}
	}, sizeof(float) * 4 * 4);
	return;
}

bool rdr_is_cullable_(const vec3 forward, const vec3 va, const vec3 vb, const vec3 vc)
{
	vec3 v_ab = {0};
	vec3 v_ac = {0};
	vec3 v_normal = {0};

	fop_vec3_1sub2(vb, va, v_ab);
	fop_vec3_1sub2(vc, va, v_ac);
	fop_vec3_cross(v_ab, v_ac, v_normal);
	fop_vec3_normalize(v_normal);

	return (fop_vec3_dot(forward, v_normal) < 0.f);
}

mesh_ndc rdr_init_mesh(u16 n_vertices, u16 n_indices)
{
	mesh_ndc mesh;
	mesh.n_vertices = n_vertices;
	mesh.n_indices = n_indices;
	mesh.vertices = malloc(sizeof(vertex_ndc) * n_vertices);
	mesh.indices = malloc(sizeof(u16) * n_indices);
	return mesh;
}

mesh_ndc rdr_clone_mesh(mesh_ndc* mesh)
{
	mesh_ndc clone = rdr_init_mesh(mesh->n_vertices, mesh->n_indices);
	rdr_copy_mesh(&clone, mesh);
	return clone;
}

void rdr_copy_mesh(mesh_ndc* dst, mesh_ndc* src)
{
	memcpy(dst->vertices, src->vertices, sizeof(vertex_ndc) * dst->n_vertices);
	memcpy(dst->indices, src->indices, sizeof(u16) * dst->n_indices);
}

void rdr_render_mesh(mesh_ndc* mesh, const camera* cam, vec3 forward)
{
	mat4x4 projection = {0};
	rdr_projection_mat_(projection, .1f, 1e+6f, cam->fov, 4.f / 3.f);

	mat4x4 view = {0};
	rdr_camera_mat_(cam, view);

	u8 already_projected[256] = {0};

	fop_vec3_normalize(forward);
	mesh_ndc projected_mesh = rdr_clone_mesh(mesh);

	// Alias
	u16* indices = mesh->indices;
	vertex_ndc* vertices = mesh->vertices;

	for (i16 k = 0; k < mesh->n_indices; k += 3) {
		if (!rdr_is_cullable_(forward,
			vertices[indices[k + 0]].xyz,
			vertices[indices[k + 1]].xyz,
			vertices[indices[k + 2]].xyz))
		{
			for (u64 i = 0; i < 3; i++) {
				if (!already_projected[indices[k + i]]) {
					rdr_viewfrom_(view,
						projected_mesh.vertices[indices[k + i]].xyz,
						projected_mesh.vertices[indices[k + i]].xyz);
					rdr_project_(projection,
						projected_mesh.vertices[indices[k + i]].xyz,
						projected_mesh.vertices[indices[k + i]].xyz);
					already_projected[indices[k + i]] = true;
				}
			}
			rtz_draw_triangle(
				&projected_mesh.vertices[indices[k + 0]],
				&projected_mesh.vertices[indices[k + 1]],
				&projected_mesh.vertices[indices[k + 2]], WIREFRAME);
		}
	}
	rdr_free_mesh(projected_mesh);
}

void rdr_free_mesh(mesh_ndc mesh)
{
	free(mesh.vertices);
	free(mesh.indices);
}