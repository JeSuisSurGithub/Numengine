#include "commons.h"
#include "interface.h"
#include "floatops.h"
#include "renderer.h"

#include <stdio.h>

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

int main(void)
{
    ntf_wait_for_key_released();

	// 80x60 Framebuffer
	rtz_init(4, 4);

	// Track frames
	u32 frame = 0;

	// Cube
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

	mesh_ndc mesh1 = rdr_clone_mesh(&model);
	mesh_ndc mesh2 = rdr_clone_mesh(&model);

	u64 kb = ntf_kb_scan();

	camera cam = { {0.f, 0.f, 0.f}, 75.f, PI, 0.f };

	// In number of frames, 3 * 32ms = ~100ms
	i16 cooldown_max = 3;
	i16 cooldown = cooldown_max;

	// Overlay
	char overlay[128] = {0};

	while (!((kb = ntf_kb_scan()) & SCANCODE_Home))
	{
		uint64_t start = ntf_clock_ms();

		// FoV
		if (kb & FOV_MORE) {
			if (cam.fov < 180.f) cam.fov += 0.3f;
		}
		if (kb & FOV_LESS) {
			if (cam.fov > 20.f) cam.fov -= 0.3f;
		}

		// Wireframe
		if (kb & WIREFRAME) {
			if (cooldown > 0) {
				--cooldown;
			} else {
				cooldown = cooldown_max;
				rdr_toggle_wireframe();
			}
		}

		// Camera rotation
		if (kb & CAMERA_LEFT) {
			cam.yaw -= PI / 180.f;
		}
		if (kb & CAMERA_RIGHT) {
			cam.yaw += PI / 180.f;
		}
		if (kb & CAMERA_DOWN) {
			cam.pitch -= PI / 180.f;
		}
		if (kb & CAMERA_UP) {
			cam.pitch += PI / 180.f;
		}

		vec3 forward = {0};
		vec3 right = {0};
		fop_forward(cam.pitch, cam.yaw, forward);
		fop_right(cam.yaw, right);

		fop_vec3_normalize(forward);
		fop_vec3_normalize(right);

		// Movement
		if (kb & XYZ_FORWARD) {
			vec3 movement = {0};
			fop_vec3_mulf(forward, .5f, movement);
			fop_vec3_1add2(cam.xyz, movement, cam.xyz);
		}
		if (kb & XYZ_BACK) {
			vec3 movement = {0};
			fop_vec3_mulf(forward, -.5f, movement);
			fop_vec3_1add2(cam.xyz, movement, cam.xyz);
		}
		if (kb & XYZ_RIGHT) {
			vec3 movement = {0};
			fop_vec3_mulf(right, .5f, movement);
			fop_vec3_1add2(cam.xyz, movement, cam.xyz);
		}
		if (kb & XYZ_LEFT) {
			vec3 movement = {0};
			fop_vec3_mulf(right, -.5f, movement);
			fop_vec3_1add2(cam.xyz, movement, cam.xyz);
		}

		// Update
		for (i16 k = 0; k < model.n_vertices; k++)
		{
			vec3 mesh1_centroid = {0};
			compute_centroid(&mesh1, mesh1_centroid);

			vec3 mesh2_centroid = {0};
			compute_centroid(&mesh2, mesh2_centroid);

			fop_3d_roll_rotation(
				fop_deg2rad(30.f),
				mesh1_centroid,
				mesh1.vertices[k].xyz,
				mesh1.vertices[k].xyz);
			fop_3d_pitch_rotation(
				fop_deg2rad(-30.f),
				mesh2_centroid,
				mesh2.vertices[k].xyz,
				mesh2.vertices[k].xyz);

			fop_3d_yaw_rotation(
				fop_deg2rad(frame % 360) * 6.f,
				mesh1_centroid,
				mesh1.vertices[k].xyz,
				mesh1.vertices[k].xyz);
			fop_3d_yaw_rotation(
				fop_deg2rad(frame % 360) * -6.f,
				mesh2_centroid,
				mesh2.vertices[k].xyz,
				mesh2.vertices[k].xyz);

			fop_3d_translate(
				(vec3){
					0.f,
					-1.f,
					20.f,
				},
				mesh1.vertices[k].xyz,
				mesh1.vertices[k].xyz);

			fop_3d_translate(
				(vec3){
					0.f,
					1.f,
					-20.f,
				},
				mesh2.vertices[k].xyz,
				mesh2.vertices[k].xyz);
		}

		// Render
		rdr_render_mesh(&mesh1, &cam, forward);
		rdr_render_mesh(&mesh2, &cam, forward);

		// Reset
		rdr_copy_mesh(&mesh1, &model);
		rdr_copy_mesh(&mesh2, &model);

		// Swap framebuffers
		rtz_flush_framebuf();
		frame++;

		// Debugging
		i16 delta_time = ntf_clock_ms() - start;
		sprintf(overlay, "RENDER TIME %ims\nXYZ %.2f %.2f %.2f\nPITCH / YAW %.2f %.2f\nFOV %.2f",
			delta_time,
			cam.xyz[0], cam.xyz[1], cam.xyz[2],
			cam.pitch, cam.yaw,
			cam.fov);

		write_log(overlay);
		ntf_print_text(read_log(), 0, 0);
		reset_log();

		while (ntf_clock_ms() - start < 32);
	}

	// Cleanup
	rdr_free_mesh(mesh1);
	rdr_free_mesh(mesh2);
	rdr_free_mesh(model);
	rtz_free();
	return 0;
}

