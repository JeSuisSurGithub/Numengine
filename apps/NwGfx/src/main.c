#include "commons.h"
#include "controls.h"
#include "floatops.h"
#include "renderer.h"

#include "../../../api/extapp_api.h"

#include <stdio.h>

void get_center(const mesh_ndc* mesh, vec3 out)
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

void extapp_main(void)
{
    ctrl_wait_for_key_released();

	// 40x60 Framebuffer
	rtz_init(8, 4);

	// Quad
	mesh_ndc model = rdr_init_mesh(4, 6);
	memcpy(model.vertices,
		&(vertex_ndc[4]){
			{.xyz = {-0.5f, -0.5f, 10.f}, .rgb = { 1.f, 0.f, 0.f}},
			{.xyz = {+0.5f, -0.5f, 10.f}, .rgb = { 0.f, 1.f, 0.f}},
			{.xyz = {+0.5f, +0.5f, 10.f}, .rgb = { 0.f, 0.f, 1.f}},
			{.xyz = {-0.5f, +0.5f, 10.f}, .rgb = { 1.f, 1.f, 1.f}},
		},
		sizeof(vertex_ndc[4]));
	memcpy(model.indices, &(u16[6]){0, 1, 3, 1, 2, 3}, sizeof(u16[6]));

	// Overlay
	char overlay[256] = {0};
	size_t frame = 0;
	u64 kb = ctrl_kb_scan();

	// Main Loop
	mesh_ndc mesh = rdr_init_mesh(4, 6);
	while (!((kb = ctrl_kb_scan()) & SCANCODE_Home))
	{
		uint64_t start = extapp_millis();
		rdr_copy_mesh(&mesh, &model);

		// Update
		/*for (i16 k = 0; k < mesh.n_vertices; k++) {
			fop_scale(
				((sin(rdr_degree2radians(frame % 360) * 6.f) + 1.f) / 2.f),
				quad.vertices[k].xyz,
				mesh.vertices[k].xyz);
			vec2 center = {0};
			fop_center(&mesh, center);
			fop_rotation(
				rdr_degree2radians((frame % 360)) * 2.f,
				center,
				mesh.vertices[k].xyz,
				mesh.vertices[k].xyz);
			fop_translate(
				(vec2){
					cos(rdr_degree2radians(frame % 360) * 6.f) * 0.5f,
					sin(rdr_degree2radians(frame % 360) * 6.f) * 0.5f
				},
				mesh.vertices[k].xyz,
				mesh.vertices[k].xyz);
		}*/

		// Render
		rdr_render_mesh(&mesh);
		rtz_flush_framebuf();

		// Render Overlay
		sprintf(overlay, "FRAME: %zu", frame++);
		extapp_drawTextSmall(overlay, 0, 0, 0xFFFF, 0x0000, 0);

		while (extapp_millis() - start < 33);
	}

	// Cleanup
	rdr_free_mesh(model);
	rtz_free();
	return;
}
