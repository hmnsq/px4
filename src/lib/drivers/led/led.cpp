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

#অন্তর্ভাব <px4_platform_common/log.h>
#অন্তর্ভাব <px4_platform_common/px4_config.h>
#অন্তর্ভাব <lib/cdev/CDev.hpp>
#অন্তর্ভাব <drivers/drv_board_led.h>
#অন্তর্ভাব <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
বাহ্যিক শূন্য led_init();
বাহ্যিক শূন্য led_on(পূর্ণসংখ্যা led);
বাহ্যিক শূন্য led_off(পূর্ণসংখ্যা led);
বাহ্যিক শূন্য led_toggle(পূর্ণসংখ্যা led);
__END_DECLS

শ্রেণী LED : cdev::CDev
{
সর্বজনীন:
	LED();
	~LED() override = পূর্বনির্ধারিত;

	পূর্ণসংখ্যা	init() override;
	পূর্ণসংখ্যা	ioctl(cdev::file_t *filp, পূর্ণসংখ্যা cmd, unsigned long arg) override;
};

LED::LED() : CDev(LED0_DEVICE_PATH)
{
	// force immediate init/device registration
	init();
}

পূর্ণসংখ্যা
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	ফেরত 0;
}

পূর্ণসংখ্যা
LED::ioctl(cdev::file_t *filp, পূর্ণসংখ্যা cmd, unsigned long arg)
{
	পূর্ণসংখ্যা ফলাফল = OK;

	নির্বাচন (cmd) {
	ক্ষেত্র LED_ON:
		led_on(arg);
		বিরতি;

	ক্ষেত্র LED_OFF:
		led_off(arg);
		বিরতি;

	ক্ষেত্র LED_TOGGLE:
		led_toggle(arg);
		বিরতি;

	পূর্বনির্ধারিত:
		ফলাফল = CDev::ioctl(filp, cmd, arg);
	}

	ফেরত ফলাফল;
}

নামাকাশ
{
LED	*gLED;
}

শূন্য
drv_led_start(শূন্য)
{
	যদি (gLED == nullptr) {
		gLED = নতুন LED;
	}
}
