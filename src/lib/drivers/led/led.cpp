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

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_config.h>
#ଅନ୍ତର୍ଭାଵ <lib/cdev/CDev.hpp>
#ଅନ୍ତର୍ଭାଵ <drivers/drv_board_led.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
ବାହ୍ୟ ଶୂନ୍ୟ led_init();
ବାହ୍ୟ ଶୂନ୍ୟ led_on(ପୂର୍ଣ୍ଣସଂଖ୍ୟା led);
ବାହ୍ୟ ଶୂନ୍ୟ led_off(ପୂର୍ଣ୍ଣସଂଖ୍ୟା led);
ବାହ୍ୟ ଶୂନ୍ୟ led_toggle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା led);
__END_DECLS

ଶ୍ରେଣୀ LED : cdev::CDev
{
ସର୍ବସାଧାରଣ:
	LED();
	~LED() override = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	init() override;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ioctl(cdev::file_t *filp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, unsigned long arg) override;
};

LED::LED() : CDev(LED0_DEVICE_PATH)
{
	// force immediate init/device registration
	init();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
LED::ioctl(cdev::file_t *filp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, unsigned long arg)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = OK;

	ବାଛ (cmd) {
	ସ୍ଥିତି LED_ON:
		led_on(arg);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି LED_OFF:
		led_off(arg);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି LED_TOGGLE:
		led_toggle(arg);
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		result = CDev::ioctl(filp, cmd, arg);
	}

	ଫେରନ୍ତୁ result;
}

ନାମାକାଶ
{
LED	*gLED;
}

ଶୂନ୍ୟ
drv_led_start(ଶୂନ୍ୟ)
{
	ଯଦି (gLED == nullptr) {
		gLED = ନୂତନ LED;
	}
}
