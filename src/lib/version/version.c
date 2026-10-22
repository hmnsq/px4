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

#شامل "version.h"

#شامل "build_git_version.h"

#شامل <string.h>

#اگر !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#اگر_ختم

#اگر defined(__PX4_LINUX)
#شامل <sys/utsname.h>
#اگر_ختم

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
شمار FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

ثابت حرف *px4_build_uri(خالی)
{
	واپس STRINGIFY(BUILD_URI);
}

uint32_t نسخہ_لیبل_مقصد_نمبر(ثابت حرف *لیبل)
{
	uint32_t نسخہ_نمبر = 0;

	int16_t بفر_2 = -1;
	size_t بفر_شمار = 0;
	size_t dash_گنتی = 0;
	size_t نقطہ_گنتی = 0;
	حرف نسخہ[3] = {0, 0, 0};
	صحیح firmware_type = FIRMWARE_TYPE_RELEASE;

	برائے (size_t i = 0; i < لڑی_لمبائی(لیبل); i++) {
		انتخاب (لیبل[i]) {
		حالت '-':
			dash_گنتی++;
			توڑو;

		حالت '.':
			نقطہ_گنتی++;
			توڑو;

		حالت 'r':
			اگر (i < لڑی_لمبائی(لیبل) - 1 && لیبل[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			توڑو;

		حالت 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			توڑو;

		حالت 't':
			اگر (i < لڑی_لمبائی(لیبل) - 1 && لیبل[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ورنہ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			توڑو;

		حالت 'v':
			اگر (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			توڑو;

		طےشدہ:
			توڑو;
		}
	}

	اگر ((dash_گنتی == 1 && نقطہ_گنتی == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_گنتی == 2 && نقطہ_گنتی == 2) ||
	    (dash_گنتی == 3 && نقطہ_گنتی == 4) ||
	    (dash_گنتی == 4 && نقطہ_گنتی == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	برائے (size_t i = 0; i < لڑی_لمبائی(لیبل); i++) {
		اگر (بفر_شمار > 2) {
			جاری;
		}

		اگر (لیبل[i] >= '0' && لیبل[i] <= '9') {
			بفر_2 = (بفر_2 == -1) ? 0 : بفر_2;
			بفر_2 = بفر_2 * 10 + (لیبل[i] - '0');

		} ورنہ {
			اگر (بفر_2 >= 0) {
				نسخہ[بفر_شمار] = بفر_2;
				بفر_شمار++;
			}

			بفر_2 = -1;
		}
	}

	اگر (بفر_2 >= 0) {
		نسخہ[بفر_شمار] = بفر_2;
		بفر_شمار++;
	}

	اگر (بفر_شمار <= 0) {
		firmware_type = 0x00;
	}

	اگر (بفر_شمار == 3 || بفر_شمار == 6) {
		نسخہ_نمبر = ((uint8_t)نسخہ[0] << 8 * 3) |
				 ((uint8_t)نسخہ[1] << 8 * 2) |
				 ((uint8_t)نسخہ[2] << 8 * 1) | firmware_type;

	} ورنہ {
		نسخہ_نمبر = 0;
	}

	واپس نسخہ_نمبر;
}

uint32_t px4_firmware_نسخہ(خالی)
{
	واپس نسخہ_لیبل_مقصد_نمبر(PX4_GIT_لیبل_STR);
}

uint32_t نسخہ_لیبل_مقصد_vendor_نسخہ_نمبر(ثابت حرف *لیبل)
{
	uint32_t نسخہ_نمبر = 0;

	int16_t بفر_2 = -1;
	size_t بفر_شمار = 0;
	حرف نسخہ[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_گنتی = 0;
	size_t نقطہ_گنتی = 0;
	صحیح firmware_type = FIRMWARE_TYPE_RELEASE;

	برائے (size_t i = 0; i < لڑی_لمبائی(لیبل); i++) {
		انتخاب (لیبل[i]) {
		حالت '-':
			dash_گنتی++;
			توڑو;

		حالت '.':
			نقطہ_گنتی++;
			توڑو;

		حالت 'r':
			اگر (i < لڑی_لمبائی(لیبل) - 1 && لیبل[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			توڑو;

		حالت 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			توڑو;

		حالت 't':
			اگر (i < لڑی_لمبائی(لیبل) - 1 && لیبل[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ورنہ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			توڑو;

		حالت 'v':
			اگر (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			توڑو;

		طےشدہ:
			توڑو;
		}
	}

	اگر ((dash_گنتی == 1 && نقطہ_گنتی == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_گنتی == 2 && نقطہ_گنتی == 2) ||
	    (dash_گنتی == 3 && نقطہ_گنتی == 4) ||
	    (dash_گنتی == 4 && نقطہ_گنتی == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	برائے (size_t i = 0; i < لڑی_لمبائی(لیبل); i++) {
		اگر (بفر_شمار > 5) {
			جاری;
		}

		اگر (لیبل[i] >= '0' && لیبل[i] <= '9') {
			بفر_2 = (بفر_2 == -1) ? 0 : بفر_2;
			بفر_2 = بفر_2 * 10 + (لیبل[i] - '0');

		} ورنہ {
			اگر (بفر_2 >= 0) {
				اگر (بفر_شمار + 1 == 4 && لیبل[i] == '-') {
					توڑو;
				}

				نسخہ[بفر_شمار] = بفر_2;
				بفر_شمار++;
			}

			بفر_2 = -1;
		}
	}

	اگر (بفر_2 >= 0 && (بفر_شمار + 1 == 3 || بفر_شمار + 1 == 6)) {
		نسخہ[بفر_شمار] = بفر_2;
		بفر_شمار++;
	}

	اگر (بفر_شمار == 6) {
		نسخہ_نمبر = ((uint8_t)نسخہ[3] << 8 * 3) |
				 ((uint8_t)نسخہ[4] << 8 * 2) |
				 ((uint8_t)نسخہ[5] << 8 * 1) | firmware_type;

	} ورنہ اگر (بفر_شمار == 3) {
		نسخہ_نمبر = firmware_type;

	} ورنہ {
		نسخہ_نمبر = 0;
	}

	واپس نسخہ_نمبر;
}

uint32_t px4_firmware_vendor_نسخہ(خالی)
{
	واپس نسخہ_لیبل_مقصد_vendor_نسخہ_نمبر(PX4_GIT_لیبل_STR);
}

ثابت حرف *px4_firmware_git_branch(خالی)
{
	واپس PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_نسخہ(خالی)
{
#اگر defined(__PX4_NUTTX)
	واپس CONFIG_CDCACM_PRODUCTID;
#ورنہ
	واپس 1;
#اگر_ختم
}

uint32_t px4_os_نسخہ(خالی)
{
#اگر defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	واپس 0; //TODO: implement version for Darwin, Cygwin, QuRT
#وگرنہ_اگر defined(__PX4_LINUX)
	ساخت utsname name;

	اگر (uname(&name) == 0) {
		حرف *c = name.release;

		// cut the part after the first '-'
		جب (*c && *c != '-') {
			++c;
		}

		*c = 0;
		واپس نسخہ_لیبل_مقصد_نمبر(name.release);

	} ورنہ {
		واپس 0;
	}

#وگرنہ_اگر defined(__PX4_NUTTX)
	واپس نسخہ_لیبل_مقصد_نمبر(NUTTX_GIT_لیبل_STR);
#ورنہ
# error "px4_os_version not implemented for current OS"
#اگر_ختم
}

ثابت حرف *px4_os_نسخہ_string(خالی)
{
#اگر defined(__PX4_NUTTX)
	واپس NUTTX_GIT_نسخہ_STR;
#ورنہ
	واپس تہی;
#اگر_ختم
}

ثابت حرف *px4_os_name(خالی)
{
#اگر defined(__PX4_DARWIN)
	واپس "Darwin";
#وگرنہ_اگر defined(__PX4_LINUX)
	واپس "Linux";
#وگرنہ_اگر defined(__PX4_QURT)
	واپس "QuRT";
#وگرنہ_اگر defined(__PX4_NUTTX)
	واپس "NuttX";
#وگرنہ_اگر defined(__PX4_CYGWIN)
	واپس "Cygwin";
#ورنہ
# error "px4_os_name not implemented for current OS"
#اگر_ختم
}

ثابت حرف *px4_toolchain_name(خالی)
{
#اگر defined(__clang__)
	واپس "Clang/LLVM";
#وگرنہ_اگر defined(__ICC) || defined(__INTEL_COMPILER)
	واپس "Intel ICC";
#وگرنہ_اگر defined(__GNUC__) || defined(__GNUG__)
	واپس "GNU GCC";
#وگرنہ_اگر defined(_MSC_VER)
	واپس "MS Visual Studio";
#ورنہ
	واپس "Unknown";
#اگر_ختم
}

ثابت حرف *px4_toolchain_نسخہ(خالی)
{
#اگر_تعریف_شدہ __VERSION__
	واپس __VERSION__;
#ورنہ
	واپس "";
#اگر_ختم
}

ثابت حرف *px4_firmware_نسخہ_string(خالی)
{
	واپس PX4_GIT_نسخہ_STR;
}

uint64_t px4_firmware_نسخہ_ثنائی(خالی)
{
	واپس PX4_GIT_نسخہ_ثنائی;
}


#اگر_تعریف_شدہ MAVLINK_LIB_GIT_نسخہ_ثنائی
uint64_t px4_mavlink_lib_نسخہ_ثنائی(خالی)
{
	واپس MAVLINK_LIB_GIT_نسخہ_ثنائی;
}
#اگر_ختم /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_نسخہ_ثنائی(خالی)
{
#اگر_تعریف_شدہ NUTTX_GIT_نسخہ_ثنائی
	واپس NUTTX_GIT_نسخہ_ثنائی;
#ورنہ
	واپس 0;
#اگر_ختم
}

ثابت حرف *px4_firmware_oem_نسخہ_string(خالی)
{
	واپس PX4_GIT_OEM_نسخہ_STR;
}
