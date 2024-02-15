#ifndef RENDERER_H
#define RENDERER_H

#include "commons.h"
#include "rasterizer.h"

typedef struct mesh{
	u16 n_vertices;
	u16 n_indices;
	ndc_vertex* vertices;
	u16* indices;
}mesh;

typedef struct camera{
	vec3 xyz;
	float fov;
	float yaw;
	float pitch;
}camera;

extern bool WIREFRAME;

void rdr_toggle_wireframe();
void rdr_mesh_centroid(vec3 out, const mesh* mesh);

void rdr_project_(vec3 out, const mat4x4 mat, const vec3 vec);
void rdr_viewfrom_(vec3 out, const mat4x4 mat, const vec3 vec);

bool rdr_is_cullable_(const vec3 forward, const vec3 va, const vec3 vb, const vec3 vc);
mesh rdr_init_mesh(u16 n_vertices, u16 n_indices);
mesh rdr_clone_mesh(mesh* mesh);
void rdr_copy_mesh(mesh* dst, mesh* src);
void rdr_free_mesh(mesh mesh);

void rdr_render_mesh(mesh* mesh, const camera* cam, vec3 forward);

#endif /* RENDERER_H */