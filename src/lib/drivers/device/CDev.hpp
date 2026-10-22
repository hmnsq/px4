/****************************************************************************
 *
 *   Copyright (c) 2012-2017 PX4 Development Team. All rights reserved.
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
 * @file CDev.hpp
 *
 * Definitions for the generic base classes in the device framework.
 */

#ifndef _DEVICE_CDEV_HPP
#define _DEVICE_CDEV_HPP

#ଅନ୍ତର୍ଭାଵ "Device.hpp"
#ଅନ୍ତର୍ଭାଵ <lib/cdev/CDev.hpp>

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_config.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/posix.h>

/**
 * Namespace encapsulating all device framework classes, functions and data.
 */
ନାମାକାଶ device
{

using file_t = cdev::file_t;

/**
 * Abstract class for any character device
 */
ଶ୍ରେଣୀ __EXPORT CDev : ସର୍ବସାଧାରଣ Device, ସର୍ବସାଧାରଣ cdev::CDev
{
ସର୍ବସାଧାରଣ:
	/**
	 * Constructor
	 *
	 * @param name		Driver name
	 * @param devname	Device node name
	 */
	CDev(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *devname); // TODO: dagar remove name and Device inheritance

	ଆଭାସୀ ~CDev() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	init();

	/**
	 * Perform an ioctl operation on the device.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @param cmd		The ioctl command value.
	 * @param arg		The ioctl argument value.
	 * @return		OK on success, or -errno otherwise.
	 */
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ioctl(file_t *filep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, unsigned long arg) { ଫେରନ୍ତୁ -ENOTTY; }

};

} // namespace device

// class instance for primary driver of each class
ଗଣନା CLASS_DEVICE {
	CLASS_DEVICE_PRIMARY = 0,
	CLASS_DEVICE_SECONDARY = 1,
	CLASS_DEVICE_TERTIARY = 2
};

#endif /* _DEVICE_CDEV_HPP */
