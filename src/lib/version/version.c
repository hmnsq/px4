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

#അന്തര്ഭാവ "version.h"

#അന്തര്ഭാവ "build_git_version.h"

#അന്തര്ഭാവ <string.h>

#എങ്കിൽ !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#എങ്കിൽ defined(__PX4_LINUX)
#അന്തര്ഭാവ <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
എണ്ണൽ FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

സ്ഥിരം അക്ഷരം *px4_build_uri(ശൂന്യം)
{
	തിരികെ STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_സംഖ്യ(സ്ഥിരം അക്ഷരം *tag)
{
	uint32_t version_സംഖ്യ = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_എണ്ണം = 0;
	size_t point_എണ്ണം = 0;
	അക്ഷരം version[3] = {0, 0, 0};
	പൂർണ്ണസംഖ്യ firmware_type = FIRMWARE_TYPE_RELEASE;

	വേണ്ടി (size_t i = 0; i < strlen(tag); i++) {
		തിരഞ്ഞെടുപ്പ് (tag[i]) {
		അവസ്ഥ '-':
			dash_എണ്ണം++;
			നിർത്തുക;

		അവസ്ഥ '.':
			point_എണ്ണം++;
			നിർത്തുക;

		അവസ്ഥ 'r':
			എങ്കിൽ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			നിർത്തുക;

		അവസ്ഥ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			നിർത്തുക;

		അവസ്ഥ 't':
			എങ്കിൽ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} അല്ലെങ്കിൽ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			നിർത്തുക;

		അവസ്ഥ 'v':
			എങ്കിൽ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
	}

	എങ്കിൽ ((dash_എണ്ണം == 1 && point_എണ്ണം == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_എണ്ണം == 2 && point_എണ്ണം == 2) ||
	    (dash_എണ്ണം == 3 && point_എണ്ണം == 4) ||
	    (dash_എണ്ണം == 4 && point_എണ്ണം == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	വേണ്ടി (size_t i = 0; i < strlen(tag); i++) {
		എങ്കിൽ (buffer_counter > 2) {
			തുടരുക;
		}

		എങ്കിൽ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} അല്ലെങ്കിൽ {
			എങ്കിൽ (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	എങ്കിൽ (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	എങ്കിൽ (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	എങ്കിൽ (buffer_counter == 3 || buffer_counter == 6) {
		version_സംഖ്യ = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} അല്ലെങ്കിൽ {
		version_സംഖ്യ = 0;
	}

	തിരികെ version_സംഖ്യ;
}

uint32_t px4_firmware_version(ശൂന്യം)
{
	തിരികെ version_tag_to_സംഖ്യ(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_സംഖ്യ(സ്ഥിരം അക്ഷരം *tag)
{
	uint32_t version_സംഖ്യ = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	അക്ഷരം version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_എണ്ണം = 0;
	size_t point_എണ്ണം = 0;
	പൂർണ്ണസംഖ്യ firmware_type = FIRMWARE_TYPE_RELEASE;

	വേണ്ടി (size_t i = 0; i < strlen(tag); i++) {
		തിരഞ്ഞെടുപ്പ് (tag[i]) {
		അവസ്ഥ '-':
			dash_എണ്ണം++;
			നിർത്തുക;

		അവസ്ഥ '.':
			point_എണ്ണം++;
			നിർത്തുക;

		അവസ്ഥ 'r':
			എങ്കിൽ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			നിർത്തുക;

		അവസ്ഥ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			നിർത്തുക;

		അവസ്ഥ 't':
			എങ്കിൽ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} അല്ലെങ്കിൽ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			നിർത്തുക;

		അവസ്ഥ 'v':
			എങ്കിൽ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
	}

	എങ്കിൽ ((dash_എണ്ണം == 1 && point_എണ്ണം == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_എണ്ണം == 2 && point_എണ്ണം == 2) ||
	    (dash_എണ്ണം == 3 && point_എണ്ണം == 4) ||
	    (dash_എണ്ണം == 4 && point_എണ്ണം == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	വേണ്ടി (size_t i = 0; i < strlen(tag); i++) {
		എങ്കിൽ (buffer_counter > 5) {
			തുടരുക;
		}

		എങ്കിൽ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} അല്ലെങ്കിൽ {
			എങ്കിൽ (buffer >= 0) {
				എങ്കിൽ (buffer_counter + 1 == 4 && tag[i] == '-') {
					നിർത്തുക;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	എങ്കിൽ (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	എങ്കിൽ (buffer_counter == 6) {
		version_സംഖ്യ = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} അല്ലെങ്കിൽ എങ്കിൽ (buffer_counter == 3) {
		version_സംഖ്യ = firmware_type;

	} അല്ലെങ്കിൽ {
		version_സംഖ്യ = 0;
	}

	തിരികെ version_സംഖ്യ;
}

uint32_t px4_firmware_vendor_version(ശൂന്യം)
{
	തിരികെ version_tag_to_vendor_version_സംഖ്യ(PX4_GIT_TAG_STR);
}

സ്ഥിരം അക്ഷരം *px4_firmware_git_branch(ശൂന്യം)
{
	തിരികെ PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(ശൂന്യം)
{
#എങ്കിൽ defined(__PX4_NUTTX)
	തിരികെ CONFIG_CDCACM_PRODUCTID;
#അല്ലെങ്കിൽ
	തിരികെ 1;
#endif
}

uint32_t px4_os_version(ശൂന്യം)
{
#എങ്കിൽ defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	തിരികെ 0; //TODO: implement version for Darwin, Cygwin, QuRT
#അല്ലെങ്കിലെങ്കിൽ defined(__PX4_LINUX)
	ഘടന utsname name;

	എങ്കിൽ (uname(&name) == 0) {
		അക്ഷരം *c = name.release;

		// cut the part after the first '-'
		ആകുമ്പോൾ (*c && *c != '-') {
			++c;
		}

		*c = 0;
		തിരികെ version_tag_to_സംഖ്യ(name.release);

	} അല്ലെങ്കിൽ {
		തിരികെ 0;
	}

#അല്ലെങ്കിലെങ്കിൽ defined(__PX4_NUTTX)
	തിരികെ version_tag_to_സംഖ്യ(NUTTX_GIT_TAG_STR);
#അല്ലെങ്കിൽ
# error "px4_os_version not implemented for current OS"
#endif
}

സ്ഥിരം അക്ഷരം *px4_os_version_string(ശൂന്യം)
{
#എങ്കിൽ defined(__PX4_NUTTX)
	തിരികെ NUTTX_GIT_VERSION_STR;
#അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
#endif
}

സ്ഥിരം അക്ഷരം *px4_os_name(ശൂന്യം)
{
#എങ്കിൽ defined(__PX4_DARWIN)
	തിരികെ "Darwin";
#അല്ലെങ്കിലെങ്കിൽ defined(__PX4_LINUX)
	തിരികെ "Linux";
#അല്ലെങ്കിലെങ്കിൽ defined(__PX4_QURT)
	തിരികെ "QuRT";
#അല്ലെങ്കിലെങ്കിൽ defined(__PX4_NUTTX)
	തിരികെ "NuttX";
#അല്ലെങ്കിലെങ്കിൽ defined(__PX4_CYGWIN)
	തിരികെ "Cygwin";
#അല്ലെങ്കിൽ
# error "px4_os_name not implemented for current OS"
#endif
}

സ്ഥിരം അക്ഷരം *px4_toolchain_name(ശൂന്യം)
{
#എങ്കിൽ defined(__clang__)
	തിരികെ "Clang/LLVM";
#അല്ലെങ്കിലെങ്കിൽ defined(__ICC) || defined(__INTEL_COMPILER)
	തിരികെ "Intel ICC";
#അല്ലെങ്കിലെങ്കിൽ defined(__GNUC__) || defined(__GNUG__)
	തിരികെ "GNU GCC";
#അല്ലെങ്കിലെങ്കിൽ defined(_MSC_VER)
	തിരികെ "MS Visual Studio";
#അല്ലെങ്കിൽ
	തിരികെ "Unknown";
#endif
}

സ്ഥിരം അക്ഷരം *px4_toolchain_version(ശൂന്യം)
{
#ifdef __VERSION__
	തിരികെ __VERSION__;
#അല്ലെങ്കിൽ
	തിരികെ "";
#endif
}

സ്ഥിരം അക്ഷരം *px4_firmware_version_string(ശൂന്യം)
{
	തിരികെ PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_ദ്വിമാന(ശൂന്യം)
{
	തിരികെ PX4_GIT_VERSION_ദ്വിമാന;
}


#ifdef MAVLINK_LIB_GIT_VERSION_ദ്വിമാന
uint64_t px4_mavlink_lib_version_ദ്വിമാന(ശൂന്യം)
{
	തിരികെ MAVLINK_LIB_GIT_VERSION_ദ്വിമാന;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_ദ്വിമാന(ശൂന്യം)
{
#ifdef NUTTX_GIT_VERSION_ദ്വിമാന
	തിരികെ NUTTX_GIT_VERSION_ദ്വിമാന;
#അല്ലെങ്കിൽ
	തിരികെ 0;
#endif
}

സ്ഥിരം അക്ഷരം *px4_firmware_oem_version_string(ശൂന്യം)
{
	തിരികെ PX4_GIT_OEM_VERSION_STR;
}
