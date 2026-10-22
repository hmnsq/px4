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

#அந்தர்பாவ "version.h"

#அந்தர்பாவ "build_git_version.h"

#அந்தர்பாவ <string.h>

#எனில் !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#எனில் defined(__PX4_LINUX)
#அந்தர்பாவ <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
எண்ணிக்கை FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

மாறிலி எழுத்து *px4_build_uri(வெறுமை)
{
	திரும்பு STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_எண்(மாறிலி எழுத்து *tag)
{
	uint32_t version_எண் = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_மொத்தம் = 0;
	size_t point_மொத்தம் = 0;
	எழுத்து version[3] = {0, 0, 0};
	முழு firmware_type = FIRMWARE_TYPE_RELEASE;

	க்கு (size_t i = 0; i < strlen(tag); i++) {
		தேர்வு (tag[i]) {
		நிலை '-':
			dash_மொத்தம்++;
			நிறுத்து;

		நிலை '.':
			point_மொத்தம்++;
			நிறுத்து;

		நிலை 'r':
			எனில் (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			நிறுத்து;

		நிலை 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			நிறுத்து;

		நிலை 't':
			எனில் (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} இல்லையெனில் {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			நிறுத்து;

		நிலை 'v':
			எனில் (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			நிறுத்து;

		இயல்பு:
			நிறுத்து;
		}
	}

	எனில் ((dash_மொத்தம் == 1 && point_மொத்தம் == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_மொத்தம் == 2 && point_மொத்தம் == 2) ||
	    (dash_மொத்தம் == 3 && point_மொத்தம் == 4) ||
	    (dash_மொத்தம் == 4 && point_மொத்தம் == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	க்கு (size_t i = 0; i < strlen(tag); i++) {
		எனில் (buffer_counter > 2) {
			தொடர்;
		}

		எனில் (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} இல்லையெனில் {
			எனில் (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	எனில் (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	எனில் (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	எனில் (buffer_counter == 3 || buffer_counter == 6) {
		version_எண் = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} இல்லையெனில் {
		version_எண் = 0;
	}

	திரும்பு version_எண்;
}

uint32_t px4_firmware_version(வெறுமை)
{
	திரும்பு version_tag_to_எண்(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_எண்(மாறிலி எழுத்து *tag)
{
	uint32_t version_எண் = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	எழுத்து version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_மொத்தம் = 0;
	size_t point_மொத்தம் = 0;
	முழு firmware_type = FIRMWARE_TYPE_RELEASE;

	க்கு (size_t i = 0; i < strlen(tag); i++) {
		தேர்வு (tag[i]) {
		நிலை '-':
			dash_மொத்தம்++;
			நிறுத்து;

		நிலை '.':
			point_மொத்தம்++;
			நிறுத்து;

		நிலை 'r':
			எனில் (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			நிறுத்து;

		நிலை 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			நிறுத்து;

		நிலை 't':
			எனில் (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} இல்லையெனில் {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			நிறுத்து;

		நிலை 'v':
			எனில் (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			நிறுத்து;

		இயல்பு:
			நிறுத்து;
		}
	}

	எனில் ((dash_மொத்தம் == 1 && point_மொத்தம் == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_மொத்தம் == 2 && point_மொத்தம் == 2) ||
	    (dash_மொத்தம் == 3 && point_மொத்தம் == 4) ||
	    (dash_மொத்தம் == 4 && point_மொத்தம் == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	க்கு (size_t i = 0; i < strlen(tag); i++) {
		எனில் (buffer_counter > 5) {
			தொடர்;
		}

		எனில் (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} இல்லையெனில் {
			எனில் (buffer >= 0) {
				எனில் (buffer_counter + 1 == 4 && tag[i] == '-') {
					நிறுத்து;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	எனில் (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	எனில் (buffer_counter == 6) {
		version_எண் = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} இல்லையெனில் எனில் (buffer_counter == 3) {
		version_எண் = firmware_type;

	} இல்லையெனில் {
		version_எண் = 0;
	}

	திரும்பு version_எண்;
}

uint32_t px4_firmware_vendor_version(வெறுமை)
{
	திரும்பு version_tag_to_vendor_version_எண்(PX4_GIT_TAG_STR);
}

மாறிலி எழுத்து *px4_firmware_git_branch(வெறுமை)
{
	திரும்பு PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(வெறுமை)
{
#எனில் defined(__PX4_NUTTX)
	திரும்பு CONFIG_CDCACM_PRODUCTID;
#இல்லையெனில்
	திரும்பு 1;
#endif
}

uint32_t px4_os_version(வெறுமை)
{
#எனில் defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	திரும்பு 0; //TODO: implement version for Darwin, Cygwin, QuRT
#அல்லதெனில் defined(__PX4_LINUX)
	கட்டமைப்பு utsname name;

	எனில் (uname(&name) == 0) {
		எழுத்து *c = name.release;

		// cut the part after the first '-'
		போது (*c && *c != '-') {
			++c;
		}

		*c = 0;
		திரும்பு version_tag_to_எண்(name.release);

	} இல்லையெனில் {
		திரும்பு 0;
	}

#அல்லதெனில் defined(__PX4_NUTTX)
	திரும்பு version_tag_to_எண்(NUTTX_GIT_TAG_STR);
#இல்லையெனில்
# error "px4_os_version not implemented for current OS"
#endif
}

மாறிலி எழுத்து *px4_os_version_string(வெறுமை)
{
#எனில் defined(__PX4_NUTTX)
	திரும்பு NUTTX_GIT_VERSION_STR;
#இல்லையெனில்
	திரும்பு வெற்று;
#endif
}

மாறிலி எழுத்து *px4_os_name(வெறுமை)
{
#எனில் defined(__PX4_DARWIN)
	திரும்பு "Darwin";
#அல்லதெனில் defined(__PX4_LINUX)
	திரும்பு "Linux";
#அல்லதெனில் defined(__PX4_QURT)
	திரும்பு "QuRT";
#அல்லதெனில் defined(__PX4_NUTTX)
	திரும்பு "NuttX";
#அல்லதெனில் defined(__PX4_CYGWIN)
	திரும்பு "Cygwin";
#இல்லையெனில்
# error "px4_os_name not implemented for current OS"
#endif
}

மாறிலி எழுத்து *px4_toolchain_name(வெறுமை)
{
#எனில் defined(__clang__)
	திரும்பு "Clang/LLVM";
#அல்லதெனில் defined(__ICC) || defined(__INTEL_COMPILER)
	திரும்பு "Intel ICC";
#அல்லதெனில் defined(__GNUC__) || defined(__GNUG__)
	திரும்பு "GNU GCC";
#அல்லதெனில் defined(_MSC_VER)
	திரும்பு "MS Visual Studio";
#இல்லையெனில்
	திரும்பு "Unknown";
#endif
}

மாறிலி எழுத்து *px4_toolchain_version(வெறுமை)
{
#ifdef __VERSION__
	திரும்பு __VERSION__;
#இல்லையெனில்
	திரும்பு "";
#endif
}

மாறிலி எழுத்து *px4_firmware_version_string(வெறுமை)
{
	திரும்பு PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_இருமம்(வெறுமை)
{
	திரும்பு PX4_GIT_VERSION_இருமம்;
}


#ifdef MAVLINK_LIB_GIT_VERSION_இருமம்
uint64_t px4_mavlink_lib_version_இருமம்(வெறுமை)
{
	திரும்பு MAVLINK_LIB_GIT_VERSION_இருமம்;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_இருமம்(வெறுமை)
{
#ifdef NUTTX_GIT_VERSION_இருமம்
	திரும்பு NUTTX_GIT_VERSION_இருமம்;
#இல்லையெனில்
	திரும்பு 0;
#endif
}

மாறிலி எழுத்து *px4_firmware_oem_version_string(வெறுமை)
{
	திரும்பு PX4_GIT_OEM_VERSION_STR;
}
