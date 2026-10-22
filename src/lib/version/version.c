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

#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "build_git_version.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਜੇ !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#ਜੇ_ਅੰਤ

#ਜੇ defined(__PX4_LINUX)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/utsname.h>
#ਜੇ_ਅੰਤ

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
ਗਿਣਤੀ FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

ਅਚਲ ਅੱਖਰ *px4_build_uri(ਖਾਲੀ)
{
	ਵਾਪਸ STRINGIFY(BUILD_URI);
}

uint32_t ਸੰਸਕਰਣ_ਟੈਗ_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(ਅਚਲ ਅੱਖਰ *ਟੈਗ)
{
	uint32_t ਸੰਸਕਰਣ_ਸੰਖਿਆ = 0;

	int16_t ਬਫਰ_2 = -1;
	size_t ਬਫਰ_counter = 0;
	size_t dash_ਗਿਣਤੀ = 0;
	size_t ਬਿੰਦੂ_ਗਿਣਤੀ = 0;
	ਅੱਖਰ ਸੰਸਕਰਣ[3] = {0, 0, 0};
	ਪੂਰਨ firmware_type = FIRMWARE_TYPE_RELEASE;

	ਲਈ (size_t i = 0; i < ਲੜੀ_ਲੰਬਾਈ(ਟੈਗ); i++) {
		ਚੋਣ (ਟੈਗ[i]) {
		ਹਾਲਤ '-':
			dash_ਗਿਣਤੀ++;
			ਰੋਕੋ;

		ਹਾਲਤ '.':
			ਬਿੰਦੂ_ਗਿਣਤੀ++;
			ਰੋਕੋ;

		ਹਾਲਤ 'r':
			ਜੇ (i < ਲੜੀ_ਲੰਬਾਈ(ਟੈਗ) - 1 && ਟੈਗ[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ਰੋਕੋ;

		ਹਾਲਤ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ਰੋਕੋ;

		ਹਾਲਤ 't':
			ਜੇ (i < ਲੜੀ_ਲੰਬਾਈ(ਟੈਗ) - 1 && ਟੈਗ[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ਨਹੀਂਤਾਂ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ਰੋਕੋ;

		ਹਾਲਤ 'v':
			ਜੇ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ਰੋਕੋ;

		ਮੂਲ:
			ਰੋਕੋ;
		}
	}

	ਜੇ ((dash_ਗਿਣਤੀ == 1 && ਬਿੰਦੂ_ਗਿਣਤੀ == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ਗਿਣਤੀ == 2 && ਬਿੰਦੂ_ਗਿਣਤੀ == 2) ||
	    (dash_ਗਿਣਤੀ == 3 && ਬਿੰਦੂ_ਗਿਣਤੀ == 4) ||
	    (dash_ਗਿਣਤੀ == 4 && ਬਿੰਦੂ_ਗਿਣਤੀ == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	ਲਈ (size_t i = 0; i < ਲੜੀ_ਲੰਬਾਈ(ਟੈਗ); i++) {
		ਜੇ (ਬਫਰ_counter > 2) {
			ਜਾਰੀ;
		}

		ਜੇ (ਟੈਗ[i] >= '0' && ਟੈਗ[i] <= '9') {
			ਬਫਰ_2 = (ਬਫਰ_2 == -1) ? 0 : ਬਫਰ_2;
			ਬਫਰ_2 = ਬਫਰ_2 * 10 + (ਟੈਗ[i] - '0');

		} ਨਹੀਂਤਾਂ {
			ਜੇ (ਬਫਰ_2 >= 0) {
				ਸੰਸਕਰਣ[ਬਫਰ_counter] = ਬਫਰ_2;
				ਬਫਰ_counter++;
			}

			ਬਫਰ_2 = -1;
		}
	}

	ਜੇ (ਬਫਰ_2 >= 0) {
		ਸੰਸਕਰਣ[ਬਫਰ_counter] = ਬਫਰ_2;
		ਬਫਰ_counter++;
	}

	ਜੇ (ਬਫਰ_counter <= 0) {
		firmware_type = 0x00;
	}

	ਜੇ (ਬਫਰ_counter == 3 || ਬਫਰ_counter == 6) {
		ਸੰਸਕਰਣ_ਸੰਖਿਆ = ((uint8_t)ਸੰਸਕਰਣ[0] << 8 * 3) |
				 ((uint8_t)ਸੰਸਕਰਣ[1] << 8 * 2) |
				 ((uint8_t)ਸੰਸਕਰਣ[2] << 8 * 1) | firmware_type;

	} ਨਹੀਂਤਾਂ {
		ਸੰਸਕਰਣ_ਸੰਖਿਆ = 0;
	}

	ਵਾਪਸ ਸੰਸਕਰਣ_ਸੰਖਿਆ;
}

uint32_t px4_firmware_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
	ਵਾਪਸ ਸੰਸਕਰਣ_ਟੈਗ_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(PX4_GIT_ਟੈਗ_STR);
}

uint32_t ਸੰਸਕਰਣ_ਟੈਗ_ਮੰਜ਼ਿਲ_vendor_ਸੰਸਕਰਣ_ਸੰਖਿਆ(ਅਚਲ ਅੱਖਰ *ਟੈਗ)
{
	uint32_t ਸੰਸਕਰਣ_ਸੰਖਿਆ = 0;

	int16_t ਬਫਰ_2 = -1;
	size_t ਬਫਰ_counter = 0;
	ਅੱਖਰ ਸੰਸਕਰਣ[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_ਗਿਣਤੀ = 0;
	size_t ਬਿੰਦੂ_ਗਿਣਤੀ = 0;
	ਪੂਰਨ firmware_type = FIRMWARE_TYPE_RELEASE;

	ਲਈ (size_t i = 0; i < ਲੜੀ_ਲੰਬਾਈ(ਟੈਗ); i++) {
		ਚੋਣ (ਟੈਗ[i]) {
		ਹਾਲਤ '-':
			dash_ਗਿਣਤੀ++;
			ਰੋਕੋ;

		ਹਾਲਤ '.':
			ਬਿੰਦੂ_ਗਿਣਤੀ++;
			ਰੋਕੋ;

		ਹਾਲਤ 'r':
			ਜੇ (i < ਲੜੀ_ਲੰਬਾਈ(ਟੈਗ) - 1 && ਟੈਗ[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ਰੋਕੋ;

		ਹਾਲਤ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ਰੋਕੋ;

		ਹਾਲਤ 't':
			ਜੇ (i < ਲੜੀ_ਲੰਬਾਈ(ਟੈਗ) - 1 && ਟੈਗ[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ਨਹੀਂਤਾਂ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ਰੋਕੋ;

		ਹਾਲਤ 'v':
			ਜੇ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ਰੋਕੋ;

		ਮੂਲ:
			ਰੋਕੋ;
		}
	}

	ਜੇ ((dash_ਗਿਣਤੀ == 1 && ਬਿੰਦੂ_ਗਿਣਤੀ == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ਗਿਣਤੀ == 2 && ਬਿੰਦੂ_ਗਿਣਤੀ == 2) ||
	    (dash_ਗਿਣਤੀ == 3 && ਬਿੰਦੂ_ਗਿਣਤੀ == 4) ||
	    (dash_ਗਿਣਤੀ == 4 && ਬਿੰਦੂ_ਗਿਣਤੀ == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	ਲਈ (size_t i = 0; i < ਲੜੀ_ਲੰਬਾਈ(ਟੈਗ); i++) {
		ਜੇ (ਬਫਰ_counter > 5) {
			ਜਾਰੀ;
		}

		ਜੇ (ਟੈਗ[i] >= '0' && ਟੈਗ[i] <= '9') {
			ਬਫਰ_2 = (ਬਫਰ_2 == -1) ? 0 : ਬਫਰ_2;
			ਬਫਰ_2 = ਬਫਰ_2 * 10 + (ਟੈਗ[i] - '0');

		} ਨਹੀਂਤਾਂ {
			ਜੇ (ਬਫਰ_2 >= 0) {
				ਜੇ (ਬਫਰ_counter + 1 == 4 && ਟੈਗ[i] == '-') {
					ਰੋਕੋ;
				}

				ਸੰਸਕਰਣ[ਬਫਰ_counter] = ਬਫਰ_2;
				ਬਫਰ_counter++;
			}

			ਬਫਰ_2 = -1;
		}
	}

	ਜੇ (ਬਫਰ_2 >= 0 && (ਬਫਰ_counter + 1 == 3 || ਬਫਰ_counter + 1 == 6)) {
		ਸੰਸਕਰਣ[ਬਫਰ_counter] = ਬਫਰ_2;
		ਬਫਰ_counter++;
	}

	ਜੇ (ਬਫਰ_counter == 6) {
		ਸੰਸਕਰਣ_ਸੰਖਿਆ = ((uint8_t)ਸੰਸਕਰਣ[3] << 8 * 3) |
				 ((uint8_t)ਸੰਸਕਰਣ[4] << 8 * 2) |
				 ((uint8_t)ਸੰਸਕਰਣ[5] << 8 * 1) | firmware_type;

	} ਨਹੀਂਤਾਂ ਜੇ (ਬਫਰ_counter == 3) {
		ਸੰਸਕਰਣ_ਸੰਖਿਆ = firmware_type;

	} ਨਹੀਂਤਾਂ {
		ਸੰਸਕਰਣ_ਸੰਖਿਆ = 0;
	}

	ਵਾਪਸ ਸੰਸਕਰਣ_ਸੰਖਿਆ;
}

uint32_t px4_firmware_vendor_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
	ਵਾਪਸ ਸੰਸਕਰਣ_ਟੈਗ_ਮੰਜ਼ਿਲ_vendor_ਸੰਸਕਰਣ_ਸੰਖਿਆ(PX4_GIT_ਟੈਗ_STR);
}

ਅਚਲ ਅੱਖਰ *px4_firmware_git_branch(ਖਾਲੀ)
{
	ਵਾਪਸ PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
#ਜੇ defined(__PX4_NUTTX)
	ਵਾਪਸ CONFIG_CDCACM_PRODUCTID;
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ 1;
#ਜੇ_ਅੰਤ
}

uint32_t px4_os_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
#ਜੇ defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	ਵਾਪਸ 0; //TODO: implement version for Darwin, Cygwin, QuRT
#ਜਾਂਜੇ defined(__PX4_LINUX)
	ਬਣਤਰ utsname name;

	ਜੇ (uname(&name) == 0) {
		ਅੱਖਰ *c = name.release;

		// cut the part after the first '-'
		ਜਦੋਂ (*c && *c != '-') {
			++c;
		}

		*c = 0;
		ਵਾਪਸ ਸੰਸਕਰਣ_ਟੈਗ_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(name.release);

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ 0;
	}

#ਜਾਂਜੇ defined(__PX4_NUTTX)
	ਵਾਪਸ ਸੰਸਕਰਣ_ਟੈਗ_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(NUTTX_GIT_ਟੈਗ_STR);
#ਨਹੀਂਤਾਂ
# error "px4_os_version not implemented for current OS"
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *px4_os_ਸੰਸਕਰਣ_string(ਖਾਲੀ)
{
#ਜੇ defined(__PX4_NUTTX)
	ਵਾਪਸ NUTTX_GIT_ਸੰਸਕਰਣ_STR;
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *px4_os_name(ਖਾਲੀ)
{
#ਜੇ defined(__PX4_DARWIN)
	ਵਾਪਸ "Darwin";
#ਜਾਂਜੇ defined(__PX4_LINUX)
	ਵਾਪਸ "Linux";
#ਜਾਂਜੇ defined(__PX4_QURT)
	ਵਾਪਸ "QuRT";
#ਜਾਂਜੇ defined(__PX4_NUTTX)
	ਵਾਪਸ "NuttX";
#ਜਾਂਜੇ defined(__PX4_CYGWIN)
	ਵਾਪਸ "Cygwin";
#ਨਹੀਂਤਾਂ
# error "px4_os_name not implemented for current OS"
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *px4_toolchain_name(ਖਾਲੀ)
{
#ਜੇ defined(__clang__)
	ਵਾਪਸ "Clang/LLVM";
#ਜਾਂਜੇ defined(__ICC) || defined(__INTEL_COMPILER)
	ਵਾਪਸ "Intel ICC";
#ਜਾਂਜੇ defined(__GNUC__) || defined(__GNUG__)
	ਵਾਪਸ "GNU GCC";
#ਜਾਂਜੇ defined(_MSC_VER)
	ਵਾਪਸ "MS Visual Studio";
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ "Unknown";
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *px4_toolchain_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VERSION__
	ਵਾਪਸ __VERSION__;
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ "";
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *px4_firmware_ਸੰਸਕਰਣ_string(ਖਾਲੀ)
{
	ਵਾਪਸ PX4_GIT_ਸੰਸਕਰਣ_STR;
}

uint64_t px4_firmware_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ(ਖਾਲੀ)
{
	ਵਾਪਸ PX4_GIT_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MAVLINK_LIB_GIT_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ
uint64_t px4_mavlink_lib_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ(ਖਾਲੀ)
{
	ਵਾਪਸ MAVLINK_LIB_GIT_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ;
}
#ਜੇ_ਅੰਤ /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NUTTX_GIT_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ
	ਵਾਪਸ NUTTX_GIT_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ;
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *px4_firmware_oem_ਸੰਸਕਰਣ_string(ਖਾਲੀ)
{
	ਵਾਪਸ PX4_GIT_OEM_ਸੰਸਕਰਣ_STR;
}
