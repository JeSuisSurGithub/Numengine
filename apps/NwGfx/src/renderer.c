#include "floatops.h"
#include "renderer.h"

bool WIREFRAME = false;

void rdr_toggle_wireframe() {
	WIREFRAME = !WIREFRAME;
}

void rdr_mesh_centroid(vec3 out, const mesh* mesh)
{
	out = (vec3){0.f, 0.f, 0.f};
	for (i16 k = 0; k < mesh->n_vertices; k++) {
		out[0] += mesh->vertices[k].xyz[0];
		out[1] += mesh->vertices[k].xyz[1];
		out[2] += mesh->vertices[k].xyz[2];
	}
	out[0] = out[0] / mesh->n_vertices;
	out[1] = out[1] / mesh->n_vertices;
	out[2] = out[2] / mesh->n_vertices;
	return;
}

void rdr_project_(vec3 out, const mat4x4 mat, const vec3 vec)
{
	vec4 in4 = {vec[0], vec[1], vec[2], 1.f};
	vec4 out4 = {0};
	fop_mat4x4_mul_vec4(out4, mat, in4);
	if (out4[3] != 0)	{
		out[0] = out4[0] / out4[3];
		out[1] = out4[1] / out4[3];
		out[2] = out4[2] / out4[3];
	}
	return;
}

void rdr_viewfrom_(vec3 out, const mat4x4 mat, const vec3 vec)
{
	fop_mat4x4_mul_vec3(out, mat, vec);
	return;
}

bool rdr_is_cullable_(const vec3 forward, const vec3 va, const vec3 vb, const vec3 vc)
{
	vec3 v_ab = {0};
	vec3 v_ac = {0};
	vec3 v_normal = {0};

	fop_vec3_1sub2(v_ab, vb, va);
	fop_vec3_1sub2(v_ac, vc, va);
	fop_vec3_cross(v_normal, v_ab, v_ac);
	fop_vec3_normalize(v_normal, v_normal);

	return (fop_vec3_dot(forward, v_normal) < 0.f);
}

mesh rdr_init_mesh(u16 n_vertices, u16 n_indices)
{
	mesh mesh;
	mesh.n_vertices = n_vertices;
	mesh.n_indices = n_indices;
	mesh.vertices = malloc(sizeof(ndc_vertex) * n_vertices);
	mesh.indices = malloc(sizeof(u16) * n_indices);
	return mesh;
}

mesh rdr_clone_mesh(mesh* src)
{
	mesh clone = rdr_init_mesh(src->n_vertices, src->n_indices);
	rdr_copy_mesh(&clone, src);
	return clone;
}

void rdr_copy_mesh(mesh* dst, mesh* src)
{
	memcpy(dst->vertices, src->vertices, sizeof(ndc_vertex) * dst->n_vertices);
	memcpy(dst->indices, src->indices, sizeof(u16) * dst->n_indices);
}

void rdr_render_mesh(mesh* ws_mesh, const camera* cam, vec3 forward)
{
	mat4x4 projection = {0};
	fop_mat4_projection(projection, .1f, 1e+6f, cam->fov, 4.f / 3.f);

	mat4x4 view = {0};
	fop_mat4_camera(view, cam->pitch, cam->yaw, cam->xyz);

	u8 is_clip_space[256] = {0};

	mesh cs_mesh = rdr_clone_mesh(ws_mesh);

	// Alias
	u16* indices = ws_mesh->indices;
	ndc_vertex* vertices = ws_mesh->vertices;

	for (i16 k = 0; k < ws_mesh->n_indices; k += 3) {
		if (!rdr_is_cullable_(forward,
			vertices[indices[k + 0]].xyz,
			vertices[indices[k + 1]].xyz,
			vertices[indices[k + 2]].xyz))
		{
			for (u64 i = 0; i < 3; i++) {
				if (!is_clip_space[indices[k + i]]) {
					rdr_viewfrom_(
						cs_mesh.vertices[indices[k + i]].xyz,
						view,
						cs_mesh.vertices[indices[k + i]].xyz);
					rdr_project_(
						cs_mesh.vertices[indices[k + i]].xyz,
						projection,
						cs_mesh.vertices[indices[k + i]].xyz);
					is_clip_space[indices[k + i]] = true;
				}
			}
			rtz_draw_triangle(
				&cs_mesh.vertices[indices[k + 0]],
				&cs_mesh.vertices[indices[k + 1]],
				&cs_mesh.vertices[indices[k + 2]], WIREFRAME);
		}
	}
	rdr_free_mesh(cs_mesh);
}

void rdr_free_mesh(mesh mesh)
{
	free(mesh.vertices);
	free(mesh.indices);
}