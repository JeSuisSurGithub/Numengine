#include "commons.h"
#include "controls.h"
#include "floatops.h"
#include "renderer.h"

#include "../../../api/extapp_api.h"

#include <stdio.h>

/*
	TODO:
	Fix Camera
	Fix Culling
	Lighting
	Clipping
	Texture UV, Palette
*/

void compute_centroid(const mesh_ndc* mesh, vec3 out)
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
	rtz_init(4, 4);

	mesh_ndc model = rdr_init_mesh(8, 36);
	memcpy(model.vertices,
		&(vertex_ndc[8]){
			{.xyz = {-0.5, -0.5, -0.5} , .rgb = { 1.f, 0.f, 0.f}},
			{.xyz = { 0.5, -0.5, -0.5} , .rgb = { 0.f, 1.f, 0.f}},
			{.xyz = { 0.5,  0.5, -0.5} , .rgb = { 0.f, 0.f, 1.f}},
			{.xyz = {-0.5,  0.5, -0.5} , .rgb = { 0.f, 1.f, 1.f}},
			{.xyz = {-0.5, -0.5,  0.5} , .rgb = { 1.f, 0.f, 1.f}},
			{.xyz = { 0.5, -0.5,  0.5} , .rgb = { 1.f, 1.f, 0.f}},
			{.xyz = { 0.5,  0.5,  0.5} , .rgb = { 0.f, 0.f, 0.f}},
			{.xyz = {-0.5,  0.5,  0.5} , .rgb = { 1.f, 1.f, 1.f}},
		},
		sizeof(vertex_ndc[8]));
	memcpy(model.indices, &(u16[36])
		{
			0, 1, 2, 2, 3, 0,   // Front face
			7, 6, 5, 5, 4, 7,   // Back face
			0, 4, 5, 5, 1, 0,   // Left face
			3, 2, 6, 6, 7, 3,   // Right face
			0, 3, 7, 7, 4, 0,   // Top face
			1, 5, 6, 6, 2, 1,   // Bottom face
		},
		sizeof(u16[36]));

	// Overlay
	char overlay[256] = {0};
	size_t frame = 0;
	u64 kb = ctrl_kb_scan();

	// Main Loop
	mesh_ndc mesh = rdr_clone_mesh(&model);
	camera cam = {
		{0.f, 0.f, 0.f},
		PI,
		0.f,
	};

	while (!((kb = ctrl_kb_scan()) & SCANCODE_Home))
	{
		uint64_t start = extapp_millis();
		rdr_copy_mesh(&mesh, &model);

		/*vec3 forward = {
            cosf(cam.pitch) * sinf(cam.yaw),
            sinf(cam.pitch),
            cosf(cam.pitch) * cosf(cam.yaw)};

        vec3 right = {
            sinf(cam.yaw - PI / 2.f),
            0,
            cosf(cam.yaw - PI / 2.f)};

		if (kb & SCANCODE_Left) {
			cam.yaw -= PI / 90.f;
		}
		if (kb & SCANCODE_Right) {
			cam.yaw += PI / 90.f;
		}
		if (kb & SCANCODE_Down) {
			cam.pitch -= PI / 90.f;
		}
		if (kb & SCANCODE_Up) {
			cam.pitch += PI / 90.f;
		}
		if (kb & SCANCODE_Imaginary) {
			vec3 movement = {0};
			fop_vec3_mulf(right, 0.0001f, movement);
			fop_vec3_1add2(cam.xyz, right, cam.xyz);
		}
		if (kb & SCANCODE_Power) {
			vec3 movement = {0};
			fop_vec3_mulf(right, 0.0001f, movement);
			fop_vec3_1sub2(cam.xyz, right, cam.xyz);
		}
		if (kb & SCANCODE_Sqrt) {
			vec3 movement = {0};
			fop_vec3_mulf(forward, 0.0001f, movement);
			fop_vec3_1add2(cam.xyz, forward, cam.xyz);
		}
		if (kb & SCANCODE_Toolbox) {
			vec3 movement = {0};
			fop_vec3_mulf(forward, 0.0001f, movement);
			fop_vec3_1sub2(cam.xyz, forward, cam.xyz);
		}*/

		// Update
		for (i16 k = 0; k < mesh.n_vertices; k++)
		{
			vec3 mesh_centroid = {0};
			compute_centroid(&mesh, mesh_centroid);

			fop_3d_roll_rotation(
				fop_deg2rad(30.f),
				mesh_centroid,
				mesh.vertices[k].xyz,
				mesh.vertices[k].xyz);

			fop_3d_yaw_rotation(
				fop_deg2rad(frame % 360) * 6.f,
				mesh_centroid,
				mesh.vertices[k].xyz,
				mesh.vertices[k].xyz);

			fop_3d_translate(
				(vec3){
					0.f,
					0.f,
					-10.f,
				},
				mesh.vertices[k].xyz,
				mesh.vertices[k].xyz);
		}

		// Render
		rdr_render_mesh(&mesh, &cam);
		rtz_flush_framebuf();

		// Render Overlay
		sprintf(overlay, "FRAME: %zu", frame++);
		extapp_drawTextSmall(overlay, 0, 0, 0xFFFF, 0x0000, 0);

		while (extapp_millis() - start < 33);
	}

	// Cleanup
	rdr_free_mesh(mesh);
	rdr_free_mesh(model);
	rtz_free();
	return;
}
