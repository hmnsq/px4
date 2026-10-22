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

#include "version.h"

#include "build_git_version.h"

#include <string.h>

#만약 !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#만약 defined(__PX4_LINUX)
#include <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
enum FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

const char *px4_build_uri(void)
{
	반환 STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_number(const char *tag)
{
	uint32_t version_number = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_count = 0;
	size_t point_count = 0;
	char version[3] = {0, 0, 0};
	정수 firmware_type = FIRMWARE_TYPE_RELEASE;

	위해 (size_t i = 0; i < strlen(tag); i++) {
		switch (tag[i]) {
		case '-':
			dash_count++;
			break;

		case '.':
			point_count++;
			break;

		case 'r':
			만약 (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			break;

		case 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			break;

		case 't':
			만약 (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} 아니면 {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			break;

		case 'v':
			만약 (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			break;

		default:
			break;
		}
	}

	만약 ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_count == 2 && point_count == 2) ||
	    (dash_count == 3 && point_count == 4) ||
	    (dash_count == 4 && point_count == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	위해 (size_t i = 0; i < strlen(tag); i++) {
		만약 (buffer_counter > 2) {
			continue;
		}

		만약 (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} 아니면 {
			만약 (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	만약 (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	만약 (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	만약 (buffer_counter == 3 || buffer_counter == 6) {
		version_number = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} 아니면 {
		version_number = 0;
	}

	반환 version_number;
}

uint32_t px4_firmware_version(void)
{
	반환 version_tag_to_number(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_number(const char *tag)
{
	uint32_t version_number = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	char version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_count = 0;
	size_t point_count = 0;
	정수 firmware_type = FIRMWARE_TYPE_RELEASE;

	위해 (size_t i = 0; i < strlen(tag); i++) {
		switch (tag[i]) {
		case '-':
			dash_count++;
			break;

		case '.':
			point_count++;
			break;

		case 'r':
			만약 (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			break;

		case 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			break;

		case 't':
			만약 (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} 아니면 {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			break;

		case 'v':
			만약 (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			break;

		default:
			break;
		}
	}

	만약 ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_count == 2 && point_count == 2) ||
	    (dash_count == 3 && point_count == 4) ||
	    (dash_count == 4 && point_count == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	위해 (size_t i = 0; i < strlen(tag); i++) {
		만약 (buffer_counter > 5) {
			continue;
		}

		만약 (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} 아니면 {
			만약 (buffer >= 0) {
				만약 (buffer_counter + 1 == 4 && tag[i] == '-') {
					break;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	만약 (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	만약 (buffer_counter == 6) {
		version_number = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} 아니면 만약 (buffer_counter == 3) {
		version_number = firmware_type;

	} 아니면 {
		version_number = 0;
	}

	반환 version_number;
}

uint32_t px4_firmware_vendor_version(void)
{
	반환 version_tag_to_vendor_version_number(PX4_GIT_TAG_STR);
}

const char *px4_firmware_git_branch(void)
{
	반환 PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(void)
{
#만약 defined(__PX4_NUTTX)
	반환 CONFIG_CDCACM_PRODUCTID;
#아니면
	반환 1;
#endif
}

uint32_t px4_os_version(void)
{
#만약 defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	반환 0; //TODO: implement version for Darwin, Cygwin, QuRT
#elif defined(__PX4_LINUX)
	struct utsname name;

	만약 (uname(&name) == 0) {
		char *c = name.release;

		// cut the part after the first '-'
		동안 (*c && *c != '-') {
			++c;
		}

		*c = 0;
		반환 version_tag_to_number(name.release);

	} 아니면 {
		반환 0;
	}

#elif defined(__PX4_NUTTX)
	반환 version_tag_to_number(NUTTX_GIT_TAG_STR);
#아니면
# error "px4_os_version not implemented for current OS"
#endif
}

const char *px4_os_version_string(void)
{
#만약 defined(__PX4_NUTTX)
	반환 NUTTX_GIT_VERSION_STR;
#아니면
	반환 NULL;
#endif
}

const char *px4_os_name(void)
{
#만약 defined(__PX4_DARWIN)
	반환 "Darwin";
#elif defined(__PX4_LINUX)
	반환 "Linux";
#elif defined(__PX4_QURT)
	반환 "QuRT";
#elif defined(__PX4_NUTTX)
	반환 "NuttX";
#elif defined(__PX4_CYGWIN)
	반환 "Cygwin";
#아니면
# error "px4_os_name not implemented for current OS"
#endif
}

const char *px4_toolchain_name(void)
{
#만약 defined(__clang__)
	반환 "Clang/LLVM";
#elif defined(__ICC) || defined(__INTEL_COMPILER)
	반환 "Intel ICC";
#elif defined(__GNUC__) || defined(__GNUG__)
	반환 "GNU GCC";
#elif defined(_MSC_VER)
	반환 "MS Visual Studio";
#아니면
	반환 "Unknown";
#endif
}

const char *px4_toolchain_version(void)
{
#ifdef __VERSION__
	반환 __VERSION__;
#아니면
	반환 "";
#endif
}

const char *px4_firmware_version_string(void)
{
	반환 PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_binary(void)
{
	반환 PX4_GIT_VERSION_BINARY;
}


#ifdef MAVLINK_LIB_GIT_VERSION_BINARY
uint64_t px4_mavlink_lib_version_binary(void)
{
	반환 MAVLINK_LIB_GIT_VERSION_BINARY;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_binary(void)
{
#ifdef NUTTX_GIT_VERSION_BINARY
	반환 NUTTX_GIT_VERSION_BINARY;
#아니면
	반환 0;
#endif
}

const char *px4_firmware_oem_version_string(void)
{
	반환 PX4_GIT_OEM_VERSION_STR;
}
