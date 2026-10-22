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

#అన్తర్భావ "version.h"

#అన్తర్భావ "build_git_version.h"

#అన్తర్భావ <string.h>

#ఒకవేళ !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#అయితే_ముగింపు

#ఒకవేళ defined(__PX4_LINUX)
#అన్తర్భావ <sys/utsname.h>
#అయితే_ముగింపు

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
గణన FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

స్థిరరాశి అక్షరం *px4_build_uri(శూన్య)
{
	తిరిగి STRINGIFY(BUILD_URI);
}

uint32_t సంస్కరణ_ట్యాగ్_గమ్యం_సంఖ్య(స్థిరరాశి అక్షరం *ట్యాగ్)
{
	uint32_t సంస్కరణ_సంఖ్య = 0;

	int16_t బఫరు_2 = -1;
	size_t బఫరు_counter = 0;
	size_t dash_లెక్క = 0;
	size_t బిందువు_లెక్క = 0;
	అక్షరం సంస్కరణ[3] = {0, 0, 0};
	పూర్ణాంకం firmware_type = FIRMWARE_TYPE_RELEASE;

	కొరకు (size_t i = 0; i < అక్షరశ్రేణి_పొడవు(ట్యాగ్); i++) {
		ఎంపిక (ట్యాగ్[i]) {
		స్థితి '-':
			dash_లెక్క++;
			ఆపు;

		స్థితి '.':
			బిందువు_లెక్క++;
			ఆపు;

		స్థితి 'r':
			ఒకవేళ (i < అక్షరశ్రేణి_పొడవు(ట్యాగ్) - 1 && ట్యాగ్[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ఆపు;

		స్థితి 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ఆపు;

		స్థితి 't':
			ఒకవేళ (i < అక్షరశ్రేణి_పొడవు(ట్యాగ్) - 1 && ట్యాగ్[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} లేకపోతే {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ఆపు;

		స్థితి 'v':
			ఒకవేళ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ఆపు;

		అప్రమేయ:
			ఆపు;
		}
	}

	ఒకవేళ ((dash_లెక్క == 1 && బిందువు_లెక్క == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_లెక్క == 2 && బిందువు_లెక్క == 2) ||
	    (dash_లెక్క == 3 && బిందువు_లెక్క == 4) ||
	    (dash_లెక్క == 4 && బిందువు_లెక్క == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	కొరకు (size_t i = 0; i < అక్షరశ్రేణి_పొడవు(ట్యాగ్); i++) {
		ఒకవేళ (బఫరు_counter > 2) {
			కొనసాగు;
		}

		ఒకవేళ (ట్యాగ్[i] >= '0' && ట్యాగ్[i] <= '9') {
			బఫరు_2 = (బఫరు_2 == -1) ? 0 : బఫరు_2;
			బఫరు_2 = బఫరు_2 * 10 + (ట్యాగ్[i] - '0');

		} లేకపోతే {
			ఒకవేళ (బఫరు_2 >= 0) {
				సంస్కరణ[బఫరు_counter] = బఫరు_2;
				బఫరు_counter++;
			}

			బఫరు_2 = -1;
		}
	}

	ఒకవేళ (బఫరు_2 >= 0) {
		సంస్కరణ[బఫరు_counter] = బఫరు_2;
		బఫరు_counter++;
	}

	ఒకవేళ (బఫరు_counter <= 0) {
		firmware_type = 0x00;
	}

	ఒకవేళ (బఫరు_counter == 3 || బఫరు_counter == 6) {
		సంస్కరణ_సంఖ్య = ((uint8_t)సంస్కరణ[0] << 8 * 3) |
				 ((uint8_t)సంస్కరణ[1] << 8 * 2) |
				 ((uint8_t)సంస్కరణ[2] << 8 * 1) | firmware_type;

	} లేకపోతే {
		సంస్కరణ_సంఖ్య = 0;
	}

	తిరిగి సంస్కరణ_సంఖ్య;
}

uint32_t px4_firmware_సంస్కరణ(శూన్య)
{
	తిరిగి సంస్కరణ_ట్యాగ్_గమ్యం_సంఖ్య(PX4_GIT_ట్యాగ్_STR);
}

uint32_t సంస్కరణ_ట్యాగ్_గమ్యం_vendor_సంస్కరణ_సంఖ్య(స్థిరరాశి అక్షరం *ట్యాగ్)
{
	uint32_t సంస్కరణ_సంఖ్య = 0;

	int16_t బఫరు_2 = -1;
	size_t బఫరు_counter = 0;
	అక్షరం సంస్కరణ[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_లెక్క = 0;
	size_t బిందువు_లెక్క = 0;
	పూర్ణాంకం firmware_type = FIRMWARE_TYPE_RELEASE;

	కొరకు (size_t i = 0; i < అక్షరశ్రేణి_పొడవు(ట్యాగ్); i++) {
		ఎంపిక (ట్యాగ్[i]) {
		స్థితి '-':
			dash_లెక్క++;
			ఆపు;

		స్థితి '.':
			బిందువు_లెక్క++;
			ఆపు;

		స్థితి 'r':
			ఒకవేళ (i < అక్షరశ్రేణి_పొడవు(ట్యాగ్) - 1 && ట్యాగ్[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			ఆపు;

		స్థితి 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			ఆపు;

		స్థితి 't':
			ఒకవేళ (i < అక్షరశ్రేణి_పొడవు(ట్యాగ్) - 1 && ట్యాగ్[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} లేకపోతే {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			ఆపు;

		స్థితి 'v':
			ఒకవేళ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			ఆపు;

		అప్రమేయ:
			ఆపు;
		}
	}

	ఒకవేళ ((dash_లెక్క == 1 && బిందువు_లెక్క == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_లెక్క == 2 && బిందువు_లెక్క == 2) ||
	    (dash_లెక్క == 3 && బిందువు_లెక్క == 4) ||
	    (dash_లెక్క == 4 && బిందువు_లెక్క == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	కొరకు (size_t i = 0; i < అక్షరశ్రేణి_పొడవు(ట్యాగ్); i++) {
		ఒకవేళ (బఫరు_counter > 5) {
			కొనసాగు;
		}

		ఒకవేళ (ట్యాగ్[i] >= '0' && ట్యాగ్[i] <= '9') {
			బఫరు_2 = (బఫరు_2 == -1) ? 0 : బఫరు_2;
			బఫరు_2 = బఫరు_2 * 10 + (ట్యాగ్[i] - '0');

		} లేకపోతే {
			ఒకవేళ (బఫరు_2 >= 0) {
				ఒకవేళ (బఫరు_counter + 1 == 4 && ట్యాగ్[i] == '-') {
					ఆపు;
				}

				సంస్కరణ[బఫరు_counter] = బఫరు_2;
				బఫరు_counter++;
			}

			బఫరు_2 = -1;
		}
	}

	ఒకవేళ (బఫరు_2 >= 0 && (బఫరు_counter + 1 == 3 || బఫరు_counter + 1 == 6)) {
		సంస్కరణ[బఫరు_counter] = బఫరు_2;
		బఫరు_counter++;
	}

	ఒకవేళ (బఫరు_counter == 6) {
		సంస్కరణ_సంఖ్య = ((uint8_t)సంస్కరణ[3] << 8 * 3) |
				 ((uint8_t)సంస్కరణ[4] << 8 * 2) |
				 ((uint8_t)సంస్కరణ[5] << 8 * 1) | firmware_type;

	} లేకపోతే ఒకవేళ (బఫరు_counter == 3) {
		సంస్కరణ_సంఖ్య = firmware_type;

	} లేకపోతే {
		సంస్కరణ_సంఖ్య = 0;
	}

	తిరిగి సంస్కరణ_సంఖ్య;
}

uint32_t px4_firmware_vendor_సంస్కరణ(శూన్య)
{
	తిరిగి సంస్కరణ_ట్యాగ్_గమ్యం_vendor_సంస్కరణ_సంఖ్య(PX4_GIT_ట్యాగ్_STR);
}

స్థిరరాశి అక్షరం *px4_firmware_git_branch(శూన్య)
{
	తిరిగి PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_సంస్కరణ(శూన్య)
{
#ఒకవేళ defined(__PX4_NUTTX)
	తిరిగి CONFIG_CDCACM_PRODUCTID;
#లేకపోతే
	తిరిగి 1;
#అయితే_ముగింపు
}

uint32_t px4_os_సంస్కరణ(శూన్య)
{
#ఒకవేళ defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	తిరిగి 0; //TODO: implement version for Darwin, Cygwin, QuRT
#లేదావేళ defined(__PX4_LINUX)
	నిర్మాణం utsname name;

	ఒకవేళ (uname(&name) == 0) {
		అక్షరం *c = name.release;

		// cut the part after the first '-'
		అయితే (*c && *c != '-') {
			++c;
		}

		*c = 0;
		తిరిగి సంస్కరణ_ట్యాగ్_గమ్యం_సంఖ్య(name.release);

	} లేకపోతే {
		తిరిగి 0;
	}

#లేదావేళ defined(__PX4_NUTTX)
	తిరిగి సంస్కరణ_ట్యాగ్_గమ్యం_సంఖ్య(NUTTX_GIT_ట్యాగ్_STR);
#లేకపోతే
# error "px4_os_version not implemented for current OS"
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *px4_os_సంస్కరణ_string(శూన్య)
{
#ఒకవేళ defined(__PX4_NUTTX)
	తిరిగి NUTTX_GIT_సంస్కరణ_STR;
#లేకపోతే
	తిరిగి ఖాళీ;
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *px4_os_name(శూన్య)
{
#ఒకవేళ defined(__PX4_DARWIN)
	తిరిగి "Darwin";
#లేదావేళ defined(__PX4_LINUX)
	తిరిగి "Linux";
#లేదావేళ defined(__PX4_QURT)
	తిరిగి "QuRT";
#లేదావేళ defined(__PX4_NUTTX)
	తిరిగి "NuttX";
#లేదావేళ defined(__PX4_CYGWIN)
	తిరిగి "Cygwin";
#లేకపోతే
# error "px4_os_name not implemented for current OS"
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *px4_toolchain_name(శూన్య)
{
#ఒకవేళ defined(__clang__)
	తిరిగి "Clang/LLVM";
#లేదావేళ defined(__ICC) || defined(__INTEL_COMPILER)
	తిరిగి "Intel ICC";
#లేదావేళ defined(__GNUC__) || defined(__GNUG__)
	తిరిగి "GNU GCC";
#లేదావేళ defined(_MSC_VER)
	తిరిగి "MS Visual Studio";
#లేకపోతే
	తిరిగి "Unknown";
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *px4_toolchain_సంస్కరణ(శూన్య)
{
#నిర్వచితమైతే __VERSION__
	తిరిగి __VERSION__;
#లేకపోతే
	తిరిగి "";
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *px4_firmware_సంస్కరణ_string(శూన్య)
{
	తిరిగి PX4_GIT_సంస్కరణ_STR;
}

uint64_t px4_firmware_సంస్కరణ_ద్విమాన(శూన్య)
{
	తిరిగి PX4_GIT_సంస్కరణ_ద్విమాన;
}


#నిర్వచితమైతే MAVLINK_LIB_GIT_సంస్కరణ_ద్విమాన
uint64_t px4_mavlink_lib_సంస్కరణ_ద్విమాన(శూన్య)
{
	తిరిగి MAVLINK_LIB_GIT_సంస్కరణ_ద్విమాన;
}
#అయితే_ముగింపు /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_సంస్కరణ_ద్విమాన(శూన్య)
{
#నిర్వచితమైతే NUTTX_GIT_సంస్కరణ_ద్విమాన
	తిరిగి NUTTX_GIT_సంస్కరణ_ద్విమాన;
#లేకపోతే
	తిరిగి 0;
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *px4_firmware_oem_సంస్కరణ_string(శూన్య)
{
	తిరిగి PX4_GIT_OEM_సంస్కరణ_STR;
}
