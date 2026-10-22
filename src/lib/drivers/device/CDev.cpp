/****************************************************************************
 *
 *   Copyright (c) 2012-2014 PX4 Development Team. All rights reserved.
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
 * @file CDev.cpp
 *
 * Character device base class.
 */

#ଅନ୍ତର୍ଭାଵ "CDev.hpp"

#ଅନ୍ତର୍ଭାଵ <cstring>

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/posix.h>

ନାମାକାଶ device
{

CDev::CDev(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *devname) :
	Device(name),
	cdev::CDev(devname)
{
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା CDev::init()
{
	PX4_DEBUG("CDev::init");

	// base class init first
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = Device::init();

	ଯଦି (ret != PX4_OK) {
		ଯାଅ out;
	}

	// now register the driver
	ଯଦି (get_devname() != nullptr) {
		ret = cdev::CDev::init();

		ଯଦି (ret != PX4_OK) {
			ଯାଅ out;
		}
	}

out:
	ଫେରନ୍ତୁ ret;
}

} // namespace device
