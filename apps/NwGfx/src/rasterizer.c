#include "floatops.h"
#include "interface.h"
#include "rasterizer.h"

u16 RENDER_WIDTH   = LCD_WIDTH;
u16 RENDER_HEIGHT  = LCD_HEIGHT;
u16 SCALE_WIDTH    = 1;
u16 SCALE_HEIGHT   = 1;
u16* rtz_framebuffer = NULL;
i16* rtz_depthbuffer = NULL;

void rtz_init(u16 horizontal_downscale, u16 vertical_downscale)
{
	RENDER_WIDTH = LCD_WIDTH / horizontal_downscale;
	RENDER_HEIGHT = LCD_HEIGHT / vertical_downscale;
	SCALE_WIDTH  = horizontal_downscale;
	SCALE_HEIGHT = vertical_downscale;
	rtz_framebuffer = malloc(RENDER_WIDTH * RENDER_HEIGHT * sizeof(u16));
	rtz_depthbuffer = malloc(RENDER_WIDTH * RENDER_HEIGHT * sizeof(i16));
	memset(rtz_framebuffer, 0, RENDER_WIDTH * RENDER_HEIGHT * sizeof(u16));
	memset(rtz_depthbuffer, INT16_MAX, RENDER_WIDTH * RENDER_HEIGHT * sizeof(i16));
	return;
}

void rtz_free()
{
	free(rtz_depthbuffer);
	free(rtz_framebuffer);
	return;
}

void rtz_flush_framebuf()
{
	for (i16 k = 0; k < RENDER_WIDTH * RENDER_HEIGHT; k++) {
		ntf_draw_rect(
			(k % RENDER_WIDTH) * SCALE_WIDTH, (k / RENDER_WIDTH) * SCALE_HEIGHT,
			SCALE_WIDTH, SCALE_HEIGHT,
			rtz_framebuffer[k]);
	}
	memset(rtz_framebuffer, 0, RENDER_WIDTH * RENDER_HEIGHT * sizeof(u16));
	memset(rtz_depthbuffer, INT16_MAX, RENDER_WIDTH * RENDER_HEIGHT * sizeof(i16));
	return;
}

u16 rtz_rgb888_to_rgb565_(u8 red, u8 green, u8 blue)
{
	u8 r5 = ((u16)red 	* 31 + 127) / 255;
    u8 g6 = ((u16)green * 63 + 127) / 255;
    u8 b5 = ((u16)blue 	* 31 + 127) / 255;

    u16 rgb565 = (r5 << 11) | (g6 << 5) | b5;
    return rgb565;
}

void rtz_put_pixel_(const ss_vertex* p)
{
	if (p->x < 0 || p->y < 0 || p->x >= RENDER_WIDTH || p->y >= RENDER_HEIGHT)
		return;
	if (p->z >= rtz_depthbuffer[p->y * RENDER_WIDTH + p->x]) {
		return;
	}
	rtz_framebuffer[p->y * RENDER_WIDTH + p->x] = rtz_rgb888_to_rgb565_(p->r, p->g, p->b);
	rtz_depthbuffer[p->y * RENDER_WIDTH + p->x] = p->z;
	return;
}

i16 rtz_distance_(i16 ax, i16 bx, i16 ay, i16 by)
{
	return roundf(sqrtf(powf(ax - bx, 2) + powf(ay - by, 2)));
}

void rtz_draw_line_(const ss_vertex* pa, const ss_vertex* pb)
{
	i16 steps = rtz_distance_(pa->x, pb->x, pa->y, pb->y);
	if (steps == 0) {
		rtz_put_pixel_(pa);
		return;
	}
	if (steps == 1) {
		rtz_put_pixel_(pa);
		rtz_put_pixel_(pb);
		return;
	}

	i16 x_diff = pb->x - pa->x;
	i16 y_diff = pb->y - pa->y;
	i16 z_diff = pb->z - pa->z;
	i16 r_diff = pb->r - pa->r;
	i16 g_diff = pb->g - pa->g;
	i16 b_diff = pb->b - pa->b;

	ss_vertex pk = {0};
	for (i16 k = 0; k <= steps; k++)
	{
		float part = fop_clamp((float)k / steps, -1.f, 1.f);
		pk.x = (i16)(pa->x + (x_diff * part));
		pk.y = (i16)(pa->y + (y_diff * part));
		pk.z = (i16)(pa->z + (z_diff * part));
		pk.r =  (u8)(pa->r + (r_diff * part));
		pk.g =  (u8)(pa->g + (g_diff * part));
		pk.b =  (u8)(pa->b + (b_diff * part));
		rtz_put_pixel_(&pk);
	}
	return;
}

