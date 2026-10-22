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

#અન્તર્ભાવ "version.h"

#અન્તર્ભાવ "build_git_version.h"

#અન્તર્ભાવ <string.h>

#જો !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#જો defined(__PX4_LINUX)
#અન્તર્ભાવ <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
ગણના FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

અચલ અક્ષર *px4_build_uri(શૂન્ય)
{
	પાછા STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_સંખ્યા(અચલ અક્ષર *tag)
{
	uint32_t version_સંખ્યા = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_ગણતરી = 0;
	size_t point_ગણતરી = 0;
	અક્ષર version[3] = {0, 0, 0};
	પૂર્ણાંક firmware_type = FIRMWARE_TYPE_RELEASE;

	માટે (size_t i = 0; i < strlen(tag); i++) {
		પસંદગી (tag[i]) {
		સ્થિતિ '-':
			dash_ગણતરી++;
			તોડો;

		સ્થિતિ '.':
			point_ગણતરી++;
			તોડો;

		સ્થિતિ 'r':
			જો (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			તોડો;

		સ્થિતિ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			તોડો;

		સ્થિતિ 't':
			જો (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} નહીંતર {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			તોડો;

		સ્થિતિ 'v':
			જો (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			તોડો;

		મૂળભૂત:
			તોડો;
		}
	}

	જો ((dash_ગણતરી == 1 && point_ગણતરી == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ગણતરી == 2 && point_ગણતરી == 2) ||
	    (dash_ગણતરી == 3 && point_ગણતરી == 4) ||
	    (dash_ગણતરી == 4 && point_ગણતરી == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	માટે (size_t i = 0; i < strlen(tag); i++) {
		જો (buffer_counter > 2) {
			ચાલુ;
		}

		જો (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} નહીંતર {
			જો (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	જો (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	જો (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	જો (buffer_counter == 3 || buffer_counter == 6) {
		version_સંખ્યા = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} નહીંતર {
		version_સંખ્યા = 0;
	}

	પાછા version_સંખ્યા;
}

uint32_t px4_firmware_version(શૂન્ય)
{
	પાછા version_tag_to_સંખ્યા(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_સંખ્યા(અચલ અક્ષર *tag)
{
	uint32_t version_સંખ્યા = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	અક્ષર version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_ગણતરી = 0;
	size_t point_ગણતરી = 0;
	પૂર્ણાંક firmware_type = FIRMWARE_TYPE_RELEASE;

	માટે (size_t i = 0; i < strlen(tag); i++) {
		પસંદગી (tag[i]) {
		સ્થિતિ '-':
			dash_ગણતરી++;
			તોડો;

		સ્થિતિ '.':
			point_ગણતરી++;
			તોડો;

		સ્થિતિ 'r':
			જો (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			તોડો;

		સ્થિતિ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			તોડો;

		સ્થિતિ 't':
			જો (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} નહીંતર {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			તોડો;

		સ્થિતિ 'v':
			જો (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			તોડો;

		મૂળભૂત:
			તોડો;
		}
	}

	જો ((dash_ગણતરી == 1 && point_ગણતરી == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ગણતરી == 2 && point_ગણતરી == 2) ||
	    (dash_ગણતરી == 3 && point_ગણતરી == 4) ||
	    (dash_ગણતરી == 4 && point_ગણતરી == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	માટે (size_t i = 0; i < strlen(tag); i++) {
		જો (buffer_counter > 5) {
			ચાલુ;
		}

		જો (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} નહીંતર {
			જો (buffer >= 0) {
				જો (buffer_counter + 1 == 4 && tag[i] == '-') {
					તોડો;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	જો (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	જો (buffer_counter == 6) {
		version_સંખ્યા = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} નહીંતર જો (buffer_counter == 3) {
		version_સંખ્યા = firmware_type;

	} નહીંતર {
		version_સંખ્યા = 0;
	}

	પાછા version_સંખ્યા;
}

uint32_t px4_firmware_vendor_version(શૂન્ય)
{
	પાછા version_tag_to_vendor_version_સંખ્યા(PX4_GIT_TAG_STR);
}

અચલ અક્ષર *px4_firmware_git_branch(શૂન્ય)
{
	પાછા PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(શૂન્ય)
{
#જો defined(__PX4_NUTTX)
	પાછા CONFIG_CDCACM_PRODUCTID;
#નહીંતર
	પાછા 1;
#endif
}

uint32_t px4_os_version(શૂન્ય)
{
#જો defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	પાછા 0; //TODO: implement version for Darwin, Cygwin, QuRT
#અથવાજો defined(__PX4_LINUX)
	સંરચના utsname name;

	જો (uname(&name) == 0) {
		અક્ષર *c = name.release;

		// cut the part after the first '-'
		જ્યારે (*c && *c != '-') {
			++c;
		}

		*c = 0;
		પાછા version_tag_to_સંખ્યા(name.release);

	} નહીંતર {
		પાછા 0;
	}

#અથવાજો defined(__PX4_NUTTX)
	પાછા version_tag_to_સંખ્યા(NUTTX_GIT_TAG_STR);
#નહીંતર
# error "px4_os_version not implemented for current OS"
#endif
}

અચલ અક્ષર *px4_os_version_string(શૂન્ય)
{
#જો defined(__PX4_NUTTX)
	પાછા NUTTX_GIT_VERSION_STR;
#નહીંતર
	પાછા ખાલી;
#endif
}

અચલ અક્ષર *px4_os_name(શૂન્ય)
{
#જો defined(__PX4_DARWIN)
	પાછા "Darwin";
#અથવાજો defined(__PX4_LINUX)
	પાછા "Linux";
#અથવાજો defined(__PX4_QURT)
	પાછા "QuRT";
#અથવાજો defined(__PX4_NUTTX)
	પાછા "NuttX";
#અથવાજો defined(__PX4_CYGWIN)
	પાછા "Cygwin";
#નહીંતર
# error "px4_os_name not implemented for current OS"
#endif
}

અચલ અક્ષર *px4_toolchain_name(શૂન્ય)
{
#જો defined(__clang__)
	પાછા "Clang/LLVM";
#અથવાજો defined(__ICC) || defined(__INTEL_COMPILER)
	પાછા "Intel ICC";
#અથવાજો defined(__GNUC__) || defined(__GNUG__)
	પાછા "GNU GCC";
#અથવાજો defined(_MSC_VER)
	પાછા "MS Visual Studio";
#નહીંતર
	પાછા "Unknown";
#endif
}

અચલ અક્ષર *px4_toolchain_version(શૂન્ય)
{
#ifdef __VERSION__
	પાછા __VERSION__;
#નહીંતર
	પાછા "";
#endif
}

અચલ અક્ષર *px4_firmware_version_string(શૂન્ય)
{
	પાછા PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_દ્વિઅંકી(શૂન્ય)
{
	પાછા PX4_GIT_VERSION_દ્વિઅંકી;
}


#ifdef MAVLINK_LIB_GIT_VERSION_દ્વિઅંકી
uint64_t px4_mavlink_lib_version_દ્વિઅંકી(શૂન્ય)
{
	પાછા MAVLINK_LIB_GIT_VERSION_દ્વિઅંકી;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_દ્વિઅંકી(શૂન્ય)
{
#ifdef NUTTX_GIT_VERSION_દ્વિઅંકી
	પાછા NUTTX_GIT_VERSION_દ્વિઅંકી;
#નહીંતર
	પાછા 0;
#endif
}

અચલ અક્ષર *px4_firmware_oem_version_string(શૂન્ય)
{
	પાછા PX4_GIT_OEM_VERSION_STR;
}
