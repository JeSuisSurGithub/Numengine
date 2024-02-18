#include "interface.h"

void ntf_wait_for_key_pressed()
{
	while (!extapp_scanKeyboard())
		extapp_msleep(10);
	return;
}

void ntf_wait_for_key_released()
{
	while (extapp_scanKeyboard())
		extapp_msleep(10);
	return;
}

u64 ntf_kb_scan()
{
	return extapp_scanKeyboard();
}

u64 ntf_clock_ms()
{
	return extapp_millis();
}

void ntf_print_text(const char* str, i16 x, i16 y)
{
	extapp_drawTextSmall(str, x, y, 0xFFFF, 0x0000, 0);
	return;
}

void ntf_draw_rect(i16 x, i16 y, u16 w, u16 h, u16 color)
{
	extapp_pushRectUniform(x, y, w, h, color);
}

void extapp_main(void) {
	int res = main();
}