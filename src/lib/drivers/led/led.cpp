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

#ضمن <px4_platform_common/log.h>
#ضمن <px4_platform_common/px4_config.h>
#ضمن <lib/cdev/CDev.hpp>
#ضمن <drivers/drv_board_led.h>
#ضمن <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
خارجي فراغ led_init();
خارجي فراغ led_on(صحيح led);
خارجي فراغ led_off(صحيح led);
خارجي فراغ led_toggle(صحيح led);
__END_DECLS

صنف LED : cdev::CDev
{
عام:
	LED();
	~LED() override = افتراضي;

	صحيح	init() override;
	صحيح	ioctl(cdev::file_t *filp, صحيح cmd, بنانشان ڊگهو arg) override;
};

LED::LED() : CDev(LED0_DEVICE_رستو)
{
	// force immediate init/device registration
	init();
}

صحيح
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	أرجع 0;
}

صحيح
LED::ioctl(cdev::file_t *filp, صحيح cmd, بنانشان ڊگهو arg)
{
	صحيح نتيجو = OK;

	بدل (cmd) {
	حالة LED_ON:
		led_on(arg);
		اكسر;

	حالة LED_OFF:
		led_off(arg);
		اكسر;

	حالة LED_TOGGLE:
		led_toggle(arg);
		اكسر;

	افتراضي:
		نتيجو = CDev::ioctl(filp, cmd, arg);
	}

	أرجع نتيجو;
}

نطاق
{
LED	*gLED;
}

فراغ
drv_led_شروعات(فراغ)
{
	إذا (gLED == خالي_اشارو) {
		gLED = جديد LED;
	}
}
