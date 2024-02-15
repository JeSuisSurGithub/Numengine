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

u16 rtz_draw_line_(const ss_vertex* pa, const ss_vertex* pb, ss_vertex* vertex_line_cache)
{
	ss_vertex* iter = vertex_line_cache;
	i16 steps = rtz_distance_(pa->x, pb->x, pa->y, pb->y);
	if (steps == 0) {
		rtz_put_pixel_(pa);
		if (vertex_line_cache != NULL) { *iter++ = *pa; }
		return (iter - vertex_line_cache);
	}
	if (steps == 1) {
		rtz_put_pixel_(pa);
		rtz_put_pixel_(pb);
		if (vertex_line_cache != NULL) {
			*iter++ = *pa;
			*iter++ = *pb;
		}
		return (iter - vertex_line_cache);
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
		if (vertex_line_cache != NULL) { *iter++ = pk; }
	}
	return (iter - vertex_line_cache);
}

void rtz_scanline_fill_(
	const ss_vertex* long_line,
	u16 long_line_len,
	const ss_vertex* line1_p1,
	const ss_vertex* line1_p2,
	const ss_vertex* line2_p1,
	const ss_vertex* line2_p2
)
{
	const ss_vertex* cur_line_p1 = line1_p1;
	const ss_vertex* cur_line_p2 = line1_p2;

	i16 cur_line_len  = rtz_distance_(cur_line_p1->x, cur_line_p2->x, cur_line_p1->y, cur_line_p2->y);
	i16 cur_line_index = 0;

	i16 x_diff = cur_line_p2->x - cur_line_p1->x;
	i16 y_diff = cur_line_p2->y - cur_line_p1->y;
	i16 z_diff = cur_line_p2->z - cur_line_p1->z;
	i16 r_diff = cur_line_p2->r - cur_line_p1->r;
	i16 g_diff = cur_line_p2->g - cur_line_p1->g;
	i16 b_diff = cur_line_p2->b - cur_line_p1->b;
	ss_vertex cur_point = {0};

	for (u16 long_line_index = 0; long_line_index < long_line_len; long_line_index++)
	{
		// Skip if next one is on the same line
		if ((long_line_index + 1) < long_line_len && long_line[long_line_index].y == long_line[long_line_index + 1].y)
			continue;

		// Interpolate until aligns
		do {
			float part = fop_clamp((float)cur_line_index / cur_line_len, -1.f, 1.f);
			cur_point.x = (i16)(cur_line_p1->x + (x_diff * part));
			cur_point.y = (i16)(cur_line_p1->y + (y_diff * part));
			cur_point.z = (i16)(cur_line_p1->z + (z_diff * part));
			cur_point.r =  (u8)(cur_line_p1->r + (r_diff * part));
			cur_point.g =  (u8)(cur_line_p1->g + (g_diff * part));
			cur_point.b =  (u8)(cur_line_p1->b + (b_diff * part));
		}
		while (cur_line_index++ <= cur_line_len && cur_point.y != long_line[long_line_index].y);

		rtz_draw_line_(&long_line[long_line_index], &cur_point, NULL);

		// Go next edge
		if (cur_line_index > cur_line_len)
		{
			cur_line_p1 = line2_p1;
			cur_line_p2 = line2_p2;

			cur_line_len  = rtz_distance_(cur_line_p1->x, cur_line_p2->x, cur_line_p1->y, cur_line_p2->y);
			cur_line_index = 0;

			x_diff = cur_line_p2->x - cur_line_p1->x;
			y_diff = cur_line_p2->y - cur_line_p1->y;
			z_diff = cur_line_p2->z - cur_line_p1->z;
			r_diff = cur_line_p2->r - cur_line_p1->r;
			g_diff = cur_line_p2->g - cur_line_p1->g;
			b_diff = cur_line_p2->b - cur_line_p1->b;
		}
	}

	return;
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
		rtz_draw_line_(&pa, &pb, NULL);
		rtz_draw_line_(&pb, &pc, NULL);
		rtz_draw_line_(&pc, &pa, NULL);
	}
	else {
		// "Long" Line, line with the biggest delta Y
		u16 ab_delta_y = abs(pa.y - pb.y);
		u16 bc_delta_y = abs(pb.y - pc.y);
		u16 ca_delta_y = abs(pc.y - pa.y);
		bool ab_longer_bc = ab_delta_y > bc_delta_y;
		bool bc_longer_ca = bc_delta_y > ca_delta_y;
		bool ca_longer_ab = ca_delta_y > ab_delta_y;
		bool ab_longest = ab_longer_bc && !ca_longer_ab;
		bool bc_longest = bc_longer_ca && !ab_longer_bc;
		bool ca_longest = ca_longer_ab && !bc_longer_ca;

		const ss_vertex* p1 = NULL;
		const ss_vertex* p2 = NULL;
		const ss_vertex* p3 = NULL;

		if (ab_longest) {
			p1 = &pa;
			p2 = &pb;
			p3 = &pc;
		}
		else if (bc_longest) {
			p1 = &pb;
			p2 = &pc;
			p3 = &pa;
		}
		else if (ca_longest) {
			p1 = &pc;
			p2 = &pa;
			p3 = &pb;
		}
		else {
			return;
		}

		ss_vertex* long_line_buf = malloc(sizeof(ss_vertex) * (rtz_distance_(p1->x, p2->x, p1->y, p2->y) + 1));
		i16 long_line_len = rtz_draw_line_(p1, p2, long_line_buf);
		rtz_scanline_fill_(long_line_buf, long_line_len, p1, p3, p3, p2);
		free(long_line_buf);
	}
	return;
}