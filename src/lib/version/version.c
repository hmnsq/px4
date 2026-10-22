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
#د_که_پای

#إذا defined(__PX4_LINUX)
#ضمن <sys/utsname.h>
#د_که_پای

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

uint32_t نسخه_نښان_منزل_شمېره(ثابت حرف *نښان)
{
	uint32_t نسخه_شمېره = 0;

	int16_t بفر_2 = -1;
	size_t بفر_counter = 0;
	size_t dash_شمېر = 0;
	size_t ټکی_شمېر = 0;
	حرف نسخه[3] = {0, 0, 0};
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < مزي_اوږدوالی(نښان); i++) {
		بدل (نښان[i]) {
		حالة '-':
			dash_شمېر++;
			اكسر;

		حالة '.':
			ټکی_شمېر++;
			اكسر;

		حالة 'r':
			إذا (i < مزي_اوږدوالی(نښان) - 1 && نښان[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			إذا (i < مزي_اوږدوالی(نښان) - 1 && نښان[i + 1] == 'y') {
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

	إذا ((dash_شمېر == 1 && ټکی_شمېر == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_شمېر == 2 && ټکی_شمېر == 2) ||
	    (dash_شمېر == 3 && ټکی_شمېر == 4) ||
	    (dash_شمېر == 4 && ټکی_شمېر == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < مزي_اوږدوالی(نښان); i++) {
		إذا (بفر_counter > 2) {
			استمر;
		}

		إذا (نښان[i] >= '0' && نښان[i] <= '9') {
			بفر_2 = (بفر_2 == -1) ? 0 : بفر_2;
			بفر_2 = بفر_2 * 10 + (نښان[i] - '0');

		} وإلا {
			إذا (بفر_2 >= 0) {
				نسخه[بفر_counter] = بفر_2;
				بفر_counter++;
			}

			بفر_2 = -1;
		}
	}

	إذا (بفر_2 >= 0) {
		نسخه[بفر_counter] = بفر_2;
		بفر_counter++;
	}

	إذا (بفر_counter <= 0) {
		firmware_type = 0x00;
	}

	إذا (بفر_counter == 3 || بفر_counter == 6) {
		نسخه_شمېره = ((uint8_t)نسخه[0] << 8 * 3) |
				 ((uint8_t)نسخه[1] << 8 * 2) |
				 ((uint8_t)نسخه[2] << 8 * 1) | firmware_type;

	} وإلا {
		نسخه_شمېره = 0;
	}

	أرجع نسخه_شمېره;
}

uint32_t px4_firmware_نسخه(فراغ)
{
	أرجع نسخه_نښان_منزل_شمېره(PX4_GIT_نښان_STR);
}

uint32_t نسخه_نښان_منزل_vendor_نسخه_شمېره(ثابت حرف *نښان)
{
	uint32_t نسخه_شمېره = 0;

	int16_t بفر_2 = -1;
	size_t بفر_counter = 0;
	حرف نسخه[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_شمېر = 0;
	size_t ټکی_شمېر = 0;
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < مزي_اوږدوالی(نښان); i++) {
		بدل (نښان[i]) {
		حالة '-':
			dash_شمېر++;
			اكسر;

		حالة '.':
			ټکی_شمېر++;
			اكسر;

		حالة 'r':
			إذا (i < مزي_اوږدوالی(نښان) - 1 && نښان[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			إذا (i < مزي_اوږدوالی(نښان) - 1 && نښان[i + 1] == 'y') {
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

	إذا ((dash_شمېر == 1 && ټکی_شمېر == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_شمېر == 2 && ټکی_شمېر == 2) ||
	    (dash_شمېر == 3 && ټکی_شمېر == 4) ||
	    (dash_شمېر == 4 && ټکی_شمېر == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < مزي_اوږدوالی(نښان); i++) {
		إذا (بفر_counter > 5) {
			استمر;
		}

		إذا (نښان[i] >= '0' && نښان[i] <= '9') {
			بفر_2 = (بفر_2 == -1) ? 0 : بفر_2;
			بفر_2 = بفر_2 * 10 + (نښان[i] - '0');

		} وإلا {
			إذا (بفر_2 >= 0) {
				إذا (بفر_counter + 1 == 4 && نښان[i] == '-') {
					اكسر;
				}

				نسخه[بفر_counter] = بفر_2;
				بفر_counter++;
			}

			بفر_2 = -1;
		}
	}

	إذا (بفر_2 >= 0 && (بفر_counter + 1 == 3 || بفر_counter + 1 == 6)) {
		نسخه[بفر_counter] = بفر_2;
		بفر_counter++;
	}

	إذا (بفر_counter == 6) {
		نسخه_شمېره = ((uint8_t)نسخه[3] << 8 * 3) |
				 ((uint8_t)نسخه[4] << 8 * 2) |
				 ((uint8_t)نسخه[5] << 8 * 1) | firmware_type;

	} وإلا إذا (بفر_counter == 3) {
		نسخه_شمېره = firmware_type;

	} وإلا {
		نسخه_شمېره = 0;
	}

	أرجع نسخه_شمېره;
}

uint32_t px4_firmware_vendor_نسخه(فراغ)
{
	أرجع نسخه_نښان_منزل_vendor_نسخه_شمېره(PX4_GIT_نښان_STR);
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
#د_که_پای
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
		أرجع نسخه_نښان_منزل_شمېره(name.release);

	} وإلا {
		أرجع 0;
	}

#وإلاإذا defined(__PX4_NUTTX)
	أرجع نسخه_نښان_منزل_شمېره(NUTTX_GIT_نښان_STR);
#وإلا
# error "px4_os_version not implemented for current OS"
#د_که_پای
}

ثابت حرف *px4_os_نسخه_string(فراغ)
{
#إذا defined(__PX4_NUTTX)
	أرجع NUTTX_GIT_نسخه_STR;
#وإلا
	أرجع عدم;
#د_که_پای
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
#د_که_پای
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
#د_که_پای
}

ثابت حرف *px4_toolchain_نسخه(فراغ)
{
#که_تعریف_شوی __VERSION__
	أرجع __VERSION__;
#وإلا
	أرجع "";
#د_که_پای
}

ثابت حرف *px4_firmware_نسخه_string(فراغ)
{
	أرجع PX4_GIT_نسخه_STR;
}

uint64_t px4_firmware_نسخه_دوګونی(فراغ)
{
	أرجع PX4_GIT_نسخه_دوګونی;
}


#که_تعریف_شوی MAVLINK_LIB_GIT_نسخه_دوګونی
uint64_t px4_mavlink_lib_نسخه_دوګونی(فراغ)
{
	أرجع MAVLINK_LIB_GIT_نسخه_دوګونی;
}
#د_که_پای /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_نسخه_دوګونی(فراغ)
{
#که_تعریف_شوی NUTTX_GIT_نسخه_دوګونی
	أرجع NUTTX_GIT_نسخه_دوګونی;
#وإلا
	أرجع 0;
#د_که_پای
}

ثابت حرف *px4_firmware_oem_نسخه_string(فراغ)
{
	أرجع PX4_GIT_OEM_نسخه_STR;
}
