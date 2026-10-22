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

#ଅନ୍ତର୍ଭାଵ "version.h"

#ଅନ୍ତର୍ଭାଵ "build_git_version.h"

#ଅନ୍ତର୍ଭାଵ <string.h>

#ଯଦି !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#ଯଦି defined(__PX4_LINUX)
#ଅନ୍ତର୍ଭାଵ <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
ଗଣନା FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

ଅଚଳ ଅକ୍ଷର *px4_build_uri(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_ସଂଖ୍ୟା(ଅଚଳ ଅକ୍ଷର *tag)
{
	uint32_t version_ସଂଖ୍ୟା = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_ଗଣନା = 0;
	size_t point_ଗଣନା = 0;
	ଅକ୍ଷର version[3] = {0, 0, 0};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା firmware_type = FIRMWARE_TYPE_RELEASE;

	ପାଇଁ (size_t i = 0; i < strlen(tag); i++) {
		ବାଛ (tag[i]) {
		ସ୍ଥିତି '-':
			dash_ଗଣନା++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି '.':
			point_ଗଣନା++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'r':
			ଯଦି (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି 't':
			ଯଦି (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ଅନ୍ୟଥା {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'v':
			ଯଦି (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି ((dash_ଗଣନା == 1 && point_ଗଣନା == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ଗଣନା == 2 && point_ଗଣନା == 2) ||
	    (dash_ଗଣନା == 3 && point_ଗଣନା == 4) ||
	    (dash_ଗଣନା == 4 && point_ଗଣନା == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	ପାଇଁ (size_t i = 0; i < strlen(tag); i++) {
		ଯଦି (buffer_counter > 2) {
			ଜାରି;
		}

		ଯଦି (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} ଅନ୍ୟଥା {
			ଯଦି (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	ଯଦି (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	ଯଦି (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	ଯଦି (buffer_counter == 3 || buffer_counter == 6) {
		version_ସଂଖ୍ୟା = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} ଅନ୍ୟଥା {
		version_ସଂଖ୍ୟା = 0;
	}

	ଫେରନ୍ତୁ version_ସଂଖ୍ୟା;
}

uint32_t px4_firmware_version(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ version_tag_to_ସଂଖ୍ୟା(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_ସଂଖ୍ୟା(ଅଚଳ ଅକ୍ଷର *tag)
{
	uint32_t version_ସଂଖ୍ୟା = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	ଅକ୍ଷର version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_ଗଣନା = 0;
	size_t point_ଗଣନା = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା firmware_type = FIRMWARE_TYPE_RELEASE;

	ପାଇଁ (size_t i = 0; i < strlen(tag); i++) {
		ବାଛ (tag[i]) {
		ସ୍ଥିତି '-':
			dash_ଗଣନା++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି '.':
			point_ଗଣନା++;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'r':
			ଯଦି (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ଭାଙ୍ଗ;

		ସ୍ଥିତି 't':
			ଯଦି (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ଅନ୍ୟଥା {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'v':
			ଯଦି (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି ((dash_ଗଣନା == 1 && point_ଗଣନା == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ଗଣନା == 2 && point_ଗଣନା == 2) ||
	    (dash_ଗଣନା == 3 && point_ଗଣନା == 4) ||
	    (dash_ଗଣନା == 4 && point_ଗଣନା == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	ପାଇଁ (size_t i = 0; i < strlen(tag); i++) {
		ଯଦି (buffer_counter > 5) {
			ଜାରି;
		}

		ଯଦି (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} ଅନ୍ୟଥା {
			ଯଦି (buffer >= 0) {
				ଯଦି (buffer_counter + 1 == 4 && tag[i] == '-') {
					ଭାଙ୍ଗ;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	ଯଦି (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	ଯଦି (buffer_counter == 6) {
		version_ସଂଖ୍ୟା = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} ଅନ୍ୟଥା ଯଦି (buffer_counter == 3) {
		version_ସଂଖ୍ୟା = firmware_type;

	} ଅନ୍ୟଥା {
		version_ସଂଖ୍ୟା = 0;
	}

	ଫେରନ୍ତୁ version_ସଂଖ୍ୟା;
}

uint32_t px4_firmware_vendor_version(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ version_tag_to_vendor_version_ସଂଖ୍ୟା(PX4_GIT_TAG_STR);
}

ଅଚଳ ଅକ୍ଷର *px4_firmware_git_branch(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(ଶୂନ୍ୟ)
{
#ଯଦି defined(__PX4_NUTTX)
	ଫେରନ୍ତୁ CONFIG_CDCACM_PRODUCTID;
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ 1;
#endif
}

uint32_t px4_os_version(ଶୂନ୍ୟ)
{
#ଯଦି defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	ଫେରନ୍ତୁ 0; //TODO: implement version for Darwin, Cygwin, QuRT
#ନଚେତଯଦି defined(__PX4_LINUX)
	ସଂରଚନା utsname name;

	ଯଦି (uname(&name) == 0) {
		ଅକ୍ଷର *c = name.release;

		// cut the part after the first '-'
		ଯେତେବେଳେ (*c && *c != '-') {
			++c;
		}

		*c = 0;
		ଫେରନ୍ତୁ version_tag_to_ସଂଖ୍ୟା(name.release);

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ 0;
	}

#ନଚେତଯଦି defined(__PX4_NUTTX)
	ଫେରନ୍ତୁ version_tag_to_ସଂଖ୍ୟା(NUTTX_GIT_TAG_STR);
#ଅନ୍ୟଥା
# error "px4_os_version not implemented for current OS"
#endif
}

ଅଚଳ ଅକ୍ଷର *px4_os_version_string(ଶୂନ୍ୟ)
{
#ଯଦି defined(__PX4_NUTTX)
	ଫେରନ୍ତୁ NUTTX_GIT_VERSION_STR;
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି;
#endif
}

ଅଚଳ ଅକ୍ଷର *px4_os_name(ଶୂନ୍ୟ)
{
#ଯଦି defined(__PX4_DARWIN)
	ଫେରନ୍ତୁ "Darwin";
#ନଚେତଯଦି defined(__PX4_LINUX)
	ଫେରନ୍ତୁ "Linux";
#ନଚେତଯଦି defined(__PX4_QURT)
	ଫେରନ୍ତୁ "QuRT";
#ନଚେତଯଦି defined(__PX4_NUTTX)
	ଫେରନ୍ତୁ "NuttX";
#ନଚେତଯଦି defined(__PX4_CYGWIN)
	ଫେରନ୍ତୁ "Cygwin";
#ଅନ୍ୟଥା
# error "px4_os_name not implemented for current OS"
#endif
}

ଅଚଳ ଅକ୍ଷର *px4_toolchain_name(ଶୂନ୍ୟ)
{
#ଯଦି defined(__clang__)
	ଫେରନ୍ତୁ "Clang/LLVM";
#ନଚେତଯଦି defined(__ICC) || defined(__INTEL_COMPILER)
	ଫେରନ୍ତୁ "Intel ICC";
#ନଚେତଯଦି defined(__GNUC__) || defined(__GNUG__)
	ଫେରନ୍ତୁ "GNU GCC";
#ନଚେତଯଦି defined(_MSC_VER)
	ଫେରନ୍ତୁ "MS Visual Studio";
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ "Unknown";
#endif
}

ଅଚଳ ଅକ୍ଷର *px4_toolchain_version(ଶୂନ୍ୟ)
{
#ifdef __VERSION__
	ଫେରନ୍ତୁ __VERSION__;
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ "";
#endif
}

ଅଚଳ ଅକ୍ଷର *px4_firmware_version_string(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_ଦ୍ୱିମିକ(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ PX4_GIT_VERSION_ଦ୍ୱିମିକ;
}


#ifdef MAVLINK_LIB_GIT_VERSION_ଦ୍ୱିମିକ
uint64_t px4_mavlink_lib_version_ଦ୍ୱିମିକ(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ MAVLINK_LIB_GIT_VERSION_ଦ୍ୱିମିକ;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_ଦ୍ୱିମିକ(ଶୂନ୍ୟ)
{
#ifdef NUTTX_GIT_VERSION_ଦ୍ୱିମିକ
	ଫେରନ୍ତୁ NUTTX_GIT_VERSION_ଦ୍ୱିମିକ;
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ 0;
#endif
}

ଅଚଳ ଅକ୍ଷର *px4_firmware_oem_version_string(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ PX4_GIT_OEM_VERSION_STR;
}
