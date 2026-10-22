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

#अन्तर्भाव <px4_platform_common/log.h>
#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <lib/cdev/CDev.hpp>
#अन्तर्भाव <drivers/drv_board_led.h>
#अन्तर्भाव <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
बाह्य शून्य led_init();
बाह्य शून्य led_on(पूर्णाङ्क led);
बाह्य शून्य led_off(पूर्णाङ्क led);
बाह्य शून्य led_toggle(पूर्णाङ्क led);
__END_DECLS

वग्ग LED : cdev::CDev
{
सार्वजनिक:
	LED();
	~LED() override = स्वभाव;

	पूर्णाङ्क	init() override;
	पूर्णाङ्क	ioctl(cdev::file_t *filp, पूर्णाङ्क cmd, unsigned long arg) override;
};

LED::LED() : CDev(LED0_DEVICE_PATH)
{
	// force immediate init/device registration
	init();
}

पूर्णाङ्क
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	पटिनिवत्त 0;
}

पूर्णाङ्क
LED::ioctl(cdev::file_t *filp, पूर्णाङ्क cmd, unsigned long arg)
{
	पूर्णाङ्क result = OK;

	विकल्प (cmd) {
	पक्ष LED_ON:
		led_on(arg);
		भङ्ग;

	पक्ष LED_OFF:
		led_off(arg);
		भङ्ग;

	पक्ष LED_TOGGLE:
		led_toggle(arg);
		भङ्ग;

	स्वभाव:
		result = CDev::ioctl(filp, cmd, arg);
	}

	पटिनिवत्त result;
}

नामाकाश
{
LED	*gLED;
}

शून्य
drv_led_start(शून्य)
{
	सचे (gLED == nullptr) {
		gLED = नूतन LED;
	}
}
