#include "controls.h"
#include "../../../api/extapp_api.h"

void ctrl_wait_for_key_pressed()
{
	while (!extapp_scanKeyboard())
		extapp_msleep(10);
}

void ctrl_wait_for_key_released()
{
	while (extapp_scanKeyboard())
		extapp_msleep(10);
}

u64 ctrl_kb_scan()
{
	return extapp_scanKeyboard();
}