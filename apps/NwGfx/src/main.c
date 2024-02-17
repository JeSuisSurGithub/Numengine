#include "commons.h"
#include "interface.h"
#include "floatops.h"
#include "renderer.h"

#include <stdio.h>

int main(void)
{
    ntf_wait_for_key_released();

	// 4x4 pixel tiles, 80x60 - half native resolution
	rtz_init(4, 4);

	// Movement
	bool tick_freeze = false;
	u32 tick = 0;

	// Cube
	mesh cube = rdr_init_mesh(8, 36);
	memcpy(cube.vertices,
		&(ndc_vertex[8]){
			{.xyz = {-0.5, -0.5, -0.5} , .rgb = { 1.f, 0.f, 0.f}},
			{.xyz = { 0.5, -0.5, -0.5} , .rgb = { 0.f, 1.f, 0.f}},
			{.xyz = { 0.5,  0.5, -0.5} , .rgb = { 0.f, 0.f, 1.f}},
			{.xyz = {-0.5,  0.5, -0.5} , .rgb = { 0.f, 1.f, 1.f}},
			{.xyz = {-0.5, -0.5,  0.5} , .rgb = { 1.f, 0.f, 1.f}},
			{.xyz = { 0.5, -0.5,  0.5} , .rgb = { 1.f, 1.f, 0.f}},
			{.xyz = { 0.5,  0.5,  0.5} , .rgb = { 0.f, 0.f, 0.f}},
			{.xyz = {-0.5,  0.5,  0.5} , .rgb = { 1.f, 1.f, 1.f}},
		},
		sizeof(ndc_vertex[8]));
	memcpy(cube.indices, &(u16[36])
		{
			0, 1, 2, 2, 3, 0,   // Front face
			7, 6, 5, 5, 4, 7,   // Back face
			0, 4, 5, 5, 1, 0,   // Left face
			3, 2, 6, 6, 7, 3,   // Right face
			0, 3, 7, 7, 4, 0,   // Top face
			1, 5, 6, 6, 2, 1,   // Bottom face
		},
		sizeof(u16[36]));

	mesh mesh1 = rdr_clone_mesh(&cube);
	mesh mesh2 = rdr_clone_mesh(&cube);

	u64 kb = ntf_kb_scan();

	camera cam = { {0.f, 0.f, 0.f}, 75.f, 0.f, 0.f };

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
		if (kb & MODE_WIREFRAME) {
			if (cooldown > 0) {
				--cooldown;
			} else {
				cooldown = cooldown_max;
				rdr_toggle_wireframe();
			}
		}

		// Freeze
		if (kb & MODE_FREEZE) {
			if (cooldown > 0) {
				--cooldown;
			} else {
				cooldown = cooldown_max;
				tick_freeze = !tick_freeze;
			}
		}

		// Camera rotation
		if (kb & CAMERA_LEFT) {
			cam.yaw += PI / 30.f;
		}
		if (kb & CAMERA_RIGHT) {
			cam.yaw -= PI / 30.f;
		}
		if (kb & CAMERA_UP) {
			cam.pitch -= PI / 30.f;
		}
		if (kb & CAMERA_DOWN) {
			cam.pitch += PI / 30.f;
		}

		vec3 forward = {0};
		vec3 right = {0};
		fop_vec3_forward(forward, cam.pitch, cam.yaw);
		fop_vec3_right(right, cam.yaw);

		fop_vec3_normalize(forward, forward);
		fop_vec3_normalize(right, right);

		// Movement
		vec3 movement = {0};
		if (kb & XYZ_FORWARD) {
			fop_vec3_mul_flt(movement, forward, .5f);
			fop_vec3_1add2(cam.xyz, cam.xyz, movement);
		}
		if (kb & XYZ_BACK) {
			fop_vec3_mul_flt(movement, forward, -.5f);
			fop_vec3_1add2(cam.xyz, cam.xyz, movement);
		}
		if (kb & XYZ_RIGHT) {
			fop_vec3_mul_flt(movement, right, .5f);
			fop_vec3_1add2(cam.xyz, cam.xyz, movement);
		}
		if (kb & XYZ_LEFT) {
			fop_vec3_mul_flt(movement, right, -.5f);
			fop_vec3_1add2(cam.xyz, cam.xyz, movement);
		}

		// Update
		if (!tick_freeze) ++tick;

		mat4x4 mesh1_model_mat = {0};
		mat4x4 mesh2_model_mat = {0};

		mat4x4 yaw = {0};
		mat4x4 pitch = {0};
		mat4x4 roll = {0};
		mat4x4 scale = {0};
		mat4x4 translate = {0};

		fop_mat4_identity(mesh1_model_mat);
		fop_mat4_yaw(yaw, fop_deg2rad(tick % 360) * 6.f);
		fop_mat4_pitch(pitch, fop_deg2rad(-30.f));
		fop_mat4_roll(roll, fop_deg2rad(30.f));
		fop_mat4_scale(scale, (vec3){2.f,2.f,2.f});
		fop_mat4_translate(translate, (vec3){0.f,-4.f + sinf(fop_deg2rad(tick % 360) * 10.f) * 2.f,0.f});

		fop_mat4x4_1mul2(mesh1_model_mat, yaw, mesh1_model_mat);
		fop_mat4x4_1mul2(mesh1_model_mat, pitch, mesh1_model_mat);
		fop_mat4x4_1mul2(mesh1_model_mat, roll, mesh1_model_mat);
		fop_mat4x4_1mul2(mesh2_model_mat, scale, mesh1_model_mat);
		fop_mat4x4_1mul2(mesh1_model_mat, translate, mesh1_model_mat);

		fop_mat4_identity(mesh2_model_mat);
		fop_mat4_scale(scale, (vec3){20.f,6.f,20.f});
		fop_mat4_translate(translate, (vec3){0.f,2.f,0.f});

		fop_mat4x4_1mul2(mesh2_model_mat, scale, mesh2_model_mat);
		fop_mat4x4_1mul2(mesh2_model_mat, translate, mesh2_model_mat);

		for (i16 k = 0; k < cube.n_vertices; k++)
		{
			fop_mat4x4_mul_vec3(mesh1.vertices[k].xyz, mesh1_model_mat, mesh1.vertices[k].xyz);
			fop_mat4x4_mul_vec3(mesh2.vertices[k].xyz, mesh2_model_mat, mesh2.vertices[k].xyz);
		}

		// Render
		rdr_render_mesh(&mesh1, &cam, forward);
		rdr_render_mesh(&mesh2, &cam, forward);

		// Reset
		rdr_copy_mesh(&mesh1, &cube);
		rdr_copy_mesh(&mesh2, &cube);

		// Swap framebuffers
		rtz_flush_framebuf();

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
	rdr_free_mesh(cube);
	rtz_free();
	return 0;
}