void rtz_bufferless_filler_(const ss_vertex* pa, const ss_vertex* pb, const ss_vertex* pc)
{
	i16 ab_delta_x = pb->x - pa->x;
	i16 bc_delta_x = pc->x - pb->x;
	i16 ca_delta_x = pa->x - pc->x;

	i16 ab_delta_y = pb->y - pa->y;
	i16 bc_delta_y = pc->y - pb->y;
	i16 ca_delta_y = pa->y - pc->y;

	i16 ab_delta_z = pb->z - pa->z;
	i16 bc_delta_z = pc->z - pb->z;
	i16 ca_delta_z = pa->z - pc->z;

	i16 ab_delta_r = pb->r - pa->r;
	i16 bc_delta_r = pc->r - pb->r;
	i16 ca_delta_r = pa->r - pc->r;

	i16 ab_delta_g = pb->g - pa->g;
	i16 bc_delta_g = pc->g - pb->g;
	i16 ca_delta_g = pa->g - pc->g;

	i16 ab_delta_b = pb->b - pa->b;
	i16 bc_delta_b = pc->b - pb->b;
	i16 ca_delta_b = pa->b - pc->b;

	ss_vertex vertex_ab = {0};
	ss_vertex vertex_bc = {0};
	ss_vertex vertex_ca = {0};

	u16 min_y = 0;
	u16 max_y = RENDER_HEIGHT;

	if (pa->y < pb->y) {
        min_y = pa->y;
        max_y = pb->y;
    } else {
        min_y = pb->y;
        max_y = pa->y;
    }

    if (pc->y < min_y) {
        min_y = pc->y;
    } else if (pc->y > max_y) {
        max_y = pc->y;
    }

	for (u16 y = min_y; y < max_y; y++)
	{
		// "reverse interpolation", find the percentage at which we would get to Y along the lines AB, BC and CA
		float part_ab = (float)(y - pa->y) / ab_delta_y;
		float part_bc = (float)(y - pb->y) / bc_delta_y;
		float part_ca = (float)(y - pc->y) / ca_delta_y;

		bool intersect_ab = (0.f <= part_ab && part_ab <= 1.f);
		bool intersect_bc = (0.f <= part_bc && part_bc <= 1.f);
		bool intersect_ca = (0.f <= part_ca && part_ca <= 1.f);

		if (intersect_ab) {
			vertex_ab = (ss_vertex){
				.y = y,
				.x = (pa->x + ab_delta_x * part_ab),
				.z = (pa->z + ab_delta_z * part_ab),
				.r = (pa->r + ab_delta_r * part_ab),
				.g = (pa->g + ab_delta_g * part_ab),
				.b = (pa->b + ab_delta_b * part_ab),
			};
		}

		if (intersect_bc) {
			vertex_bc = (ss_vertex){
				.y = y,
				.x = pb->x + bc_delta_x * part_bc,
				.z = pb->z + bc_delta_z * part_bc,
				.r = pb->r + bc_delta_r * part_bc,
				.g = pb->g + bc_delta_g * part_bc,
				.b = pb->b + bc_delta_b * part_bc,
			};
		}

		if (intersect_ca) {
			vertex_ca = (ss_vertex){
				.y = y,
				.x = pc->x + ca_delta_x * part_ca,
				.z = pc->z + ca_delta_z * part_ca,
				.r = pc->r + ca_delta_r * part_ca,
				.g = pc->g + ca_delta_g * part_ca,
				.b = pc->b + ca_delta_b * part_ca,
			};
		}

		if (intersect_ab && intersect_bc) {
			rtz_draw_line_(&vertex_ab, &vertex_bc);
		}
		if (intersect_bc && intersect_ca) {
			rtz_draw_line_(&vertex_bc, &vertex_ca);
		}
		if (intersect_ca && intersect_ab) {
			rtz_draw_line_(&vertex_ca, &vertex_ab);
		}
	}
}

ss_vertex rtz_ndc_to_viewport_(const ndc_vertex* p)
{
	ss_vertex np = {0};
	np.x = (p->xyz[0] + 1) / 2 * RENDER_WIDTH;
	np.y = (p->xyz[1] + 1) / 2 * RENDER_HEIGHT;
	np.z = p->xyz[2] * INT16_MAX;
	np.r = p->rgb[0] * UINT8_MAX;
	np.g = p->rgb[1] * UINT8_MAX;
	np.b = p->rgb[2] * UINT8_MAX;
	return np;
}

bool rtz_comp_point_(const ndc_vertex* left, const ndc_vertex* right) {
	return (left->xyz[0] == right->xyz[0]) && (left->xyz[1] == right->xyz[1]);
}

void rtz_draw_triangle(const ndc_vertex* pa_ndc, const ndc_vertex* pb_ndc, const ndc_vertex* pc_ndc, bool wireframe)
{
	// Skip useless
	if (rtz_comp_point_(pa_ndc, pb_ndc) || rtz_comp_point_(pb_ndc, pc_ndc) || rtz_comp_point_(pc_ndc, pa_ndc)) {
		return;
	}
	if (pa_ndc->xyz[0] <= -1.f || pa_ndc->xyz[0] >= 1.f || pa_ndc->xyz[1] <= -1.f || pa_ndc->xyz[1] >= 1.f) {
		return;
	}
	if (pb_ndc->xyz[0] <= -1.f || pb_ndc->xyz[0] >= 1.f || pb_ndc->xyz[1] <= -1.f || pb_ndc->xyz[1] >= 1.f) {
		return;
	}
	if (pc_ndc->xyz[0] <= -1.f || pc_ndc->xyz[0] >= 1.f || pc_ndc->xyz[1] <= -1.f || pc_ndc->xyz[1] >= 1.f) {
		return;
	}

	const ss_vertex pa = rtz_ndc_to_viewport_(pa_ndc);
	const ss_vertex pb = rtz_ndc_to_viewport_(pb_ndc);
	const ss_vertex pc = rtz_ndc_to_viewport_(pc_ndc);

	if (wireframe) {
		rtz_draw_line_(&pa, &pb);
		rtz_draw_line_(&pb, &pc);
		rtz_draw_line_(&pc, &pa);
	}
	else {
		rtz_bufferless_filler_(&pa, &pb, &pc);
	}
	return;
}