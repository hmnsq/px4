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

#شامل "version.h"

#شامل "build_git_version.h"

#شامل <string.h>

#اگر !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#اگر defined(__PX4_LINUX)
#شامل <sys/utsname.h>
#endif

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
شمارش FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

ثابت نویسه *px4_build_uri(تهی)
{
	بازگشت STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_عدد(ثابت نویسه *tag)
{
	uint32_t version_عدد = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_شمار = 0;
	size_t point_شمار = 0;
	نویسه version[3] = {0, 0, 0};
	صحیح firmware_type = FIRMWARE_TYPE_RELEASE;

	برای (size_t i = 0; i < strlen(tag); i++) {
		انتخاب (tag[i]) {
		حالت '-':
			dash_شمار++;
			بشکن;

		حالت '.':
			point_شمار++;
			بشکن;

		حالت 'r':
			اگر (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			بشکن;

		حالت 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			بشکن;

		حالت 't':
			اگر (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} وگرنه {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			بشکن;

		حالت 'v':
			اگر (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			بشکن;

		پیشفرض:
			بشکن;
		}
	}

	اگر ((dash_شمار == 1 && point_شمار == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_شمار == 2 && point_شمار == 2) ||
	    (dash_شمار == 3 && point_شمار == 4) ||
	    (dash_شمار == 4 && point_شمار == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	برای (size_t i = 0; i < strlen(tag); i++) {
		اگر (buffer_counter > 2) {
			ادامه;
		}

		اگر (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} وگرنه {
			اگر (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	اگر (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	اگر (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	اگر (buffer_counter == 3 || buffer_counter == 6) {
		version_عدد = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} وگرنه {
		version_عدد = 0;
	}

	بازگشت version_عدد;
}

uint32_t px4_firmware_version(تهی)
{
	بازگشت version_tag_to_عدد(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_عدد(ثابت نویسه *tag)
{
	uint32_t version_عدد = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	نویسه version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_شمار = 0;
	size_t point_شمار = 0;
	صحیح firmware_type = FIRMWARE_TYPE_RELEASE;

	برای (size_t i = 0; i < strlen(tag); i++) {
		انتخاب (tag[i]) {
		حالت '-':
			dash_شمار++;
			بشکن;

		حالت '.':
			point_شمار++;
			بشکن;

		حالت 'r':
			اگر (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			بشکن;

		حالت 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			بشکن;

		حالت 't':
			اگر (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} وگرنه {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			بشکن;

		حالت 'v':
			اگر (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			بشکن;

		پیشفرض:
			بشکن;
		}
	}

	اگر ((dash_شمار == 1 && point_شمار == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_شمار == 2 && point_شمار == 2) ||
	    (dash_شمار == 3 && point_شمار == 4) ||
	    (dash_شمار == 4 && point_شمار == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	برای (size_t i = 0; i < strlen(tag); i++) {
		اگر (buffer_counter > 5) {
			ادامه;
		}

		اگر (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} وگرنه {
			اگر (buffer >= 0) {
				اگر (buffer_counter + 1 == 4 && tag[i] == '-') {
					بشکن;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	اگر (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	اگر (buffer_counter == 6) {
		version_عدد = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} وگرنه اگر (buffer_counter == 3) {
		version_عدد = firmware_type;

	} وگرنه {
		version_عدد = 0;
	}

	بازگشت version_عدد;
}

uint32_t px4_firmware_vendor_version(تهی)
{
	بازگشت version_tag_to_vendor_version_عدد(PX4_GIT_TAG_STR);
}

ثابت نویسه *px4_firmware_git_branch(تهی)
{
	بازگشت PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(تهی)
{
#اگر defined(__PX4_NUTTX)
	بازگشت CONFIG_CDCACM_PRODUCTID;
#وگرنه
	بازگشت 1;
#endif
}

uint32_t px4_os_version(تهی)
{
#اگر defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	بازگشت 0; //TODO: implement version for Darwin, Cygwin, QuRT
#وگرنهاگر defined(__PX4_LINUX)
	ساختار utsname name;

	اگر (uname(&name) == 0) {
		نویسه *c = name.release;

		// cut the part after the first '-'
		وقتی (*c && *c != '-') {
			++c;
		}

		*c = 0;
		بازگشت version_tag_to_عدد(name.release);

	} وگرنه {
		بازگشت 0;
	}

#وگرنهاگر defined(__PX4_NUTTX)
	بازگشت version_tag_to_عدد(NUTTX_GIT_TAG_STR);
#وگرنه
# error "px4_os_version not implemented for current OS"
#endif
}

ثابت نویسه *px4_os_version_string(تهی)
{
#اگر defined(__PX4_NUTTX)
	بازگشت NUTTX_GIT_VERSION_STR;
#وگرنه
	بازگشت پوچ;
#endif
}

ثابت نویسه *px4_os_name(تهی)
{
#اگر defined(__PX4_DARWIN)
	بازگشت "Darwin";
#وگرنهاگر defined(__PX4_LINUX)
	بازگشت "Linux";
#وگرنهاگر defined(__PX4_QURT)
	بازگشت "QuRT";
#وگرنهاگر defined(__PX4_NUTTX)
	بازگشت "NuttX";
#وگرنهاگر defined(__PX4_CYGWIN)
	بازگشت "Cygwin";
#وگرنه
# error "px4_os_name not implemented for current OS"
#endif
}

ثابت نویسه *px4_toolchain_name(تهی)
{
#اگر defined(__clang__)
	بازگشت "Clang/LLVM";
#وگرنهاگر defined(__ICC) || defined(__INTEL_COMPILER)
	بازگشت "Intel ICC";
#وگرنهاگر defined(__GNUC__) || defined(__GNUG__)
	بازگشت "GNU GCC";
#وگرنهاگر defined(_MSC_VER)
	بازگشت "MS Visual Studio";
#وگرنه
	بازگشت "Unknown";
#endif
}

ثابت نویسه *px4_toolchain_version(تهی)
{
#ifdef __VERSION__
	بازگشت __VERSION__;
#وگرنه
	بازگشت "";
#endif
}

ثابت نویسه *px4_firmware_version_string(تهی)
{
	بازگشت PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_دودویی(تهی)
{
	بازگشت PX4_GIT_VERSION_دودویی;
}


#ifdef MAVLINK_LIB_GIT_VERSION_دودویی
uint64_t px4_mavlink_lib_version_دودویی(تهی)
{
	بازگشت MAVLINK_LIB_GIT_VERSION_دودویی;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_دودویی(تهی)
{
#ifdef NUTTX_GIT_VERSION_دودویی
	بازگشت NUTTX_GIT_VERSION_دودویی;
#وگرنه
	بازگشت 0;
#endif
}

ثابت نویسه *px4_firmware_oem_version_string(تهی)
{
	بازگشت PX4_GIT_OEM_VERSION_STR;
}
