#ifndef RENDERER_H
#define RENDERER_H

#include "commons.h"
#include "rasterizer.h"

typedef struct mesh_ndc{
	u16 n_vertices;
	i16 n_indices;
	vertex_ndc* vertices;
	u16* indices;
}mesh_ndc;

void rdr_project_(const mat4x4 mat, const vec3 in, vec3 out);
void rdr_projection_mat_(mat4x4 out, float znear, float zfar, float fov_degree, float ratio);
bool rdr_is_cullable_(const vec3 camera, const vec3 va, const vec3 vb, const vec3 vc);
mesh_ndc rdr_init_mesh(u16 n_vertices, u16 n_indices);
void rdr_copy_mesh(mesh_ndc* dst, mesh_ndc* src);
void rdr_render_mesh(mesh_ndc* mesh);
void rdr_free_mesh(mesh_ndc mesh);

#endif /* RENDERER_H */