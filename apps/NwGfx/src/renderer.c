#include "renderer.h"
#include "rasterizer.h"

mesh_ndc rdr_init_mesh(u16 n_vertices, u16 n_indices)
{
	mesh_ndc mesh;
	mesh.n_vertices = n_vertices;
	mesh.n_indices = n_indices;
	mesh.vertices = malloc(sizeof(vertex_ndc) * n_vertices);
	mesh.indices = malloc(sizeof(u16) * n_indices);
	return mesh;
}

mesh_ndc rdr_copy_mesh(mesh_ndc* mesh)
{
	mesh_ndc copy = rdr_init_mesh(mesh->n_vertices, mesh->n_indices);
	memcpy(copy.vertices, mesh->vertices, sizeof(vertex_ndc) * mesh->n_vertices);
	memcpy(copy.indices, mesh->indices, sizeof(u16) * mesh->n_indices);
	return copy;
}

void rdr_render_mesh(mesh_ndc* mesh)
{
	for (i16 k = 0; k < mesh->n_indices; k += 3) {
		rtz_draw_triangle(
			&mesh->vertices[mesh->indices[k + 0]],
			&mesh->vertices[mesh->indices[k + 1]],
			&mesh->vertices[mesh->indices[k + 2]], false);
	}
}

void rdr_free_mesh(mesh_ndc mesh)
{
	free(mesh.vertices);
	free(mesh.indices);
}

float rdr_degree2radians(float degrees)
{
	return degrees * (3.14159f / 180.0f);
}