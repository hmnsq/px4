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

#𐤀𐤌 !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#𐤀𐤌 defined(__PX4_LINUX)
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
			𐤀𐤌 (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			𐤀𐤌 (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} وإلا {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			اكسر;

		حالة 'v':
			𐤀𐤌 (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			اكسر;

		افتراضي:
			اكسر;
		}
	}

	𐤀𐤌 ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_count == 2 && point_count == 2) ||
	    (dash_count == 3 && point_count == 4) ||
	    (dash_count == 4 && point_count == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < strlen(tag); i++) {
		𐤀𐤌 (buffer_counter > 2) {
			استمر;
		}

		𐤀𐤌 (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} وإلا {
			𐤀𐤌 (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	𐤀𐤌 (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	𐤀𐤌 (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	𐤀𐤌 (buffer_counter == 3 || buffer_counter == 6) {
		version_number = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} وإلا {
		version_number = 0;
	}

	أرجع version_number;
}

uint32_t px4_firmware_version(فراغ)
{
	أرجع version_tag_to_number(PX4_GIT_TAG_STR);
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
			𐤀𐤌 (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			اكسر;

		حالة 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			اكسر;

		حالة 't':
			𐤀𐤌 (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} وإلا {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			اكسر;

		حالة 'v':
			𐤀𐤌 (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			اكسر;

		افتراضي:
			اكسر;
		}
	}

	𐤀𐤌 ((dash_count == 1 && point_count == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_count == 2 && point_count == 2) ||
	    (dash_count == 3 && point_count == 4) ||
	    (dash_count == 4 && point_count == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	لكل (size_t i = 0; i < strlen(tag); i++) {
		𐤀𐤌 (buffer_counter > 5) {
			استمر;
		}

		𐤀𐤌 (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} وإلا {
			𐤀𐤌 (buffer >= 0) {
				𐤀𐤌 (buffer_counter + 1 == 4 && tag[i] == '-') {
					اكسر;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	𐤀𐤌 (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	𐤀𐤌 (buffer_counter == 6) {
		version_number = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} وإلا 𐤀𐤌 (buffer_counter == 3) {
		version_number = firmware_type;

	} وإلا {
		version_number = 0;
	}

	أرجع version_number;
}

uint32_t px4_firmware_vendor_version(فراغ)
{
	أرجع version_tag_to_vendor_version_number(PX4_GIT_TAG_STR);
}

ثابت حرف *px4_firmware_git_branch(فراغ)
{
	أرجع PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(فراغ)
{
#𐤀𐤌 defined(__PX4_NUTTX)
	أرجع CONFIG_CDCACM_PRODUCTID;
#وإلا
	أرجع 1;
#endif
}

uint32_t px4_os_version(فراغ)
{
#𐤀𐤌 defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	أرجع 0; //TODO: implement version for Darwin, Cygwin, QuRT
#وإلاإذا defined(__PX4_LINUX)
	بنية utsname name;

	𐤀𐤌 (uname(&name) == 0) {
		حرف *c = name.release;

		// cut the part after the first '-'
		طالما (*c && *c != '-') {
			++c;
		}

		*c = 0;
		أرجع version_tag_to_number(name.release);

	} وإلا {
		أرجع 0;
	}

#وإلاإذا defined(__PX4_NUTTX)
	أرجع version_tag_to_number(NUTTX_GIT_TAG_STR);
#وإلا
# error "px4_os_version not implemented for current OS"
#endif
}

ثابت حرف *px4_os_version_string(فراغ)
{
#𐤀𐤌 defined(__PX4_NUTTX)
	أرجع NUTTX_GIT_VERSION_STR;
#وإلا
	أرجع عدم;
#endif
}

ثابت حرف *px4_os_name(فراغ)
{
#𐤀𐤌 defined(__PX4_DARWIN)
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
#𐤀𐤌 defined(__clang__)
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

uint64_t px4_firmware_version_binary(فراغ)
{
	أرجع PX4_GIT_VERSION_BINARY;
}


#ifdef MAVLINK_LIB_GIT_VERSION_BINARY
uint64_t px4_mavlink_lib_version_binary(فراغ)
{
	أرجع MAVLINK_LIB_GIT_VERSION_BINARY;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_binary(فراغ)
{
#ifdef NUTTX_GIT_VERSION_BINARY
	أرجع NUTTX_GIT_VERSION_BINARY;
#وإلا
	أرجع 0;
#endif
}

ثابت حرف *px4_firmware_oem_version_string(فراغ)
{
	أرجع PX4_GIT_OEM_VERSION_STR;
}
