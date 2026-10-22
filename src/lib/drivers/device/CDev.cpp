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

#അന്തര്ഭാവ "CDev.hpp"

#അന്തര്ഭാവ <cstring>

#അന്തര്ഭാവ <px4_platform_common/log.h>
#അന്തര്ഭാവ <px4_platform_common/posix.h>

നാമാകാശ device
{

CDev::CDev(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *devname) :
	Device(name),
	cdev::CDev(devname)
{
}

പൂർണ്ണസംഖ്യ CDev::init()
{
	PX4_DEBUG("CDev::init");

	// base class init first
	പൂർണ്ണസംഖ്യ ret = Device::init();

	എങ്കിൽ (ret != PX4_OK) {
		പോകുക out;
	}

	// now register the driver
	എങ്കിൽ (get_devname() != nullptr) {
		ret = cdev::CDev::init();

		എങ്കിൽ (ret != PX4_OK) {
			പോകുക out;
		}
	}

out:
	തിരികെ ret;
}

} // namespace device
