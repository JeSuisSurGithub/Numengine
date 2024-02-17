#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "commons.h"

typedef struct ndc_vertex
{
	vec3 xyz;
	vec3 rgb;
}ndc_vertex;

typedef struct ss_vertex
{
	i16 x;
	i16 y;
	u8  z;
	u8  r;
	u8  g;
	u8  b;
}ss_vertex;

extern u16 RENDER_WIDTH;
extern u16 RENDER_HEIGHT;
extern u16 SCALE_WIDTH;
extern u16 SCALE_HEIGHT;

// Heap framebuffer and depthbuffer
extern u16* rtz_framebuffer;
extern u8*  rtz_depthbuffer;

void rtz_init(u16 horizontal_downscale, u16 vertical_downscale);
void rtz_free();
void rtz_flush_framebuf();

u16  rtz_rgb888_to_rgb565_(u8 red, u8 green, u8 blue);
void rtz_put_pixel_(const ss_vertex* p);
i16  rtz_distance_(i16 ax, i16 bx, i16 ay, i16 by);
void  rtz_draw_line_(const ss_vertex* pa, const ss_vertex* pb);
void rtz_bufferless_filler_(const ss_vertex* pa, const ss_vertex* pb, const ss_vertex* pc);
ss_vertex rtz_ndc_to_viewport_(const ndc_vertex* p);
void rtz_draw_triangle(
	const ndc_vertex* pa_ndc,
	const ndc_vertex* pb_ndc,
	const ndc_vertex* pc_ndc,
	bool wireframe);

#endif /* RASTERIZER_H */