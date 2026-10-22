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

#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <lib/cdev/CDev.hpp>
#అన్తర్భావ <drivers/drv_board_led.h>
#అన్తర్భావ <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
బాహ్య శూన్య led_init();
బాహ్య శూన్య led_on(పూర్ణాంకం led);
బాహ్య శూన్య led_off(పూర్ణాంకం led);
బాహ్య శూన్య led_toggle(పూర్ణాంకం led);
__END_DECLS

తరగతి LED : cdev::CDev
{
బహిరంగ:
	LED();
	~LED() override = అప్రమేయ;

	పూర్ణాంకం	init() override;
	పూర్ణాంకం	ioctl(cdev::file_t *filp, పూర్ణాంకం cmd, గుర్తులేని దీర్ఘ arg) override;
};

LED::LED() : CDev(LED0_DEVICE_మార్గం)
{
	// force immediate init/device registration
	init();
}

పూర్ణాంకం
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	తిరిగి 0;
}

పూర్ణాంకం
LED::ioctl(cdev::file_t *filp, పూర్ణాంకం cmd, గుర్తులేని దీర్ఘ arg)
{
	పూర్ణాంకం ఫలితం = OK;

	ఎంపిక (cmd) {
	స్థితి LED_ON:
		led_on(arg);
		ఆపు;

	స్థితి LED_OFF:
		led_off(arg);
		ఆపు;

	స్థితి LED_TOGGLE:
		led_toggle(arg);
		ఆపు;

	అప్రమేయ:
		ఫలితం = CDev::ioctl(filp, cmd, arg);
	}

	తిరిగి ఫలితం;
}

నామాకాశ
{
LED	*gLED;
}

శూన్య
drv_led_ప్రారంభం(శూన్య)
{
	ఒకవేళ (gLED == శూన్యసూచిక) {
		gLED = కొత్త LED;
	}
}
