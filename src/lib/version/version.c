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

#ಅನ್ತರ್ಭಾವ "version.h"

#ಅನ್ತರ್ಭಾವ "build_git_version.h"

#ಅನ್ತರ್ಭಾವ <string.h>

#ಯದಿ !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#ಯದಿ defined(__PX4_LINUX)
#ಅನ್ತರ್ಭಾವ <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
ಗಣನಾ FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

ಧ್ರುವ ವರ್ಣ *px4_build_uri(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_ಸಂಖ್ಯೆ(ಧ್ರುವ ವರ್ಣ *tag)
{
	uint32_t version_ಸಂಖ್ಯೆ = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_ಎಣಿಕೆ = 0;
	size_t point_ಎಣಿಕೆ = 0;
	ವರ್ಣ version[3] = {0, 0, 0};
	ಪೂರ್ಣಾಙ್ಕ firmware_type = FIRMWARE_TYPE_RELEASE;

	ಪ್ರತಿ (size_t i = 0; i < strlen(tag); i++) {
		ವಿಕಲ್ಪ (tag[i]) {
		ಪಕ್ಷ '-':
			dash_ಎಣಿಕೆ++;
			ಭಙ್ಗ;

		ಪಕ್ಷ '.':
			point_ಎಣಿಕೆ++;
			ಭಙ್ಗ;

		ಪಕ್ಷ 'r':
			ಯದಿ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ಭಙ್ಗ;

		ಪಕ್ಷ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ಭಙ್ಗ;

		ಪಕ್ಷ 't':
			ಯದಿ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ಅನ್ಯಥಾ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ಭಙ್ಗ;

		ಪಕ್ಷ 'v':
			ಯದಿ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
	}

	ಯದಿ ((dash_ಎಣಿಕೆ == 1 && point_ಎಣಿಕೆ == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ಎಣಿಕೆ == 2 && point_ಎಣಿಕೆ == 2) ||
	    (dash_ಎಣಿಕೆ == 3 && point_ಎಣಿಕೆ == 4) ||
	    (dash_ಎಣಿಕೆ == 4 && point_ಎಣಿಕೆ == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	ಪ್ರತಿ (size_t i = 0; i < strlen(tag); i++) {
		ಯದಿ (buffer_counter > 2) {
			ಅನುವರ್ತ;
		}

		ಯದಿ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} ಅನ್ಯಥಾ {
			ಯದಿ (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	ಯದಿ (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	ಯದಿ (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	ಯದಿ (buffer_counter == 3 || buffer_counter == 6) {
		version_ಸಂಖ್ಯೆ = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} ಅನ್ಯಥಾ {
		version_ಸಂಖ್ಯೆ = 0;
	}

	ಪ್ರತ್ಯಾಗಮ version_ಸಂಖ್ಯೆ;
}

uint32_t px4_firmware_version(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ version_tag_to_ಸಂಖ್ಯೆ(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_ಸಂಖ್ಯೆ(ಧ್ರುವ ವರ್ಣ *tag)
{
	uint32_t version_ಸಂಖ್ಯೆ = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	ವರ್ಣ version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_ಎಣಿಕೆ = 0;
	size_t point_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಙ್ಕ firmware_type = FIRMWARE_TYPE_RELEASE;

	ಪ್ರತಿ (size_t i = 0; i < strlen(tag); i++) {
		ವಿಕಲ್ಪ (tag[i]) {
		ಪಕ್ಷ '-':
			dash_ಎಣಿಕೆ++;
			ಭಙ್ಗ;

		ಪಕ್ಷ '.':
			point_ಎಣಿಕೆ++;
			ಭಙ್ಗ;

		ಪಕ್ಷ 'r':
			ಯದಿ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ಭಙ್ಗ;

		ಪಕ್ಷ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ಭಙ್ಗ;

		ಪಕ್ಷ 't':
			ಯದಿ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ಅನ್ಯಥಾ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ಭಙ್ಗ;

		ಪಕ್ಷ 'v':
			ಯದಿ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
	}

	ಯದಿ ((dash_ಎಣಿಕೆ == 1 && point_ಎಣಿಕೆ == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ಎಣಿಕೆ == 2 && point_ಎಣಿಕೆ == 2) ||
	    (dash_ಎಣಿಕೆ == 3 && point_ಎಣಿಕೆ == 4) ||
	    (dash_ಎಣಿಕೆ == 4 && point_ಎಣಿಕೆ == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	ಪ್ರತಿ (size_t i = 0; i < strlen(tag); i++) {
		ಯದಿ (buffer_counter > 5) {
			ಅನುವರ್ತ;
		}

		ಯದಿ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} ಅನ್ಯಥಾ {
			ಯದಿ (buffer >= 0) {
				ಯದಿ (buffer_counter + 1 == 4 && tag[i] == '-') {
					ಭಙ್ಗ;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	ಯದಿ (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	ಯದಿ (buffer_counter == 6) {
		version_ಸಂಖ್ಯೆ = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} ಅನ್ಯಥಾ ಯದಿ (buffer_counter == 3) {
		version_ಸಂಖ್ಯೆ = firmware_type;

	} ಅನ್ಯಥಾ {
		version_ಸಂಖ್ಯೆ = 0;
	}

	ಪ್ರತ್ಯಾಗಮ version_ಸಂಖ್ಯೆ;
}

uint32_t px4_firmware_vendor_version(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ version_tag_to_vendor_version_ಸಂಖ್ಯೆ(PX4_GIT_TAG_STR);
}

ಧ್ರುವ ವರ್ಣ *px4_firmware_git_branch(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(ಶೂನ್ಯ)
{
#ಯದಿ defined(__PX4_NUTTX)
	ಪ್ರತ್ಯಾಗಮ CONFIG_CDCACM_PRODUCTID;
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ 1;
#endif
}

uint32_t px4_os_version(ಶೂನ್ಯ)
{
#ಯದಿ defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	ಪ್ರತ್ಯಾಗಮ 0; //TODO: implement version for Darwin, Cygwin, QuRT
#ನೋಚೇದ್ಯದಿ defined(__PX4_LINUX)
	ಸಂರಚನಾ utsname name;

	ಯದಿ (uname(&name) == 0) {
		ವರ್ಣ *c = name.release;

		// cut the part after the first '-'
		ಯಾವತ್ (*c && *c != '-') {
			++c;
		}

		*c = 0;
		ಪ್ರತ್ಯಾಗಮ version_tag_to_ಸಂಖ್ಯೆ(name.release);

	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

#ನೋಚೇದ್ಯದಿ defined(__PX4_NUTTX)
	ಪ್ರತ್ಯಾಗಮ version_tag_to_ಸಂಖ್ಯೆ(NUTTX_GIT_TAG_STR);
#ಅನ್ಯಥಾ
# error "px4_os_version not implemented for current OS"
#endif
}

ಧ್ರುವ ವರ್ಣ *px4_os_version_string(ಶೂನ್ಯ)
{
#ಯದಿ defined(__PX4_NUTTX)
	ಪ್ರತ್ಯಾಗಮ NUTTX_GIT_VERSION_STR;
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
}

ಧ್ರುವ ವರ್ಣ *px4_os_name(ಶೂನ್ಯ)
{
#ಯದಿ defined(__PX4_DARWIN)
	ಪ್ರತ್ಯಾಗಮ "Darwin";
#ನೋಚೇದ್ಯದಿ defined(__PX4_LINUX)
	ಪ್ರತ್ಯಾಗಮ "Linux";
#ನೋಚೇದ್ಯದಿ defined(__PX4_QURT)
	ಪ್ರತ್ಯಾಗಮ "QuRT";
#ನೋಚೇದ್ಯದಿ defined(__PX4_NUTTX)
	ಪ್ರತ್ಯಾಗಮ "NuttX";
#ನೋಚೇದ್ಯದಿ defined(__PX4_CYGWIN)
	ಪ್ರತ್ಯಾಗಮ "Cygwin";
#ಅನ್ಯಥಾ
# error "px4_os_name not implemented for current OS"
#endif
}

ಧ್ರುವ ವರ್ಣ *px4_toolchain_name(ಶೂನ್ಯ)
{
#ಯದಿ defined(__clang__)
	ಪ್ರತ್ಯಾಗಮ "Clang/LLVM";
#ನೋಚೇದ್ಯದಿ defined(__ICC) || defined(__INTEL_COMPILER)
	ಪ್ರತ್ಯಾಗಮ "Intel ICC";
#ನೋಚೇದ್ಯದಿ defined(__GNUC__) || defined(__GNUG__)
	ಪ್ರತ್ಯಾಗಮ "GNU GCC";
#ನೋಚೇದ್ಯದಿ defined(_MSC_VER)
	ಪ್ರತ್ಯಾಗಮ "MS Visual Studio";
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ "Unknown";
#endif
}

ಧ್ರುವ ವರ್ಣ *px4_toolchain_version(ಶೂನ್ಯ)
{
#ifdef __VERSION__
	ಪ್ರತ್ಯಾಗಮ __VERSION__;
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ "";
#endif
}

ಧ್ರುವ ವರ್ಣ *px4_firmware_version_string(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_ದ್ವಿಮಾನ(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ PX4_GIT_VERSION_ದ್ವಿಮಾನ;
}


#ifdef MAVLINK_LIB_GIT_VERSION_ದ್ವಿಮಾನ
uint64_t px4_mavlink_lib_version_ದ್ವಿಮಾನ(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ MAVLINK_LIB_GIT_VERSION_ದ್ವಿಮಾನ;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_ದ್ವಿಮಾನ(ಶೂನ್ಯ)
{
#ifdef NUTTX_GIT_VERSION_ದ್ವಿಮಾನ
	ಪ್ರತ್ಯಾಗಮ NUTTX_GIT_VERSION_ದ್ವಿಮಾನ;
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

ಧ್ರುವ ವರ್ಣ *px4_firmware_oem_version_string(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ PX4_GIT_OEM_VERSION_STR;
}
