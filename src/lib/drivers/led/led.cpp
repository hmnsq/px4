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

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/cdev/CDev.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_board_led.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
ਬਾਹਰੀ ਖਾਲੀ led_init();
ਬਾਹਰੀ ਖਾਲੀ led_on(ਪੂਰਨ led);
ਬਾਹਰੀ ਖਾਲੀ led_off(ਪੂਰਨ led);
ਬਾਹਰੀ ਖਾਲੀ led_toggle(ਪੂਰਨ led);
__END_DECLS

ਵਰਗ LED : cdev::CDev
{
ਜਨਤਕ:
	LED();
	~LED() override = ਮੂਲ;

	ਪੂਰਨ	init() override;
	ਪੂਰਨ	ioctl(cdev::file_t *filp, ਪੂਰਨ cmd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg) override;
};

LED::LED() : CDev(LED0_DEVICE_ਰਸਤਾ)
{
	// force immediate init/device registration
	init();
}

ਪੂਰਨ
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	ਵਾਪਸ 0;
}

ਪੂਰਨ
LED::ioctl(cdev::file_t *filp, ਪੂਰਨ cmd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg)
{
	ਪੂਰਨ ਨਤੀਜਾ = OK;

	ਚੋਣ (cmd) {
	ਹਾਲਤ LED_ON:
		led_on(arg);
		ਰੋਕੋ;

	ਹਾਲਤ LED_OFF:
		led_off(arg);
		ਰੋਕੋ;

	ਹਾਲਤ LED_TOGGLE:
		led_toggle(arg);
		ਰੋਕੋ;

	ਮੂਲ:
		ਨਤੀਜਾ = CDev::ioctl(filp, cmd, arg);
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਨਾਮਾਕਾਸ਼
{
LED	*gLED;
}

ਖਾਲੀ
drv_led_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
	ਜੇ (gLED == ਖਾਲੀ_ਸੂਚਕ) {
		gLED = ਨਵਾਂ LED;
	}
}
