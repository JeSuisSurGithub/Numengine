#ifndef INTERFACE_H
#define INTERFACE_H

#include "commons.h"

#include "../../../api/extapp_api.h"

typedef enum CONTROLS {
	CAMERA_UP      = SCANCODE_Toolbox,
	CAMERA_DOWN    = SCANCODE_Sqrt,
	CAMERA_LEFT    = SCANCODE_Imaginary,
	CAMERA_RIGHT   = SCANCODE_Power,
	XYZ_FORWARD    = SCANCODE_Up,
	XYZ_BACK       = SCANCODE_Down,
	XYZ_LEFT       = SCANCODE_Left,
	XYZ_RIGHT      = SCANCODE_Right,
	FOV_MORE       = SCANCODE_Alpha,
	FOV_LESS       = SCANCODE_Cosine,
	MODE_WIREFRAME = SCANCODE_Shift,
	MODE_FREEZE    = SCANCODE_Sine,
}CONTROLS;

void ntf_wait_for_key_pressed();
void ntf_wait_for_key_released();
u64 ntf_kb_scan();
u64 ntf_clock_ms();
void ntf_print_text(const char* str, i16 x, i16 y);
void ntf_draw_rect(i16 x, i16 y, u16 w, u16 h, u16 color);

extern int main();

#endif /* INTERFACE_H */