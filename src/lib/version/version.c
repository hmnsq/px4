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

#ܐܢ !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#ܐܢ defined(__PX4_LINUX)
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
	ܦܢܐ STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_number(ثابت حرف *tag)
{
	uint32_t version_number = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_count = 0;
	size_t point_count = 0;
	حرف version[3] = {0, 0, 0};
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < strlen(tag); i++) {
		بدل (tag[i]) {
		حالة '-':
			dash_count++;
			اكسر;

		حالة '.':
			point_count++;
			اكسر;

		حالة 'r':
			ܐܢ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			ܐܢ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ܐܚܪܢܐ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			اكسر;

		حالة 'v':
			ܐܢ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			اكسر;

		افتراضي:
			اكسر;
		}
	}

	ܐܢ ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_count == 2 && point_count == 2) ||
	    (dash_count == 3 && point_count == 4) ||
	    (dash_count == 4 && point_count == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < strlen(tag); i++) {
		ܐܢ (buffer_counter > 2) {
			استمر;
		}

		ܐܢ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} ܐܚܪܢܐ {
			ܐܢ (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	ܐܢ (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	ܐܢ (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	ܐܢ (buffer_counter == 3 || buffer_counter == 6) {
		version_number = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} ܐܚܪܢܐ {
		version_number = 0;
	}

	ܦܢܐ version_number;
}

uint32_t px4_firmware_version(فراغ)
{
	ܦܢܐ version_tag_to_number(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_number(ثابت حرف *tag)
{
	uint32_t version_number = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	حرف version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_count = 0;
	size_t point_count = 0;
	صحيح firmware_type = FIRMWARE_TYPE_RELEASE;

	لكل (size_t i = 0; i < strlen(tag); i++) {
		بدل (tag[i]) {
		حالة '-':
			dash_count++;
			اكسر;

		حالة '.':
			point_count++;
			اكسر;

		حالة 'r':
			ܐܢ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			ܐܢ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ܐܚܪܢܐ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			اكسر;

		حالة 'v':
			ܐܢ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			اكسر;

		افتراضي:
			اكسر;
		}
	}

	ܐܢ ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_count == 2 && point_count == 2) ||
	    (dash_count == 3 && point_count == 4) ||
	    (dash_count == 4 && point_count == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < strlen(tag); i++) {
		ܐܢ (buffer_counter > 5) {
			استمر;
		}

		ܐܢ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} ܐܚܪܢܐ {
			ܐܢ (buffer >= 0) {
				ܐܢ (buffer_counter + 1 == 4 && tag[i] == '-') {
					اكسر;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	ܐܢ (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	ܐܢ (buffer_counter == 6) {
		version_number = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} ܐܚܪܢܐ ܐܢ (buffer_counter == 3) {
		version_number = firmware_type;

	} ܐܚܪܢܐ {
		version_number = 0;
	}

	ܦܢܐ version_number;
}

uint32_t px4_firmware_vendor_version(فراغ)
{
	ܦܢܐ version_tag_to_vendor_version_number(PX4_GIT_TAG_STR);
}

ثابت حرف *px4_firmware_git_branch(فراغ)
{
	ܦܢܐ PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(فراغ)
{
#ܐܢ defined(__PX4_NUTTX)
	ܦܢܐ CONFIG_CDCACM_PRODUCTID;
#ܐܚܪܢܐ
	ܦܢܐ 1;
#endif
}

uint32_t px4_os_version(فراغ)
{
#ܐܢ defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	ܦܢܐ 0; //TODO: implement version for Darwin, Cygwin, QuRT
#وإلاإذا defined(__PX4_LINUX)
	بنية utsname name;

	ܐܢ (uname(&name) == 0) {
		حرف *c = name.release;

		// cut the part after the first '-'
		ܟܡܐ (*c && *c != '-') {
			++c;
		}

		*c = 0;
		ܦܢܐ version_tag_to_number(name.release);

	} ܐܚܪܢܐ {
		ܦܢܐ 0;
	}

#وإلاإذا defined(__PX4_NUTTX)
	ܦܢܐ version_tag_to_number(NUTTX_GIT_TAG_STR);
#ܐܚܪܢܐ
# error "px4_os_version not implemented for current OS"
#endif
}

ثابت حرف *px4_os_version_string(فراغ)
{
#ܐܢ defined(__PX4_NUTTX)
	ܦܢܐ NUTTX_GIT_VERSION_STR;
#ܐܚܪܢܐ
	ܦܢܐ عدم;
#endif
}

ثابت حرف *px4_os_name(فراغ)
{
#ܐܢ defined(__PX4_DARWIN)
	ܦܢܐ "Darwin";
#وإلاإذا defined(__PX4_LINUX)
	ܦܢܐ "Linux";
#وإلاإذا defined(__PX4_QURT)
	ܦܢܐ "QuRT";
#وإلاإذا defined(__PX4_NUTTX)
	ܦܢܐ "NuttX";
#وإلاإذا defined(__PX4_CYGWIN)
	ܦܢܐ "Cygwin";
#ܐܚܪܢܐ
# error "px4_os_name not implemented for current OS"
#endif
}

ثابت حرف *px4_toolchain_name(فراغ)
{
#ܐܢ defined(__clang__)
	ܦܢܐ "Clang/LLVM";
#وإلاإذا defined(__ICC) || defined(__INTEL_COMPILER)
	ܦܢܐ "Intel ICC";
#وإلاإذا defined(__GNUC__) || defined(__GNUG__)
	ܦܢܐ "GNU GCC";
#وإلاإذا defined(_MSC_VER)
	ܦܢܐ "MS Visual Studio";
#ܐܚܪܢܐ
	ܦܢܐ "Unknown";
#endif
}

ثابت حرف *px4_toolchain_version(فراغ)
{
#ifdef __VERSION__
	ܦܢܐ __VERSION__;
#ܐܚܪܢܐ
	ܦܢܐ "";
#endif
}

ثابت حرف *px4_firmware_version_string(فراغ)
{
	ܦܢܐ PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_binary(فراغ)
{
	ܦܢܐ PX4_GIT_VERSION_BINARY;
}


#ifdef MAVLINK_LIB_GIT_VERSION_BINARY
uint64_t px4_mavlink_lib_version_binary(فراغ)
{
	ܦܢܐ MAVLINK_LIB_GIT_VERSION_BINARY;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_binary(فراغ)
{
#ifdef NUTTX_GIT_VERSION_BINARY
	ܦܢܐ NUTTX_GIT_VERSION_BINARY;
#ܐܚܪܢܐ
	ܦܢܐ 0;
#endif
}

ثابت حرف *px4_firmware_oem_version_string(فراغ)
{
	ܦܢܐ PX4_GIT_OEM_VERSION_STR;
}
