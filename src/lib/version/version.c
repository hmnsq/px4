/****************************************************************************
 *
 *   Copyright (c) 2016 PX4 Development Team. All rights reserved.
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

#ضمن "version.h"

#ضمن "build_git_version.h"

#ضمن <string.h>

#إذا !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#پایان_اگر

#إذا defined(__PX4_LINUX)
#ضمن <sys/utsname.h>
#پایان_اگر

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
تعداد FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

ثابت حرف *px4_build_uri(فراغ)
{
	أرجع STRINGIFY(BUILD_URI);
}

uint32_t نسخه_برچسب_مقصد_عدد(ثابت حرف *برچسب)
{
	uint32_t نسخه_عدد = 0;

	int16_t میانگیر_2 = -1;
	size_t میانگیر_counter = 0;
	size_t dash_شمار = 0;
	size_t نقطه_شمار = 0;
	حرف نسخه[3] = {0, 0, 0};
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < طول_رشته(برچسب); i++) {
		بدل (برچسب[i]) {
		حالة '-':
			dash_شمار++;
			اكسر;

		حالة '.':
			نقطه_شمار++;
			اكسر;

		حالة 'r':
			إذا (i < طول_رشته(برچسب) - 1 && برچسب[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			إذا (i < طول_رشته(برچسب) - 1 && برچسب[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} وإلا {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			اكسر;

		حالة 'v':
			إذا (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			اكسر;

		افتراضي:
			اكسر;
		}
	}

	إذا ((dash_شمار == 1 && نقطه_شمار == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_شمار == 2 && نقطه_شمار == 2) ||
	    (dash_شمار == 3 && نقطه_شمار == 4) ||
	    (dash_شمار == 4 && نقطه_شمار == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < طول_رشته(برچسب); i++) {
		إذا (میانگیر_counter > 2) {
			استمر;
		}

		إذا (برچسب[i] >= '0' && برچسب[i] <= '9') {
			میانگیر_2 = (میانگیر_2 == -1) ? 0 : میانگیر_2;
			میانگیر_2 = میانگیر_2 * 10 + (برچسب[i] - '0');

		} وإلا {
			إذا (میانگیر_2 >= 0) {
				نسخه[میانگیر_counter] = میانگیر_2;
				میانگیر_counter++;
			}

			میانگیر_2 = -1;
		}
	}

	إذا (میانگیر_2 >= 0) {
		نسخه[میانگیر_counter] = میانگیر_2;
		میانگیر_counter++;
	}

	إذا (میانگیر_counter <= 0) {
		firmware_type = 0x00;
	}

	إذا (میانگیر_counter == 3 || میانگیر_counter == 6) {
		نسخه_عدد = ((uint8_t)نسخه[0] << 8 * 3) |
				 ((uint8_t)نسخه[1] << 8 * 2) |
				 ((uint8_t)نسخه[2] << 8 * 1) | firmware_type;

	} وإلا {
		نسخه_عدد = 0;
	}

	أرجع نسخه_عدد;
}

uint32_t px4_firmware_نسخه(فراغ)
{
	أرجع نسخه_برچسب_مقصد_عدد(PX4_GIT_برچسب_STR);
}

uint32_t نسخه_برچسب_مقصد_vendor_نسخه_عدد(ثابت حرف *برچسب)
{
	uint32_t نسخه_عدد = 0;

	int16_t میانگیر_2 = -1;
	size_t میانگیر_counter = 0;
	حرف نسخه[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_شمار = 0;
	size_t نقطه_شمار = 0;
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < طول_رشته(برچسب); i++) {
		بدل (برچسب[i]) {
		حالة '-':
			dash_شمار++;
			اكسر;

		حالة '.':
			نقطه_شمار++;
			اكسر;

		حالة 'r':
			إذا (i < طول_رشته(برچسب) - 1 && برچسب[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			إذا (i < طول_رشته(برچسب) - 1 && برچسب[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} وإلا {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			اكسر;

		حالة 'v':
			إذا (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			اكسر;

		افتراضي:
			اكسر;
		}
	}

	إذا ((dash_شمار == 1 && نقطه_شمار == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_شمار == 2 && نقطه_شمار == 2) ||
	    (dash_شمار == 3 && نقطه_شمار == 4) ||
	    (dash_شمار == 4 && نقطه_شمار == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < طول_رشته(برچسب); i++) {
		إذا (میانگیر_counter > 5) {
			استمر;
		}

		إذا (برچسب[i] >= '0' && برچسب[i] <= '9') {
			میانگیر_2 = (میانگیر_2 == -1) ? 0 : میانگیر_2;
			میانگیر_2 = میانگیر_2 * 10 + (برچسب[i] - '0');

		} وإلا {
			إذا (میانگیر_2 >= 0) {
				إذا (میانگیر_counter + 1 == 4 && برچسب[i] == '-') {
					اكسر;
				}

				نسخه[میانگیر_counter] = میانگیر_2;
				میانگیر_counter++;
			}

			میانگیر_2 = -1;
		}
	}

	إذا (میانگیر_2 >= 0 && (میانگیر_counter + 1 == 3 || میانگیر_counter + 1 == 6)) {
		نسخه[میانگیر_counter] = میانگیر_2;
		میانگیر_counter++;
	}

	إذا (میانگیر_counter == 6) {
		نسخه_عدد = ((uint8_t)نسخه[3] << 8 * 3) |
				 ((uint8_t)نسخه[4] << 8 * 2) |
				 ((uint8_t)نسخه[5] << 8 * 1) | firmware_type;

	} وإلا إذا (میانگیر_counter == 3) {
		نسخه_عدد = firmware_type;

	} وإلا {
		نسخه_عدد = 0;
	}

	أرجع نسخه_عدد;
}

uint32_t px4_firmware_vendor_نسخه(فراغ)
{
	أرجع نسخه_برچسب_مقصد_vendor_نسخه_عدد(PX4_GIT_برچسب_STR);
}

ثابت حرف *px4_firmware_git_branch(فراغ)
{
	أرجع PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_نسخه(فراغ)
{
#إذا defined(__PX4_NUTTX)
	أرجع CONFIG_CDCACM_PRODUCTID;
#وإلا
	أرجع 1;
#پایان_اگر
}

uint32_t px4_os_نسخه(فراغ)
{
#إذا defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	أرجع 0; //TODO: implement version for Darwin, Cygwin, QuRT
#وإلاإذا defined(__PX4_LINUX)
	بنية utsname name;

	إذا (uname(&name) == 0) {
		حرف *c = name.release;

		// cut the part after the first '-'
		طالما (*c && *c != '-') {
			++c;
		}

		*c = 0;
		أرجع نسخه_برچسب_مقصد_عدد(name.release);

	} وإلا {
		أرجع 0;
	}

#وإلاإذا defined(__PX4_NUTTX)
	أرجع نسخه_برچسب_مقصد_عدد(NUTTX_GIT_برچسب_STR);
#وإلا
# error "px4_os_version not implemented for current OS"
#پایان_اگر
}

ثابت حرف *px4_os_نسخه_string(فراغ)
{
#إذا defined(__PX4_NUTTX)
	أرجع NUTTX_GIT_نسخه_STR;
#وإلا
	أرجع عدم;
#پایان_اگر
}

ثابت حرف *px4_os_name(فراغ)
{
#إذا defined(__PX4_DARWIN)
	أرجع "Darwin";
#وإلاإذا defined(__PX4_LINUX)
	أرجع "Linux";
#وإلاإذا defined(__PX4_QURT)
	أرجع "QuRT";
#وإلاإذا defined(__PX4_NUTTX)
	أرجع "NuttX";
#وإلاإذا defined(__PX4_CYGWIN)
	أرجع "Cygwin";
#وإلا
# error "px4_os_name not implemented for current OS"
#پایان_اگر
}

ثابت حرف *px4_toolchain_name(فراغ)
{
#إذا defined(__clang__)
	أرجع "Clang/LLVM";
#وإلاإذا defined(__ICC) || defined(__INTEL_COMPILER)
	أرجع "Intel ICC";
#وإلاإذا defined(__GNUC__) || defined(__GNUG__)
	أرجع "GNU GCC";
#وإلاإذا defined(_MSC_VER)
	أرجع "MS Visual Studio";
#وإلا
	أرجع "Unknown";
#پایان_اگر
}

ثابت حرف *px4_toolchain_نسخه(فراغ)
{
#اگر_تعریف_شده __VERSION__
	أرجع __VERSION__;
#وإلا
	أرجع "";
#پایان_اگر
}

ثابت حرف *px4_firmware_نسخه_string(فراغ)
{
	أرجع PX4_GIT_نسخه_STR;
}

uint64_t px4_firmware_نسخه_دودویی(فراغ)
{
	أرجع PX4_GIT_نسخه_دودویی;
}


#اگر_تعریف_شده MAVLINK_LIB_GIT_نسخه_دودویی
uint64_t px4_mavlink_lib_نسخه_دودویی(فراغ)
{
	أرجع MAVLINK_LIB_GIT_نسخه_دودویی;
}
#پایان_اگر /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_نسخه_دودویی(فراغ)
{
#اگر_تعریف_شده NUTTX_GIT_نسخه_دودویی
	أرجع NUTTX_GIT_نسخه_دودویی;
#وإلا
	أرجع 0;
#پایان_اگر
}

ثابت حرف *px4_firmware_oem_نسخه_string(فراغ)
{
	أرجع PX4_GIT_OEM_نسخه_STR;
}
