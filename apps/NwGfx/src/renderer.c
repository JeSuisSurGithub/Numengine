#include "renderer.h"
#include "floatops.h"

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
	float fov = (1.f / tanf(fov_degree * 0.5f / 180.f * PI));
	memcpy(out, (mat4x4){
		{ratio / fov, 0.f, 0.f, 0.f},
		{0.f, fov, 0.f, 0.f},
		{0.f, 0.f, zfar / (zfar - znear), 1.f},
		{0.f, 0.f, (-zfar * znear) / (zfar - znear), 0.f},
	}, sizeof(float) * 4 * 4);
	return;
}

/* -------------------- WIP -------------------- */
void rdr_viewfrom_(const camera* cam, const vec3 in, vec3 out)
{
	fop_3d_yaw_rotation(cam->yaw, cam->xyz, in, out);
	fop_3d_pitch_rotation(cam->pitch, cam->xyz, in, out);
	vec3 translation = {-cam->xyz[0], -cam->xyz[1], -cam->xyz[2]};
	fop_3d_translate(translation, in, out);
	return;
}
/* -------------------- WIP -------------------- */

bool rdr_is_cullable_(const vec3 forward, const vec3 va, const vec3 vb, const vec3 vc)
{
	vec3 v_ab = {0};
	vec3 v_ac = {0};
	vec3 v_normal = {0};

	fop_vec3_1sub2(vb, va, v_ab);
	fop_vec3_1sub2(vc, va, v_ac);
	fop_vec3_cross(v_ab, v_ac, v_normal);
	fop_vec3_normalize(v_normal);

	return (fop_vec3_dot(v_normal, forward) <= 0.f);
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

void rdr_render_mesh(mesh_ndc* mesh, const camera* cam)
{
	float fov = 90.f;
	mat4x4 projection = {0};
	rdr_projection_mat_(projection, 0.1f, 1000.f, fov, 4.f / 3.f);

	bool already_transformed[256] = {0};

	for (i16 k = 0; k < mesh->n_indices; k += 3) {
		vec3 forward = {0.f, 0.f, -1.f};
		fop_vec3_normalize(forward);
		if (!rdr_is_cullable_(forward,
			mesh->vertices[mesh->indices[k + 0]].xyz,
			mesh->vertices[mesh->indices[k + 1]].xyz,
			mesh->vertices[mesh->indices[k + 2]].xyz))
		{
			if (!already_transformed[mesh->indices[k + 0]]) {
				// rdr_viewfrom_(cam,
				// 	mesh->vertices[mesh->indices[k + 0]].xyz,
				// 	mesh->vertices[mesh->indices[k + 0]].xyz);
				rdr_project_(projection,
					mesh->vertices[mesh->indices[k + 0]].xyz,
					mesh->vertices[mesh->indices[k + 0]].xyz);
				already_transformed[mesh->indices[k + 0]] = true;
			}
			if (!already_transformed[mesh->indices[k + 1]]) {
				// rdr_viewfrom_(cam,
				// 	mesh->vertices[mesh->indices[k + 1]].xyz,
				// 	mesh->vertices[mesh->indices[k + 1]].xyz);
				rdr_project_(projection,
					mesh->vertices[mesh->indices[k + 1]].xyz,
					mesh->vertices[mesh->indices[k + 1]].xyz);
				already_transformed[mesh->indices[k + 1]] = true;
			}
			if (!already_transformed[mesh->indices[k + 2]]) {
				// rdr_viewfrom_(cam,
				// 	mesh->vertices[mesh->indices[k + 2]].xyz,
				// 	mesh->vertices[mesh->indices[k + 2]].xyz);
				rdr_project_(projection,
					mesh->vertices[mesh->indices[k + 2]].xyz,
					mesh->vertices[mesh->indices[k + 2]].xyz);
				already_transformed[mesh->indices[k + 2]] = true;
			}

			rtz_draw_triangle(
				&mesh->vertices[mesh->indices[k + 0]],
				&mesh->vertices[mesh->indices[k + 1]],
				&mesh->vertices[mesh->indices[k + 2]], false);
		}
	}
}

void rdr_free_mesh(mesh_ndc mesh)
{
	free(mesh.vertices);
	free(mesh.indices);
}