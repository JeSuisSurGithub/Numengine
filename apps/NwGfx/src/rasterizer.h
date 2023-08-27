#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "commons.h"

typedef struct vertex_ndc
{
	vec3 xyz;
	vec3 rgb;
}vertex_ndc;

typedef struct vertex_
{
	i16 x;
	i16 y;
	u8  r;
	u8  g;
	u8  b;
}vertex_;

void rtz_init(u16 horizontal_downscale, u16 vertical_downscale);
void rtz_free();
void rtz_swap_buffers();
u16 rtz_rgb888_to_rgb565_(u8 red, u8 green, u8 blue);
void rtz_put_pixel_(const vertex_* p);
i16 rtz_distance_(i16 ax, i16 bx, i16 ay, i16 by);
void rtz_draw_line_(const vertex_* pa, const vertex_* pb, vertex_* line);
void rtz_scanline_fill_(
	const vertex_* long_line,
	u16 long_line_distance,
	const vertex_* line1_vertex1,
	const vertex_* line1_vertex2,
	const vertex_* line2_vertex1,
	const vertex_* line2_vertex2
);
vertex_ rtz_ndc_to_viewport_(const vertex_ndc* p);
bool rtz_comp_point_(const vertex_ndc* left, const vertex_ndc* right);
void rtz_draw_triangle(const vertex_ndc* pa_ndc, const vertex_ndc* pb_ndc, const vertex_ndc* pc_ndc, bool wireframe);

#endif /* RASTERIZER_H */