#include "floatops.h"
#include "interface.h"
#include "rasterizer.h"

u16 RENDER_WIDTH   = LCD_WIDTH;
u16 RENDER_HEIGHT  = LCD_HEIGHT;
u16 SCALE_WIDTH    = 1;
u16 SCALE_HEIGHT   = 1;
u16* rtz_framebuffer = NULL;
u8*  rtz_depthbuffer = NULL;

void rtz_init(u16 horizontal_downscale, u16 vertical_downscale)
{
	RENDER_WIDTH = LCD_WIDTH / horizontal_downscale;
	RENDER_HEIGHT = LCD_HEIGHT / vertical_downscale;
	SCALE_WIDTH  = horizontal_downscale;
	SCALE_HEIGHT = vertical_downscale;
	rtz_framebuffer = malloc(RENDER_WIDTH * RENDER_HEIGHT * sizeof(u16));
	rtz_depthbuffer = malloc(RENDER_WIDTH * RENDER_HEIGHT * sizeof(u8));
	memset(rtz_framebuffer, 0, RENDER_WIDTH * RENDER_HEIGHT * sizeof(u16));
	memset(rtz_depthbuffer, UINT8_MAX, RENDER_WIDTH * RENDER_HEIGHT * sizeof(u8));
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
	memset(rtz_depthbuffer, UINT8_MAX, RENDER_WIDTH * RENDER_HEIGHT * sizeof(u8));
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
		pk.x = (i16)roundf(pa->x + (x_diff * part));
		pk.y = (i16)roundf(pa->y + (y_diff * part));
		pk.z =  (u8)roundf(pa->z + (z_diff * part));
		pk.r =  (u8)roundf(pa->r + (r_diff * part));
		pk.g =  (u8)roundf(pa->g + (g_diff * part));
		pk.b =  (u8)roundf(pa->b + (b_diff * part));
		rtz_put_pixel_(&pk);
	}
	return;
}

void rtz_clip_x(i16 y, ss_vertex* v1, ss_vertex* v2)
{
	if (v1->x < 0 || v1->x > RENDER_WIDTH || v2->x < 0 || v2->x > RENDER_WIDTH)
	{
		i16 delta_x = v2->x - v1->x;
		i16 delta_z = v2->z - v1->z;
		i16 delta_r = v2->r - v1->r;
		i16 delta_g = v2->g - v1->g;
		i16 delta_b = v2->b - v1->b;

		i16 v1x = (v1->x < 0) ? 0 : v1->x;
		v1x = (v1x > RENDER_WIDTH) ? RENDER_WIDTH : v1x;

		float v1_part = fop_clamp((float)(v1x - v1->x) / delta_x, -1.f, 1.f);
		(*v1) = (ss_vertex){
			.y = y,
			.x = v1x,
			.z = (v1->z + delta_z * v1_part),
			.r = (v1->r + delta_r * v1_part),
			.g = (v1->g + delta_g * v1_part),
			.b = (v1->b + delta_b * v1_part),
		};

		i16 v2x = (v2->x < 0) ? 0 : v2->x;
		v2x = (v2x > RENDER_WIDTH) ? RENDER_WIDTH : v2x;
		float v2_part = fop_clamp((float)(v2x - v2->x) / delta_x, -1.f, 1.f);
		(*v2) = (ss_vertex){
			.y = y,
			.x = v2x,
			.z = (v2->z + delta_z * v2_part),
			.r = (v2->r + delta_r * v2_part),
			.g = (v2->g + delta_g * v2_part),
			.b = (v2->b + delta_b * v2_part),
		};
	}
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

	i16 min_y = 0;
	i16 max_y = RENDER_HEIGHT;

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

	min_y = (min_y < 0) ? 0 : min_y;
	max_y = (max_y > RENDER_HEIGHT) ? RENDER_HEIGHT : max_y;

	for (i16 y = min_y; y <= max_y; y++)
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
				.x = (pb->x + bc_delta_x * part_bc),
				.z = (pb->z + bc_delta_z * part_bc),
				.r = (pb->r + bc_delta_r * part_bc),
				.g = (pb->g + bc_delta_g * part_bc),
				.b = (pb->b + bc_delta_b * part_bc),
			};
		}

		if (intersect_ca) {
			vertex_ca = (ss_vertex){
				.y = y,
				.x = (pc->x + ca_delta_x * part_ca),
				.z = (pc->z + ca_delta_z * part_ca),
				.r = (pc->r + ca_delta_r * part_ca),
				.g = (pc->g + ca_delta_g * part_ca),
				.b = (pc->b + ca_delta_b * part_ca),
			};
		}

		if (intersect_ab && intersect_bc) {
			rtz_clip_x(y, &vertex_ab, &vertex_bc);
			rtz_draw_line_(&vertex_ab, &vertex_bc);
		}
		if (intersect_bc && intersect_ca) {
			rtz_clip_x(y, &vertex_bc, &vertex_ca);
			rtz_draw_line_(&vertex_bc, &vertex_ca);
		}
		if (intersect_ca && intersect_ab) {
			rtz_clip_x(y, &vertex_ca, &vertex_ab);
			rtz_draw_line_(&vertex_ca, &vertex_ab);
		}
	}
}

ss_vertex rtz_ndc_to_viewport_(const ndc_vertex* p)
{
	ss_vertex np = {0};
	np.x = (p->xyz[0] + 1) / 2 * RENDER_WIDTH;
	np.y = (p->xyz[1] + 1) / 2 * RENDER_HEIGHT;
	np.z = (p->xyz[2] + 1) / 2 * UINT8_MAX;
	np.r = p->rgb[0] * UINT8_MAX;
	np.g = p->rgb[1] * UINT8_MAX;
	np.b = p->rgb[2] * UINT8_MAX;
	return np;
}

void rtz_draw_triangle(const ndc_vertex* pa_ndc, const ndc_vertex* pb_ndc, const ndc_vertex* pc_ndc, bool wireframe)
{
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