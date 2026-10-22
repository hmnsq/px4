/****************************************************************************
 *
 *   Copyright (c) 2013 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file led.cpp
 *
 * LED driver to control the onboard LED(s) via ioctl interface.
 */

#അന്തര്ഭാവ <px4_platform_common/log.h>
#അന്തര്ഭാവ <px4_platform_common/px4_config.h>
#അന്തര്ഭാവ <lib/cdev/CDev.hpp>
#അന്തര്ഭാവ <drivers/drv_board_led.h>
#അന്തര്ഭാവ <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
ബാഹ്യ ശൂന്യം led_init();
ബാഹ്യ ശൂന്യം led_on(പൂർണ്ണസംഖ്യ led);
ബാഹ്യ ശൂന്യം led_off(പൂർണ്ണസംഖ്യ led);
ബാഹ്യ ശൂന്യം led_toggle(പൂർണ്ണസംഖ്യ led);
__END_DECLS

വർഗ്ഗം LED : cdev::CDev
{
പൊതു:
	LED();
	~LED() override = സ്ഥിരസ്ഥിതി;

	പൂർണ്ണസംഖ്യ	init() override;
	പൂർണ്ണസംഖ്യ	ioctl(cdev::file_t *filp, പൂർണ്ണസംഖ്യ cmd, unsigned long arg) override;
};

LED::LED() : CDev(LED0_DEVICE_PATH)
{
	// force immediate init/device registration
	init();
}

പൂർണ്ണസംഖ്യ
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
LED::ioctl(cdev::file_t *filp, പൂർണ്ണസംഖ്യ cmd, unsigned long arg)
{
	പൂർണ്ണസംഖ്യ result = OK;

	തിരഞ്ഞെടുപ്പ് (cmd) {
	അവസ്ഥ LED_ON:
		led_on(arg);
		നിർത്തുക;

	അവസ്ഥ LED_OFF:
		led_off(arg);
		നിർത്തുക;

	അവസ്ഥ LED_TOGGLE:
		led_toggle(arg);
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		result = CDev::ioctl(filp, cmd, arg);
	}

	തിരികെ result;
}

നാമാകാശ
{
LED	*gLED;
}

ശൂന്യം
drv_led_start(ശൂന്യം)
{
	എങ്കിൽ (gLED == nullptr) {
		gLED = പുതിയ LED;
	}
}
