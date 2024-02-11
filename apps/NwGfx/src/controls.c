#include "controls.h"

#include "../../../api/extapp_api.h"

void ctrl_wait_for_key_pressed()
{
	while (!extapp_scanKeyboard())
		extapp_msleep(10);
	return;
}

void ctrl_wait_for_key_released()
{
	while (extapp_scanKeyboard())
		extapp_msleep(10);
	return;
}

u64 ctrl_kb_scan()
{
	return extapp_scanKeyboard();
}