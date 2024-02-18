#include "commons.h"
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

mesh rdr_clone_mesh(const mesh* src)
{
	mesh clone = rdr_init_mesh(src->n_vertices, src->n_indices);
	rdr_copy_mesh(&clone, src);
	return clone;
}

void rdr_copy_mesh(mesh* dst, const mesh* src)
{
	memcpy(dst->vertices, src->vertices, sizeof(ndc_vertex) * dst->n_vertices);
	memcpy(dst->indices, src->indices, sizeof(u16) * dst->n_indices);
}

bool rdr_comp_point_(const ndc_vertex* left, const ndc_vertex* right)
{
	return (left->xyz[0] == right->xyz[0]) && (left->xyz[1] == right->xyz[1]) && (left->xyz[2] == right->xyz[2]);
}

i16 rdr_filter_triangle_(const ndc_vertex* pa_ndc, const ndc_vertex* pb_ndc, const ndc_vertex* pc_ndc)
{
	// -1 Skip triangle
	//  0 Keep triangle as is
	if (rdr_comp_point_(pa_ndc, pb_ndc) ||
		rdr_comp_point_(pb_ndc, pc_ndc) ||
		rdr_comp_point_(pc_ndc, pa_ndc)) {
		return -1;
	}

	bool pa_outside = (pa_ndc->xyz[2] < -1.f || pa_ndc->xyz[2] > 1.f);
	bool pb_outside = (pb_ndc->xyz[2] < -1.f || pb_ndc->xyz[2] > 1.f);
	bool pc_outside = (pc_ndc->xyz[2] < -1.f || pc_ndc->xyz[2] > 1.f);

	u16 n_outside = pa_outside + pb_outside + pc_outside;

	bool outside_left   = (pa_ndc->xyz[0] < -1.f && pb_ndc->xyz[0] < -1.f && pc_ndc->xyz[0] < -1.f);
	bool outside_right  = (pa_ndc->xyz[0] >  1.f && pb_ndc->xyz[0] >  1.f && pc_ndc->xyz[0] >  1.f);
	bool outside_top    = (pa_ndc->xyz[1] < -1.f && pb_ndc->xyz[1] < -1.f && pc_ndc->xyz[1] < -1.f);
	bool outside_bottom = (pa_ndc->xyz[1] >  1.f && pb_ndc->xyz[1] >  1.f && pc_ndc->xyz[1] >  1.f);

	if (n_outside == 0) {
		return 0;
	}
	if (n_outside == 1 || n_outside == 2) {
		return -1;
	}
	if ((n_outside == 3) && (outside_left ^ outside_right ^ outside_top ^ outside_bottom)) {
		return -1;
	}
	return -1;
}

void rdr_render_mesh(const mesh* ws_mesh, const camera* cam, vec3 forward)
{
	mesh ndc_mesh = rdr_clone_mesh(ws_mesh);
	u16* indices = ndc_mesh.indices;
	ndc_vertex* vertices = ndc_mesh.vertices;

	mat4x4 projection = {0};
	fop_mat4_projection(projection, 0.1f, 100.f, cam->fov, 4.f / 3.f);

	mat4x4 view = {0};
	fop_mat4_camera(view, cam->pitch, cam->yaw, cam->xyz);

	// Per vertex operations
	for (i16 k = 0; k < ndc_mesh.n_vertices; k++) {
		rdr_viewfrom_(
			ndc_mesh.vertices[k].xyz,
			view,
			ndc_mesh.vertices[k].xyz);
		rdr_project_(
			ndc_mesh.vertices[k].xyz,
			projection,
			ndc_mesh.vertices[k].xyz);
		float z = ndc_mesh.vertices[k].xyz[2];
	}

	// Per triangle operations
	for (i16 k = 0; k < ndc_mesh.n_indices; k += 3) {
		bool is_cullable = rdr_is_cullable_(forward,
			ws_mesh->vertices[ws_mesh->indices[k + 0]].xyz,
			ws_mesh->vertices[ws_mesh->indices[k + 1]].xyz,
			ws_mesh->vertices[ws_mesh->indices[k + 2]].xyz);

		i16 filter_status = rdr_filter_triangle_(
			&ndc_mesh.vertices[indices[k + 0]],
			&ndc_mesh.vertices[indices[k + 1]],
			&ndc_mesh.vertices[indices[k + 2]]);

		if (!is_cullable)
		{
			if (filter_status == 0)
			{
				rtz_draw_triangle(
					&ndc_mesh.vertices[indices[k + 0]],
					&ndc_mesh.vertices[indices[k + 1]],
					&ndc_mesh.vertices[indices[k + 2]], WIREFRAME);
			}
		}
	}
	rdr_free_mesh(ndc_mesh);
}

void rdr_free_mesh(mesh mesh)
{
	free(mesh.vertices);
	free(mesh.indices);
}