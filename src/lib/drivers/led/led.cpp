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

#અન્તર્ભાવ <px4_platform_common/log.h>
#અન્તર્ભાવ <px4_platform_common/px4_config.h>
#અન્તર્ભાવ <lib/cdev/CDev.hpp>
#અન્તર્ભાવ <drivers/drv_board_led.h>
#અન્તર્ભાવ <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
બાહ્ય શૂન્ય led_init();
બાહ્ય શૂન્ય led_on(પૂર્ણાંક led);
બાહ્ય શૂન્ય led_off(પૂર્ણાંક led);
બાહ્ય શૂન્ય led_toggle(પૂર્ણાંક led);
__END_DECLS

વર્ગ LED : cdev::CDev
{
જાહેર:
	LED();
	~LED() override = મૂળભૂત;

	પૂર્ણાંક	init() override;
	પૂર્ણાંક	ioctl(cdev::file_t *filp, પૂર્ણાંક cmd, unsigned long arg) override;
};

LED::LED() : CDev(LED0_DEVICE_PATH)
{
	// force immediate init/device registration
	init();
}

પૂર્ણાંક
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	પાછા 0;
}

પૂર્ણાંક
LED::ioctl(cdev::file_t *filp, પૂર્ણાંક cmd, unsigned long arg)
{
	પૂર્ણાંક result = OK;

	પસંદગી (cmd) {
	સ્થિતિ LED_ON:
		led_on(arg);
		તોડો;

	સ્થિતિ LED_OFF:
		led_off(arg);
		તોડો;

	સ્થિતિ LED_TOGGLE:
		led_toggle(arg);
		તોડો;

	મૂળભૂત:
		result = CDev::ioctl(filp, cmd, arg);
	}

	પાછા result;
}

નામાકાશ
{
LED	*gLED;
}

શૂન્ય
drv_led_start(શૂન્ય)
{
	જો (gLED == nullptr) {
		gLED = નવું LED;
	}
}
