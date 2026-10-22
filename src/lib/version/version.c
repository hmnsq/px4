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

#ಆದರೆ !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__PX4_LINUX)
#ಅನ್ತರ್ಭಾವ <sys/utsname.h>
#ಆದರೆ_ಅಂತ್ಯ

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
ಎಣಿಕೆ FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *px4_build_uri(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ STRINGIFY(BUILD_URI);
}

uint32_t ಆವೃತ್ತಿ_ಟ್ಯಾಗ್_ಗಮ್ಯ_ಸಂಖ್ಯೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಟ್ಯಾಗ್)
{
	uint32_t ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ = 0;

	int16_t ಬಫರ್_2 = -1;
	size_t ಬಫರ್_counter = 0;
	size_t dash_ಎಣಿಕೆ = 0;
	size_t ಬಿಂದು_ಎಣಿಕೆ = 0;
	ಅಕ್ಷರ ಆವೃತ್ತಿ[3] = {0, 0, 0};
	ಪೂರ್ಣಾಂಕ firmware_type = FIRMWARE_TYPE_RELEASE;

	ಗಾಗಿ (size_t i = 0; i < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಟ್ಯಾಗ್); i++) {
		ಆಯ್ಕೆ (ಟ್ಯಾಗ್[i]) {
		ಸ್ಥಿತಿ '-':
			dash_ಎಣಿಕೆ++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '.':
			ಬಿಂದು_ಎಣಿಕೆ++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'r':
			ಆದರೆ (i < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಟ್ಯಾಗ್) - 1 && ಟ್ಯಾಗ್[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 't':
			ಆದರೆ (i < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಟ್ಯಾಗ್) - 1 && ಟ್ಯಾಗ್[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ಇಲ್ಲದಿದ್ದರೆ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'v':
			ಆದರೆ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ ((dash_ಎಣಿಕೆ == 1 && ಬಿಂದು_ಎಣಿಕೆ == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ಎಣಿಕೆ == 2 && ಬಿಂದು_ಎಣಿಕೆ == 2) ||
	    (dash_ಎಣಿಕೆ == 3 && ಬಿಂದು_ಎಣಿಕೆ == 4) ||
	    (dash_ಎಣಿಕೆ == 4 && ಬಿಂದು_ಎಣಿಕೆ == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	ಗಾಗಿ (size_t i = 0; i < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಟ್ಯಾಗ್); i++) {
		ಆದರೆ (ಬಫರ್_counter > 2) {
			ಮುಂದುವರಿ;
		}

		ಆದರೆ (ಟ್ಯಾಗ್[i] >= '0' && ಟ್ಯಾಗ್[i] <= '9') {
			ಬಫರ್_2 = (ಬಫರ್_2 == -1) ? 0 : ಬಫರ್_2;
			ಬಫರ್_2 = ಬಫರ್_2 * 10 + (ಟ್ಯಾಗ್[i] - '0');

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (ಬಫರ್_2 >= 0) {
				ಆವೃತ್ತಿ[ಬಫರ್_counter] = ಬಫರ್_2;
				ಬಫರ್_counter++;
			}

			ಬಫರ್_2 = -1;
		}
	}

	ಆದರೆ (ಬಫರ್_2 >= 0) {
		ಆವೃತ್ತಿ[ಬಫರ್_counter] = ಬಫರ್_2;
		ಬಫರ್_counter++;
	}

	ಆದರೆ (ಬಫರ್_counter <= 0) {
		firmware_type = 0x00;
	}

	ಆದರೆ (ಬಫರ್_counter == 3 || ಬಫರ್_counter == 6) {
		ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ = ((uint8_t)ಆವೃತ್ತಿ[0] << 8 * 3) |
				 ((uint8_t)ಆವೃತ್ತಿ[1] << 8 * 2) |
				 ((uint8_t)ಆವೃತ್ತಿ[2] << 8 * 1) | firmware_type;

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ = 0;
	}

	ಹಿಂತಿರುಗಿ ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ;
}

uint32_t px4_firmware_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ ಆವೃತ್ತಿ_ಟ್ಯಾಗ್_ಗಮ್ಯ_ಸಂಖ್ಯೆ(PX4_GIT_ಟ್ಯಾಗ್_STR);
}

uint32_t ಆವೃತ್ತಿ_ಟ್ಯಾಗ್_ಗಮ್ಯ_vendor_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಟ್ಯಾಗ್)
{
	uint32_t ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ = 0;

	int16_t ಬಫರ್_2 = -1;
	size_t ಬಫರ್_counter = 0;
	ಅಕ್ಷರ ಆವೃತ್ತಿ[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_ಎಣಿಕೆ = 0;
	size_t ಬಿಂದು_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಂಕ firmware_type = FIRMWARE_TYPE_RELEASE;

	ಗಾಗಿ (size_t i = 0; i < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಟ್ಯಾಗ್); i++) {
		ಆಯ್ಕೆ (ಟ್ಯಾಗ್[i]) {
		ಸ್ಥಿತಿ '-':
			dash_ಎಣಿಕೆ++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '.':
			ಬಿಂದು_ಎಣಿಕೆ++;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'r':
			ಆದರೆ (i < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಟ್ಯಾಗ್) - 1 && ಟ್ಯಾಗ್[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 't':
			ಆದರೆ (i < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಟ್ಯಾಗ್) - 1 && ಟ್ಯಾಗ್[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} ಇಲ್ಲದಿದ್ದರೆ {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'v':
			ಆದರೆ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ ((dash_ಎಣಿಕೆ == 1 && ಬಿಂದು_ಎಣಿಕೆ == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_ಎಣಿಕೆ == 2 && ಬಿಂದು_ಎಣಿಕೆ == 2) ||
	    (dash_ಎಣಿಕೆ == 3 && ಬಿಂದು_ಎಣಿಕೆ == 4) ||
	    (dash_ಎಣಿಕೆ == 4 && ಬಿಂದು_ಎಣಿಕೆ == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	ಗಾಗಿ (size_t i = 0; i < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಟ್ಯಾಗ್); i++) {
		ಆದರೆ (ಬಫರ್_counter > 5) {
			ಮುಂದುವರಿ;
		}

		ಆದರೆ (ಟ್ಯಾಗ್[i] >= '0' && ಟ್ಯಾಗ್[i] <= '9') {
			ಬಫರ್_2 = (ಬಫರ್_2 == -1) ? 0 : ಬಫರ್_2;
			ಬಫರ್_2 = ಬಫರ್_2 * 10 + (ಟ್ಯಾಗ್[i] - '0');

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (ಬಫರ್_2 >= 0) {
				ಆದರೆ (ಬಫರ್_counter + 1 == 4 && ಟ್ಯಾಗ್[i] == '-') {
					ನಿಲ್ಲಿಸು;
				}

				ಆವೃತ್ತಿ[ಬಫರ್_counter] = ಬಫರ್_2;
				ಬಫರ್_counter++;
			}

			ಬಫರ್_2 = -1;
		}
	}

	ಆದರೆ (ಬಫರ್_2 >= 0 && (ಬಫರ್_counter + 1 == 3 || ಬಫರ್_counter + 1 == 6)) {
		ಆವೃತ್ತಿ[ಬಫರ್_counter] = ಬಫರ್_2;
		ಬಫರ್_counter++;
	}

	ಆದರೆ (ಬಫರ್_counter == 6) {
		ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ = ((uint8_t)ಆವೃತ್ತಿ[3] << 8 * 3) |
				 ((uint8_t)ಆವೃತ್ತಿ[4] << 8 * 2) |
				 ((uint8_t)ಆವೃತ್ತಿ[5] << 8 * 1) | firmware_type;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಫರ್_counter == 3) {
		ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ = firmware_type;

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ = 0;
	}

	ಹಿಂತಿರುಗಿ ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ;
}

uint32_t px4_firmware_vendor_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ ಆವೃತ್ತಿ_ಟ್ಯಾಗ್_ಗಮ್ಯ_vendor_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ(PX4_GIT_ಟ್ಯಾಗ್_STR);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *px4_firmware_git_branch(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
#ಆದರೆ defined(__PX4_NUTTX)
	ಹಿಂತಿರುಗಿ CONFIG_CDCACM_PRODUCTID;
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ 1;
#ಆದರೆ_ಅಂತ್ಯ
}

uint32_t px4_os_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
#ಆದರೆ defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	ಹಿಂತಿರುಗಿ 0; //TODO: implement version for Darwin, Cygwin, QuRT
#ಅಥವಾಆದರೆ defined(__PX4_LINUX)
	ರಚನೆ utsname name;

	ಆದರೆ (uname(&name) == 0) {
		ಅಕ್ಷರ *c = name.release;

		// cut the part after the first '-'
		ಆಗ (*c && *c != '-') {
			++c;
		}

		*c = 0;
		ಹಿಂತಿರುಗಿ ಆವೃತ್ತಿ_ಟ್ಯಾಗ್_ಗಮ್ಯ_ಸಂಖ್ಯೆ(name.release);

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ 0;
	}

#ಅಥವಾಆದರೆ defined(__PX4_NUTTX)
	ಹಿಂತಿರುಗಿ ಆವೃತ್ತಿ_ಟ್ಯಾಗ್_ಗಮ್ಯ_ಸಂಖ್ಯೆ(NUTTX_GIT_ಟ್ಯಾಗ್_STR);
#ಇಲ್ಲದಿದ್ದರೆ
# error "px4_os_version not implemented for current OS"
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *px4_os_ಆವೃತ್ತಿ_string(ಶೂನ್ಯ)
{
#ಆದರೆ defined(__PX4_NUTTX)
	ಹಿಂತಿರುಗಿ NUTTX_GIT_ಆವೃತ್ತಿ_STR;
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *px4_os_name(ಶೂನ್ಯ)
{
#ಆದರೆ defined(__PX4_DARWIN)
	ಹಿಂತಿರುಗಿ "Darwin";
#ಅಥವಾಆದರೆ defined(__PX4_LINUX)
	ಹಿಂತಿರುಗಿ "Linux";
#ಅಥವಾಆದರೆ defined(__PX4_QURT)
	ಹಿಂತಿರುಗಿ "QuRT";
#ಅಥವಾಆದರೆ defined(__PX4_NUTTX)
	ಹಿಂತಿರುಗಿ "NuttX";
#ಅಥವಾಆದರೆ defined(__PX4_CYGWIN)
	ಹಿಂತಿರುಗಿ "Cygwin";
#ಇಲ್ಲದಿದ್ದರೆ
# error "px4_os_name not implemented for current OS"
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *px4_toolchain_name(ಶೂನ್ಯ)
{
#ಆದರೆ defined(__clang__)
	ಹಿಂತಿರುಗಿ "Clang/LLVM";
#ಅಥವಾಆದರೆ defined(__ICC) || defined(__INTEL_COMPILER)
	ಹಿಂತಿರುಗಿ "Intel ICC";
#ಅಥವಾಆದರೆ defined(__GNUC__) || defined(__GNUG__)
	ಹಿಂತಿರುಗಿ "GNU GCC";
#ಅಥವಾಆದರೆ defined(_MSC_VER)
	ಹಿಂತಿರುಗಿ "MS Visual Studio";
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ "Unknown";
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *px4_toolchain_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VERSION__
	ಹಿಂತಿರುಗಿ __VERSION__;
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ "";
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *px4_firmware_ಆವೃತ್ತಿ_string(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ PX4_GIT_ಆವೃತ್ತಿ_STR;
}

uint64_t px4_firmware_ಆವೃತ್ತಿ_ದ್ವಿಮಾನ(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ PX4_GIT_ಆವೃತ್ತಿ_ದ್ವಿಮಾನ;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MAVLINK_LIB_GIT_ಆವೃತ್ತಿ_ದ್ವಿಮಾನ
uint64_t px4_mavlink_lib_ಆವೃತ್ತಿ_ದ್ವಿಮಾನ(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ MAVLINK_LIB_GIT_ಆವೃತ್ತಿ_ದ್ವಿಮಾನ;
}
#ಆದರೆ_ಅಂತ್ಯ /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_ಆವೃತ್ತಿ_ದ್ವಿಮಾನ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NUTTX_GIT_ಆವೃತ್ತಿ_ದ್ವಿಮಾನ
	ಹಿಂತಿರುಗಿ NUTTX_GIT_ಆವೃತ್ತಿ_ದ್ವಿಮಾನ;
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *px4_firmware_oem_ಆವೃತ್ತಿ_string(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ PX4_GIT_OEM_ಆವೃತ್ತಿ_STR;
}
