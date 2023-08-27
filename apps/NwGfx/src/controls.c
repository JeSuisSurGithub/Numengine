#include "controls.h"

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