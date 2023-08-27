#ifndef RENDERER_H
#define RENDERER_H

#include "commons.h"
#include "rasterizer.h"

typedef struct mesh_ndc{
	u16 n_vertices;
	u16 n_indices;
	vertex_ndc* vertices;
	u16* indices;
}mesh_ndc;

mesh_ndc rdr_init_mesh(u16 n_vertices, u16 n_indices);
mesh_ndc rdr_copy_mesh(mesh_ndc* mesh);
void rdr_render_mesh(mesh_ndc* mesh);
void rdr_free_mesh(mesh_ndc mesh);
float rdr_degree2radians(float degrees);

#endif /* RENDERER_H */