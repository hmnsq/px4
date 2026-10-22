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
#endif

#إذا defined(__PX4_LINUX)
#ضمن <sys/utsname.h>
#endif

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

uint32_t version_tag_to_نمبر(ثابت حرف *tag)
{
	uint32_t version_نمبر = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_گنتی = 0;
	size_t point_گنتی = 0;
	حرف version[3] = {0, 0, 0};
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < strlen(tag); i++) {
		بدل (tag[i]) {
		حالة '-':
			dash_گنتی++;
			اكسر;

		حالة '.':
			point_گنتی++;
			اكسر;

		حالة 'r':
			إذا (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			إذا (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
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

	إذا ((dash_گنتی == 1 && point_گنتی == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_گنتی == 2 && point_گنتی == 2) ||
	    (dash_گنتی == 3 && point_گنتی == 4) ||
	    (dash_گنتی == 4 && point_گنتی == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < strlen(tag); i++) {
		إذا (buffer_counter > 2) {
			استمر;
		}

		إذا (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} وإلا {
			إذا (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	إذا (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	إذا (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	إذا (buffer_counter == 3 || buffer_counter == 6) {
		version_نمبر = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} وإلا {
		version_نمبر = 0;
	}

	أرجع version_نمبر;
}

uint32_t px4_firmware_version(فراغ)
{
	أرجع version_tag_to_نمبر(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_نمبر(ثابت حرف *tag)
{
	uint32_t version_نمبر = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	حرف version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_گنتی = 0;
	size_t point_گنتی = 0;
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < strlen(tag); i++) {
		بدل (tag[i]) {
		حالة '-':
			dash_گنتی++;
			اكسر;

		حالة '.':
			point_گنتی++;
			اكسر;

		حالة 'r':
			إذا (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			إذا (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
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

	إذا ((dash_گنتی == 1 && point_گنتی == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_گنتی == 2 && point_گنتی == 2) ||
	    (dash_گنتی == 3 && point_گنتی == 4) ||
	    (dash_گنتی == 4 && point_گنتی == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < strlen(tag); i++) {
		إذا (buffer_counter > 5) {
			استمر;
		}

		إذا (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} وإلا {
			إذا (buffer >= 0) {
				إذا (buffer_counter + 1 == 4 && tag[i] == '-') {
					اكسر;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	إذا (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	إذا (buffer_counter == 6) {
		version_نمبر = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} وإلا إذا (buffer_counter == 3) {
		version_نمبر = firmware_type;

	} وإلا {
		version_نمبر = 0;
	}

	أرجع version_نمبر;
}

uint32_t px4_firmware_vendor_version(فراغ)
{
	أرجع version_tag_to_vendor_version_نمبر(PX4_GIT_TAG_STR);
}

ثابت حرف *px4_firmware_git_branch(فراغ)
{
	أرجع PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(فراغ)
{
#إذا defined(__PX4_NUTTX)
	أرجع CONFIG_CDCACM_PRODUCTID;
#وإلا
	أرجع 1;
#endif
}

uint32_t px4_os_version(فراغ)
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
		أرجع version_tag_to_نمبر(name.release);

	} وإلا {
		أرجع 0;
	}

#وإلاإذا defined(__PX4_NUTTX)
	أرجع version_tag_to_نمبر(NUTTX_GIT_TAG_STR);
#وإلا
# error "px4_os_version not implemented for current OS"
#endif
}

ثابت حرف *px4_os_version_string(فراغ)
{
#إذا defined(__PX4_NUTTX)
	أرجع NUTTX_GIT_VERSION_STR;
#وإلا
	أرجع عدم;
#endif
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
#endif
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
#endif
}

ثابت حرف *px4_toolchain_version(فراغ)
{
#ifdef __VERSION__
	أرجع __VERSION__;
#وإلا
	أرجع "";
#endif
}

ثابت حرف *px4_firmware_version_string(فراغ)
{
	أرجع PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_ثنائی(فراغ)
{
	أرجع PX4_GIT_VERSION_ثنائی;
}


#ifdef MAVLINK_LIB_GIT_VERSION_ثنائی
uint64_t px4_mavlink_lib_version_ثنائی(فراغ)
{
	أرجع MAVLINK_LIB_GIT_VERSION_ثنائی;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_ثنائی(فراغ)
{
#ifdef NUTTX_GIT_VERSION_ثنائی
	أرجع NUTTX_GIT_VERSION_ثنائی;
#وإلا
	أرجع 0;
#endif
}

ثابت حرف *px4_firmware_oem_version_string(فراغ)
{
	أرجع PX4_GIT_OEM_VERSION_STR;
}
