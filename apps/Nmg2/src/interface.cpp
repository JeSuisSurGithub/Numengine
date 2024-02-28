#include "interface.hpp"
#include "commons.hpp"

namespace nmg {
	void wait_for_key_pressed()
	{
		while (!extapp_scanKeyboard())
			extapp_msleep(10);
		return;
	}

	void wait_for_key_released()
	{
		while (extapp_scanKeyboard())
			extapp_msleep(10);
		return;
	}

	u64 kb_scan()
	{
		return extapp_scanKeyboard();
	}

	u64 clock_ms()
	{
		return extapp_millis();
	}

	void print_text(const std::string& str, i16 x, i16 y)
	{
		extapp_drawTextSmall(str.c_str(), x, y, 0xFFFF, 0x0000, 0);
		return;
	}

	void draw_rect(i16 x, i16 y, u16 w, u16 h, u16 color)
	{
		extapp_pushRectUniform(x, y, w, h, color);
	}
}

void extapp_main(void) {
	nmg::DEBUG_LOG = new std::string;
	int res = main();
	delete nmg::DEBUG_LOG;
}