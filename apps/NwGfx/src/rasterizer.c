#include "rasterizer.h"
#include "floatops.h"
#include "../../../api/extapp_api.h"

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
	extapp_waitForVBlank();
	for (i16 k = 0; k < RENDER_WIDTH * RENDER_HEIGHT; k++) {
		extapp_pushRectUniform(
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

void rtz_put_pixel_(const vertex_* p)
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
	return sqrtf(powf(ax - bx, 2) + powf(ay - by, 2));
}

void rtz_draw_line_(const vertex_* pa, const vertex_* pb, vertex_* line)
{
	vertex_* iter = line;
	u16 steps = rtz_distance_(pa->x, pb->x, pa->y, pb->y);
	if (steps == 0) {
		rtz_put_pixel_(pa);
		if (line != NULL) { *iter++ = *pa; }
		return;
	}
	if (steps == 1) {
		rtz_put_pixel_(pa);
		rtz_put_pixel_(pb);
		if (line != NULL) {
			*iter++ = *pa;
			*iter++ = *pb;
		}
		return;
	}

	i16 v_diff = pb->x - pa->x;
	i16 h_diff = pb->y - pa->y;
	i16 z_diff = pb->z - pa->z;
	i16 r_diff = pb->r - pa->r;
	i16 g_diff = pb->g - pa->g;
	i16 b_diff = pb->b - pa->b;

	vertex_ pk = {0};
	for (i16 k = 0; k <= steps; k++)
	{
		pk.x = (i16)(pa->x + ((float)(v_diff * k) / steps));
		pk.y = (i16)(pa->y + ((float)(h_diff * k) / steps));
		pk.z = (i16)(pa->z + ((float)(z_diff * k) / steps));
		pk.r =  (u8)(pa->r + ((float)(r_diff * k) / steps));
		pk.g =  (u8)(pa->g + ((float)(g_diff * k) / steps));
		pk.b =  (u8)(pa->b + ((float)(b_diff * k) / steps));
		rtz_put_pixel_(&pk);
		if (line != NULL) { *iter++ = pk; }
	}
	return;
}

void rtz_scanline_fill_(
	const vertex_* long_line,
	u16 long_line_distance,
	const vertex_* line1_vertex1,
	const vertex_* line1_vertex2,
	const vertex_* line2_vertex1,
	const vertex_* line2_vertex2
)
{
	i16 n = 1;

	u16 steps  = rtz_distance_(line1_vertex1->x, line1_vertex2->x, line1_vertex1->y, line1_vertex2->y);
	i16 v_diff = line1_vertex2->x - line1_vertex1->x;
	i16 h_diff = line1_vertex2->y - line1_vertex1->y;
	i16 z_diff = line1_vertex2->z - line1_vertex1->z;
	i16 r_diff = line1_vertex2->r - line1_vertex1->r;
	i16 g_diff = line1_vertex2->g - line1_vertex1->g;
	i16 b_diff = line1_vertex2->b - line1_vertex1->b;

	i16 k = 0;
	vertex_ pk = {
		.x = (i16)(line1_vertex1->x + ((float)(v_diff * k) / steps)),
		.y = (i16)(line1_vertex1->y + ((float)(h_diff * k) / steps)),
		.z = (i16)(line1_vertex1->z + ((float)(z_diff * k) / steps)),
		.r =  (u8)(line1_vertex1->r + ((float)(r_diff * k) / steps)),
		.g =  (u8)(line1_vertex1->g + ((float)(g_diff * k) / steps)),
		.b =  (u8)(line1_vertex1->b + ((float)(b_diff * k) / steps)),
	};
	for (; k < steps && n < long_line_distance; n++)
	{
		while (long_line[n].y != pk.y) {
			if (h_diff == 0) {
				n++;
			}
			k++;
			pk.y = (i16)(line1_vertex1->y + ((float)(h_diff * k) / steps));
		}
		pk.x = (i16)(line1_vertex1->x + ((float)(v_diff * k) / steps));
		pk.z = (i16)(line1_vertex1->z + ((float)(z_diff * k) / steps));
		pk.r =  (u8)(line1_vertex1->r + ((float)(r_diff * k) / steps));
		pk.g =  (u8)(line1_vertex1->g + ((float)(g_diff * k) / steps));
		pk.b =  (u8)(line1_vertex1->b + ((float)(b_diff * k) / steps));
		rtz_draw_line_(&long_line[n], &pk, NULL);
	}
	n--;

	steps  = rtz_distance_(line2_vertex1->x, line2_vertex2->x, line2_vertex1->y, line2_vertex2->y);
	v_diff = line2_vertex2->x - line2_vertex1->x;
	h_diff = line2_vertex2->y - line2_vertex1->y;
	z_diff = line2_vertex2->z - line2_vertex1->z;
	r_diff = line2_vertex2->r - line2_vertex1->r;
	g_diff = line2_vertex2->g - line2_vertex1->g;
	b_diff = line2_vertex2->b - line2_vertex1->b;

	k = 0;
	pk.x = (i16)(line2_vertex1->x + ((float)(v_diff * k) / steps));
	pk.y = (i16)(line2_vertex1->y + ((float)(h_diff * k) / steps));
	pk.z = (i16)(line2_vertex1->z + ((float)(z_diff * k) / steps));
	pk.r =  (u8)(line2_vertex1->r + ((float)(r_diff * k) / steps));
	pk.g =  (u8)(line2_vertex1->g + ((float)(g_diff * k) / steps));
	pk.b =  (u8)(line2_vertex1->b + ((float)(b_diff * k) / steps));

	for (; k < steps && n < long_line_distance; n++)
	{
		while (long_line[n].y != pk.y) {
			if (h_diff == 0) {
				n++;
			}
			k++;
			pk.y = (i16)(line2_vertex1->y + ((float)(h_diff * k) / steps));
		}
		pk.x = (i16)(line2_vertex1->x + ((float)(v_diff * k) / steps));
		pk.z = (i16)(line2_vertex1->z + ((float)(z_diff * k) / steps));
		pk.r =  (u8)(line2_vertex1->r + ((float)(r_diff * k) / steps));
		pk.g =  (u8)(line2_vertex1->g + ((float)(g_diff * k) / steps));
		pk.b =  (u8)(line2_vertex1->b + ((float)(b_diff * k) / steps));
		rtz_draw_line_(&long_line[n], &pk, NULL);
	}
	return;
}

vertex_ rtz_ndc_to_viewport_(const vertex_ndc* p)
{
	vertex_ np = {0};
	np.x = (p->xyz[0] + 1) / 2 * RENDER_WIDTH;
	np.y = (p->xyz[1] + 1) / 2 * RENDER_HEIGHT;
	np.z = p->xyz[2] * INT16_MAX;
	np.r = p->rgb[0] * UINT8_MAX;
	np.g = p->rgb[1] * UINT8_MAX;
	np.b = p->rgb[2] * UINT8_MAX;
	return np;
}

bool rtz_comp_point_(const vertex_ndc* left, const vertex_ndc* right) {
	return (left->xyz[0] == right->xyz[0]) && (left->xyz[1] == right->xyz[1]);
}

void rtz_draw_triangle(const vertex_ndc* pa_ndc, const vertex_ndc* pb_ndc, const vertex_ndc* pc_ndc, bool wireframe)
{
	if (rtz_comp_point_(pa_ndc, pb_ndc) || rtz_comp_point_(pb_ndc, pc_ndc) || rtz_comp_point_(pc_ndc, pa_ndc)) {
		return;
	}

	// Temporary
	if (pa_ndc->xyz[0] <= -1.f || pa_ndc->xyz[0] >= 1.f || pa_ndc->xyz[1] <= -1.f || pa_ndc->xyz[1] >= 1.f) {
		return;
	}
	if (pb_ndc->xyz[0] <= -1.f || pb_ndc->xyz[0] >= 1.f || pb_ndc->xyz[1] <= -1.f || pb_ndc->xyz[1] >= 1.f) {
		return;
	}
	if (pc_ndc->xyz[0] <= -1.f || pc_ndc->xyz[0] >= 1.f || pc_ndc->xyz[1] <= -1.f || pc_ndc->xyz[1] >= 1.f) {
		return;
	}

	const vertex_ pa = rtz_ndc_to_viewport_(pa_ndc);
	const vertex_ pb = rtz_ndc_to_viewport_(pb_ndc);
	const vertex_ pc = rtz_ndc_to_viewport_(pc_ndc);

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

		const vertex_* p1 = NULL;
		const vertex_* p2 = NULL;
		const vertex_* p3 = NULL;

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

		u16 long_line_distance = rtz_distance_(p1->x, p2->x, p1->y, p2->y);
		vertex_* long_line_buf = malloc(sizeof(vertex_) * (long_line_distance + 1));
		rtz_draw_line_(p1, p2, long_line_buf);
		rtz_scanline_fill_(long_line_buf, long_line_distance, p1, p3, p3, p2);
		free(long_line_buf);
	}
	return;
}