#include "commons.h"
#include "controls.h"
#include "renderer.h"
#include "transform.h"

#include <stdio.h>

void extapp_main(void)
{
    ctrl_wait_for_key_released();

	rtz_init(8, 4);
	srand(extapp_millis());

	mesh_ndc quad = rdr_init_mesh(4, 6);
	memcpy(quad.vertices,
		&(vertex_ndc[4]){
			{.xyz = {-0.5f, -0.5f, 1.f}, .rgb = { 1.f, 0.f, 0.f}},
			{.xyz = {+0.5f, -0.5f, 1.f}, .rgb = { 0.f, 1.f, 0.f}},
			{.xyz = {+0.5f, +0.5f, 1.f}, .rgb = { 0.f, 0.f, 1.f}},
			{.xyz = {-0.5f, +0.5f, 1.f}, .rgb = { 1.f, 1.f, 1.f}},
		},
		sizeof(vertex_ndc[4]));
	memcpy(quad.indices, &(u16[6]){0, 1, 3, 1, 2, 3}, sizeof(u16[6]));
	mesh_ndc mesh = rdr_copy_mesh(&quad);

	char overlay[256] = {0};
	size_t frame = 0;

	while (!extapp_scanKeyboard())
	{
		uint64_t start = extapp_millis();
		vec2 center = {0};
		tsf_center(&mesh, center);
		for (i16 k = 0; k < mesh.n_vertices; k++) {
			tsf_scale(
				((sin(rdr_degree2radians(frame % 360) * 6.f) + 1.f) / 2.f),
				quad.vertices[k].xyz,
				mesh.vertices[k].xyz);
			tsf_rotation(
				rdr_degree2radians((frame % 360)) * 2.f,
				center,
				mesh.vertices[k].xyz,
				mesh.vertices[k].xyz);
			tsf_translate(
				(vec2){
					cos(rdr_degree2radians(frame % 360) * 6.f) * 0.5f,
					sin(rdr_degree2radians(frame % 360) * 6.f) * 0.5f
				},
				mesh.vertices[k].xyz,
				mesh.vertices[k].xyz);
		}

		rdr_render_mesh(&mesh);
		rtz_swap_buffers();

		sprintf(overlay, "FRAME: %zu", frame++);
		extapp_drawTextSmall(overlay, 0, 0, 0xFFFF, 0x0000, 0);

		while (extapp_millis() - start < 33);
	}

	rdr_free_mesh(mesh);
	rdr_free_mesh(quad);
	rtz_free();
	return;
}
