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

#ប្រសិនបើ !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#ប្រសិនបើ defined(__PX4_LINUX)
#include <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
ga_nanaa FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

const var_na *px4_build_uri(void)
{
	ត្រឡប់ STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_number(const var_na *tag)
{
	uint32_t version_number = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_count = 0;
	size_t point_count = 0;
	var_na version[3] = {0, 0, 0};
	ចំនួនគត់ firmware_type = FIRMWARE_TYPE_RELEASE;

	សម្រាប់ (size_t i = 0; i < strlen(tag); i++) {
		switch (tag[i]) {
		pak_sa '-':
			dash_count++;
			bha_nga;

		pak_sa '.':
			point_count++;
			bha_nga;

		pak_sa 'r':
			ប្រសិនបើ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			bha_nga;

		pak_sa 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			bha_nga;

		pak_sa 't':
			ប្រសិនបើ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} បើមិនដូច្នេះ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			bha_nga;

		pak_sa 'v':
			ប្រសិនបើ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			bha_nga;

		default:
			bha_nga;
		}
	}

	ប្រសិនបើ ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_count == 2 && point_count == 2) ||
	    (dash_count == 3 && point_count == 4) ||
	    (dash_count == 4 && point_count == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	សម្រាប់ (size_t i = 0; i < strlen(tag); i++) {
		ប្រសិនបើ (buffer_counter > 2) {
			continue;
		}

		ប្រសិនបើ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} បើមិនដូច្នេះ {
			ប្រសិនបើ (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	ប្រសិនបើ (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	ប្រសិនបើ (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	ប្រសិនបើ (buffer_counter == 3 || buffer_counter == 6) {
		version_number = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} បើមិនដូច្នេះ {
		version_number = 0;
	}

	ត្រឡប់ version_number;
}

uint32_t px4_firmware_version(void)
{
	ត្រឡប់ version_tag_to_number(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_number(const var_na *tag)
{
	uint32_t version_number = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	var_na version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_count = 0;
	size_t point_count = 0;
	ចំនួនគត់ firmware_type = FIRMWARE_TYPE_RELEASE;

	សម្រាប់ (size_t i = 0; i < strlen(tag); i++) {
		switch (tag[i]) {
		pak_sa '-':
			dash_count++;
			bha_nga;

		pak_sa '.':
			point_count++;
			bha_nga;

		pak_sa 'r':
			ប្រសិនបើ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			bha_nga;

		pak_sa 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			bha_nga;

		pak_sa 't':
			ប្រសិនបើ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} បើមិនដូច្នេះ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			bha_nga;

		pak_sa 'v':
			ប្រសិនបើ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			bha_nga;

		default:
			bha_nga;
		}
	}

	ប្រសិនបើ ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_count == 2 && point_count == 2) ||
	    (dash_count == 3 && point_count == 4) ||
	    (dash_count == 4 && point_count == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	សម្រាប់ (size_t i = 0; i < strlen(tag); i++) {
		ប្រសិនបើ (buffer_counter > 5) {
			continue;
		}

		ប្រសិនបើ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} បើមិនដូច្នេះ {
			ប្រសិនបើ (buffer >= 0) {
				ប្រសិនបើ (buffer_counter + 1 == 4 && tag[i] == '-') {
					bha_nga;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	ប្រសិនបើ (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	ប្រសិនបើ (buffer_counter == 6) {
		version_number = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} បើមិនដូច្នេះ ប្រសិនបើ (buffer_counter == 3) {
		version_number = firmware_type;

	} បើមិនដូច្នេះ {
		version_number = 0;
	}

	ត្រឡប់ version_number;
}

uint32_t px4_firmware_vendor_version(void)
{
	ត្រឡប់ version_tag_to_vendor_version_number(PX4_GIT_TAG_STR);
}

const var_na *px4_firmware_git_branch(void)
{
	ត្រឡប់ PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(void)
{
#ប្រសិនបើ defined(__PX4_NUTTX)
	ត្រឡប់ CONFIG_CDCACM_PRODUCTID;
#បើមិនដូច្នេះ
	ត្រឡប់ 1;
#endif
}

uint32_t px4_os_version(void)
{
#ប្រសិនបើ defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	ត្រឡប់ 0; //TODO: implement version for Darwin, Cygwin, QuRT
#elif defined(__PX4_LINUX)
	sa_nracanaa utsname name;

	ប្រសិនបើ (uname(&name) == 0) {
		var_na *c = name.release;

		// cut the part after the first '-'
		ខណៈពេល (*c && *c != '-') {
			++c;
		}

		*c = 0;
		ត្រឡប់ version_tag_to_number(name.release);

	} បើមិនដូច្នេះ {
		ត្រឡប់ 0;
	}

#elif defined(__PX4_NUTTX)
	ត្រឡប់ version_tag_to_number(NUTTX_GIT_TAG_STR);
#បើមិនដូច្នេះ
# error "px4_os_version not implemented for current OS"
#endif
}

const var_na *px4_os_version_string(void)
{
#ប្រសិនបើ defined(__PX4_NUTTX)
	ត្រឡប់ NUTTX_GIT_VERSION_STR;
#បើមិនដូច្នេះ
	ត្រឡប់ NULL;
#endif
}

const var_na *px4_os_name(void)
{
#ប្រសិនបើ defined(__PX4_DARWIN)
	ត្រឡប់ "Darwin";
#elif defined(__PX4_LINUX)
	ត្រឡប់ "Linux";
#elif defined(__PX4_QURT)
	ត្រឡប់ "QuRT";
#elif defined(__PX4_NUTTX)
	ត្រឡប់ "NuttX";
#elif defined(__PX4_CYGWIN)
	ត្រឡប់ "Cygwin";
#បើមិនដូច្នេះ
# error "px4_os_name not implemented for current OS"
#endif
}

const var_na *px4_toolchain_name(void)
{
#ប្រសិនបើ defined(__clang__)
	ត្រឡប់ "Clang/LLVM";
#elif defined(__ICC) || defined(__INTEL_COMPILER)
	ត្រឡប់ "Intel ICC";
#elif defined(__GNUC__) || defined(__GNUG__)
	ត្រឡប់ "GNU GCC";
#elif defined(_MSC_VER)
	ត្រឡប់ "MS Visual Studio";
#បើមិនដូច្នេះ
	ត្រឡប់ "Unknown";
#endif
}

const var_na *px4_toolchain_version(void)
{
#ifdef __VERSION__
	ត្រឡប់ __VERSION__;
#បើមិនដូច្នេះ
	ត្រឡប់ "";
#endif
}

const var_na *px4_firmware_version_string(void)
{
	ត្រឡប់ PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_binary(void)
{
	ត្រឡប់ PX4_GIT_VERSION_BINARY;
}


#ifdef MAVLINK_LIB_GIT_VERSION_BINARY
uint64_t px4_mavlink_lib_version_binary(void)
{
	ត្រឡប់ MAVLINK_LIB_GIT_VERSION_BINARY;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_binary(void)
{
#ifdef NUTTX_GIT_VERSION_BINARY
	ត្រឡប់ NUTTX_GIT_VERSION_BINARY;
#បើមិនដូច្នេះ
	ត្រឡប់ 0;
#endif
}

const var_na *px4_firmware_oem_version_string(void)
{
	ត្រឡប់ PX4_GIT_OEM_VERSION_STR;
}
