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
#جيڪڏهن_پورو

#إذا defined(__PX4_LINUX)
#ضمن <sys/utsname.h>
#جيڪڏهن_پورو

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

uint32_t نسخو_ٽيگ_منزل_عدد(ثابت حرف *ٽيگ)
{
	uint32_t نسخو_عدد = 0;

	int16_t بفر_2 = -1;
	size_t بفر_counter = 0;
	size_t dash_ڳڻپ = 0;
	size_t نقطو_ڳڻپ = 0;
	حرف نسخو[3] = {0, 0, 0};
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < لڙي_ڊيگهه(ٽيگ); i++) {
		بدل (ٽيگ[i]) {
		حالة '-':
			dash_ڳڻپ++;
			اكسر;

		حالة '.':
			نقطو_ڳڻپ++;
			اكسر;

		حالة 'r':
			إذا (i < لڙي_ڊيگهه(ٽيگ) - 1 && ٽيگ[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			إذا (i < لڙي_ڊيگهه(ٽيگ) - 1 && ٽيگ[i + 1] == 'y') {
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

	إذا ((dash_ڳڻپ == 1 && نقطو_ڳڻپ == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ڳڻپ == 2 && نقطو_ڳڻپ == 2) ||
	    (dash_ڳڻپ == 3 && نقطو_ڳڻپ == 4) ||
	    (dash_ڳڻپ == 4 && نقطو_ڳڻپ == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < لڙي_ڊيگهه(ٽيگ); i++) {
		إذا (بفر_counter > 2) {
			استمر;
		}

		إذا (ٽيگ[i] >= '0' && ٽيگ[i] <= '9') {
			بفر_2 = (بفر_2 == -1) ? 0 : بفر_2;
			بفر_2 = بفر_2 * 10 + (ٽيگ[i] - '0');

		} وإلا {
			إذا (بفر_2 >= 0) {
				نسخو[بفر_counter] = بفر_2;
				بفر_counter++;
			}

			بفر_2 = -1;
		}
	}

	إذا (بفر_2 >= 0) {
		نسخو[بفر_counter] = بفر_2;
		بفر_counter++;
	}

	إذا (بفر_counter <= 0) {
		firmware_type = 0x00;
	}

	إذا (بفر_counter == 3 || بفر_counter == 6) {
		نسخو_عدد = ((uint8_t)نسخو[0] << 8 * 3) |
				 ((uint8_t)نسخو[1] << 8 * 2) |
				 ((uint8_t)نسخو[2] << 8 * 1) | firmware_type;

	} وإلا {
		نسخو_عدد = 0;
	}

	أرجع نسخو_عدد;
}

uint32_t px4_firmware_نسخو(فراغ)
{
	أرجع نسخو_ٽيگ_منزل_عدد(PX4_GIT_ٽيگ_STR);
}

uint32_t نسخو_ٽيگ_منزل_vendor_نسخو_عدد(ثابت حرف *ٽيگ)
{
	uint32_t نسخو_عدد = 0;

	int16_t بفر_2 = -1;
	size_t بفر_counter = 0;
	حرف نسخو[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_ڳڻپ = 0;
	size_t نقطو_ڳڻپ = 0;
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < لڙي_ڊيگهه(ٽيگ); i++) {
		بدل (ٽيگ[i]) {
		حالة '-':
			dash_ڳڻپ++;
			اكسر;

		حالة '.':
			نقطو_ڳڻپ++;
			اكسر;

		حالة 'r':
			إذا (i < لڙي_ڊيگهه(ٽيگ) - 1 && ٽيگ[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			إذا (i < لڙي_ڊيگهه(ٽيگ) - 1 && ٽيگ[i + 1] == 'y') {
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

	إذا ((dash_ڳڻپ == 1 && نقطو_ڳڻپ == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ڳڻپ == 2 && نقطو_ڳڻپ == 2) ||
	    (dash_ڳڻپ == 3 && نقطو_ڳڻپ == 4) ||
	    (dash_ڳڻپ == 4 && نقطو_ڳڻپ == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < لڙي_ڊيگهه(ٽيگ); i++) {
		إذا (بفر_counter > 5) {
			استمر;
		}

		إذا (ٽيگ[i] >= '0' && ٽيگ[i] <= '9') {
			بفر_2 = (بفر_2 == -1) ? 0 : بفر_2;
			بفر_2 = بفر_2 * 10 + (ٽيگ[i] - '0');

		} وإلا {
			إذا (بفر_2 >= 0) {
				إذا (بفر_counter + 1 == 4 && ٽيگ[i] == '-') {
					اكسر;
				}

				نسخو[بفر_counter] = بفر_2;
				بفر_counter++;
			}

			بفر_2 = -1;
		}
	}

	إذا (بفر_2 >= 0 && (بفر_counter + 1 == 3 || بفر_counter + 1 == 6)) {
		نسخو[بفر_counter] = بفر_2;
		بفر_counter++;
	}

	إذا (بفر_counter == 6) {
		نسخو_عدد = ((uint8_t)نسخو[3] << 8 * 3) |
				 ((uint8_t)نسخو[4] << 8 * 2) |
				 ((uint8_t)نسخو[5] << 8 * 1) | firmware_type;

	} وإلا إذا (بفر_counter == 3) {
		نسخو_عدد = firmware_type;

	} وإلا {
		نسخو_عدد = 0;
	}

	أرجع نسخو_عدد;
}

uint32_t px4_firmware_vendor_نسخو(فراغ)
{
	أرجع نسخو_ٽيگ_منزل_vendor_نسخو_عدد(PX4_GIT_ٽيگ_STR);
}

ثابت حرف *px4_firmware_git_branch(فراغ)
{
	أرجع PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_نسخو(فراغ)
{
#إذا defined(__PX4_NUTTX)
	أرجع CONFIG_CDCACM_PRODUCTID;
#وإلا
	أرجع 1;
#جيڪڏهن_پورو
}

uint32_t px4_os_نسخو(فراغ)
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
		أرجع نسخو_ٽيگ_منزل_عدد(name.release);

	} وإلا {
		أرجع 0;
	}

#وإلاإذا defined(__PX4_NUTTX)
	أرجع نسخو_ٽيگ_منزل_عدد(NUTTX_GIT_ٽيگ_STR);
#وإلا
# error "px4_os_version not implemented for current OS"
#جيڪڏهن_پورو
}

ثابت حرف *px4_os_نسخو_string(فراغ)
{
#إذا defined(__PX4_NUTTX)
	أرجع NUTTX_GIT_نسخو_STR;
#وإلا
	أرجع عدم;
#جيڪڏهن_پورو
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
#جيڪڏهن_پورو
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
#جيڪڏهن_پورو
}

ثابت حرف *px4_toolchain_نسخو(فراغ)
{
#جيڪڏهن_وصف_ٿيل __VERSION__
	أرجع __VERSION__;
#وإلا
	أرجع "";
#جيڪڏهن_پورو
}

ثابت حرف *px4_firmware_نسخو_string(فراغ)
{
	أرجع PX4_GIT_نسخو_STR;
}

uint64_t px4_firmware_نسخو_ٻٽو(فراغ)
{
	أرجع PX4_GIT_نسخو_ٻٽو;
}


#جيڪڏهن_وصف_ٿيل MAVLINK_LIB_GIT_نسخو_ٻٽو
uint64_t px4_mavlink_lib_نسخو_ٻٽو(فراغ)
{
	أرجع MAVLINK_LIB_GIT_نسخو_ٻٽو;
}
#جيڪڏهن_پورو /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_نسخو_ٻٽو(فراغ)
{
#جيڪڏهن_وصف_ٿيل NUTTX_GIT_نسخو_ٻٽو
	أرجع NUTTX_GIT_نسخو_ٻٽو;
#وإلا
	أرجع 0;
#جيڪڏهن_پورو
}

ثابت حرف *px4_firmware_oem_نسخو_string(فراغ)
{
	أرجع PX4_GIT_OEM_نسخو_STR;
}
