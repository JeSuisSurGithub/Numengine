#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "commons.hpp"

#include "../../../api/extapp_api.h"

namespace nsp {
	typedef enum CONTROLS {
	}CONTROLS;

	void wait_for_key_pressed();
	void wait_for_key_released();
	u64  kb_scan();
	u64  clock_ms();
	void print_text(const std::string& str, i16 x, i16 y);
	void draw_rect(i16 x, i16 y, u16 w, u16 h, u16 color);
}

extern "C" void extapp_main();
extern "C" int main();

#endif /* INTERFACE_HPP */