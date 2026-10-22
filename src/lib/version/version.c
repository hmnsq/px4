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

#अन्तर्भाव "version.h"

#अन्तर्भाव "build_git_version.h"

#अन्तर्भाव <string.h>

#यदि !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#यदि_अन्तः

#यदि defined(__PX4_LINUX)
#अन्तर्भाव <sys/utsname.h>
#यदि_अन्तः

// dev >= 0
// alpha >= 64
// beta >= 128
// release candidate >= 192
// release == 255
गणना FIRMWARE_TYPE {
	FIRMWARE_TYPE_DEV = 0,
	FIRMWARE_TYPE_ALPHA = 64,
	FIRMWARE_TYPE_BETA = 128,
	FIRMWARE_TYPE_RC = 192,
	FIRMWARE_TYPE_RELEASE = 255
};

ध्रुव वर्ण *px4_build_uri(शून्य)
{
	प्रत्यागम STRINGIFY(BUILD_URI);
}

uint32_t संस्करणम्_चिह्नकम्_गन्तव्यम्_सङ्ख्या(ध्रुव वर्ण *चिह्नकम्)
{
	uint32_t संस्करणम्_सङ्ख्या = 0;

	int16_t बफर_2 = -1;
	size_t बफर_गणक = 0;
	size_t dash_गणनम् = 0;
	size_t point_गणनम् = 0;
	वर्ण संस्करणम्[3] = {0, 0, 0};
	पूर्णाङ्क firmware_type = FIRMWARE_TYPE_RELEASE;

	प्रति (size_t i = 0; i < वाक्यदैर्घ्यम्(चिह्नकम्); i++) {
		विकल्प (चिह्नकम्[i]) {
		पक्ष '-':
			dash_गणनम्++;
			भङ्ग;

		पक्ष '.':
			point_गणनम्++;
			भङ्ग;

		पक्ष 'r':
			यदि (i < वाक्यदैर्घ्यम्(चिह्नकम्) - 1 && चिह्नकम्[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			भङ्ग;

		पक्ष 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			भङ्ग;

		पक्ष 't':
			यदि (i < वाक्यदैर्घ्यम्(चिह्नकम्) - 1 && चिह्नकम्[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} अन्यथा {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			भङ्ग;

		पक्ष 'v':
			यदि (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			भङ्ग;

		स्वभाव:
			भङ्ग;
		}
	}

	यदि ((dash_गणनम् == 1 && point_गणनम् == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_गणनम् == 2 && point_गणनम् == 2) ||
	    (dash_गणनम् == 3 && point_गणनम् == 4) ||
	    (dash_गणनम् == 4 && point_गणनम् == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	प्रति (size_t i = 0; i < वाक्यदैर्घ्यम्(चिह्नकम्); i++) {
		यदि (बफर_गणक > 2) {
			अनुवर्त;
		}

		यदि (चिह्नकम्[i] >= '0' && चिह्नकम्[i] <= '9') {
			बफर_2 = (बफर_2 == -1) ? 0 : बफर_2;
			बफर_2 = बफर_2 * 10 + (चिह्नकम्[i] - '0');

		} अन्यथा {
			यदि (बफर_2 >= 0) {
				संस्करणम्[बफर_गणक] = बफर_2;
				बफर_गणक++;
			}

			बफर_2 = -1;
		}
	}

	यदि (बफर_2 >= 0) {
		संस्करणम्[बफर_गणक] = बफर_2;
		बफर_गणक++;
	}

	यदि (बफर_गणक <= 0) {
		firmware_type = 0x00;
	}

	यदि (बफर_गणक == 3 || बफर_गणक == 6) {
		संस्करणम्_सङ्ख्या = ((uint8_t)संस्करणम्[0] << 8 * 3) |
				 ((uint8_t)संस्करणम्[1] << 8 * 2) |
				 ((uint8_t)संस्करणम्[2] << 8 * 1) | firmware_type;

	} अन्यथा {
		संस्करणम्_सङ्ख्या = 0;
	}

	प्रत्यागम संस्करणम्_सङ्ख्या;
}

uint32_t px4_firmware_संस्करणम्(शून्य)
{
	प्रत्यागम संस्करणम्_चिह्नकम्_गन्तव्यम्_सङ्ख्या(PX4_GIT_चिह्नकम्_STR);
}

uint32_t संस्करणम्_चिह्नकम्_गन्तव्यम्_vendor_संस्करणम्_सङ्ख्या(ध्रुव वर्ण *चिह्नकम्)
{
	uint32_t संस्करणम्_सङ्ख्या = 0;

	int16_t बफर_2 = -1;
	size_t बफर_गणक = 0;
	वर्ण संस्करणम्[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_गणनम् = 0;
	size_t point_गणनम् = 0;
	पूर्णाङ्क firmware_type = FIRMWARE_TYPE_RELEASE;

	प्रति (size_t i = 0; i < वाक्यदैर्घ्यम्(चिह्नकम्); i++) {
		विकल्प (चिह्नकम्[i]) {
		पक्ष '-':
			dash_गणनम्++;
			भङ्ग;

		पक्ष '.':
			point_गणनम्++;
			भङ्ग;

		पक्ष 'r':
			यदि (i < वाक्यदैर्घ्यम्(चिह्नकम्) - 1 && चिह्नकम्[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			भङ्ग;

		पक्ष 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			भङ्ग;

		पक्ष 't':
			यदि (i < वाक्यदैर्घ्यम्(चिह्नकम्) - 1 && चिह्नकम्[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} अन्यथा {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			भङ्ग;

		पक्ष 'v':
			यदि (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			भङ्ग;

		स्वभाव:
			भङ्ग;
		}
	}

	यदि ((dash_गणनम् == 1 && point_गणनम् == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_गणनम् == 2 && point_गणनम् == 2) ||
	    (dash_गणनम् == 3 && point_गणनम् == 4) ||
	    (dash_गणनम् == 4 && point_गणनम् == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	प्रति (size_t i = 0; i < वाक्यदैर्घ्यम्(चिह्नकम्); i++) {
		यदि (बफर_गणक > 5) {
			अनुवर्त;
		}

		यदि (चिह्नकम्[i] >= '0' && चिह्नकम्[i] <= '9') {
			बफर_2 = (बफर_2 == -1) ? 0 : बफर_2;
			बफर_2 = बफर_2 * 10 + (चिह्नकम्[i] - '0');

		} अन्यथा {
			यदि (बफर_2 >= 0) {
				यदि (बफर_गणक + 1 == 4 && चिह्नकम्[i] == '-') {
					भङ्ग;
				}

				संस्करणम्[बफर_गणक] = बफर_2;
				बफर_गणक++;
			}

			बफर_2 = -1;
		}
	}

	यदि (बफर_2 >= 0 && (बफर_गणक + 1 == 3 || बफर_गणक + 1 == 6)) {
		संस्करणम्[बफर_गणक] = बफर_2;
		बफर_गणक++;
	}

	यदि (बफर_गणक == 6) {
		संस्करणम्_सङ्ख्या = ((uint8_t)संस्करणम्[3] << 8 * 3) |
				 ((uint8_t)संस्करणम्[4] << 8 * 2) |
				 ((uint8_t)संस्करणम्[5] << 8 * 1) | firmware_type;

	} अन्यथा यदि (बफर_गणक == 3) {
		संस्करणम्_सङ्ख्या = firmware_type;

	} अन्यथा {
		संस्करणम्_सङ्ख्या = 0;
	}

	प्रत्यागम संस्करणम्_सङ्ख्या;
}

uint32_t px4_firmware_vendor_संस्करणम्(शून्य)
{
	प्रत्यागम संस्करणम्_चिह्नकम्_गन्तव्यम्_vendor_संस्करणम्_सङ्ख्या(PX4_GIT_चिह्नकम्_STR);
}

ध्रुव वर्ण *px4_firmware_git_branch(शून्य)
{
	प्रत्यागम PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_संस्करणम्(शून्य)
{
#यदि defined(__PX4_NUTTX)
	प्रत्यागम CONFIG_CDCACM_PRODUCTID;
#अन्यथा
	प्रत्यागम 1;
#यदि_अन्तः
}

uint32_t px4_os_संस्करणम्(शून्य)
{
#यदि defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	प्रत्यागम 0; //TODO: implement version for Darwin, Cygwin, QuRT
#नोचेद्यदि defined(__PX4_LINUX)
	संरचना utsname name;

	यदि (uname(&name) == 0) {
		वर्ण *c = name.release;

		// cut the part after the first '-'
		यावत् (*c && *c != '-') {
			++c;
		}

		*c = 0;
		प्रत्यागम संस्करणम्_चिह्नकम्_गन्तव्यम्_सङ्ख्या(name.release);

	} अन्यथा {
		प्रत्यागम 0;
	}

#नोचेद्यदि defined(__PX4_NUTTX)
	प्रत्यागम संस्करणम्_चिह्नकम्_गन्तव्यम्_सङ्ख्या(NUTTX_GIT_चिह्नकम्_STR);
#अन्यथा
# error "px4_os_version not implemented for current OS"
#यदि_अन्तः
}

ध्रुव वर्ण *px4_os_संस्करणम्_string(शून्य)
{
#यदि defined(__PX4_NUTTX)
	प्रत्यागम NUTTX_GIT_संस्करणम्_STR;
#अन्यथा
	प्रत्यागम रिक्त;
#यदि_अन्तः
}

ध्रुव वर्ण *px4_os_name(शून्य)
{
#यदि defined(__PX4_DARWIN)
	प्रत्यागम "Darwin";
#नोचेद्यदि defined(__PX4_LINUX)
	प्रत्यागम "Linux";
#नोचेद्यदि defined(__PX4_QURT)
	प्रत्यागम "QuRT";
#नोचेद्यदि defined(__PX4_NUTTX)
	प्रत्यागम "NuttX";
#नोचेद्यदि defined(__PX4_CYGWIN)
	प्रत्यागम "Cygwin";
#अन्यथा
# error "px4_os_name not implemented for current OS"
#यदि_अन्तः
}

ध्रुव वर्ण *px4_toolchain_name(शून्य)
{
#यदि defined(__clang__)
	प्रत्यागम "Clang/LLVM";
#नोचेद्यदि defined(__ICC) || defined(__INTEL_COMPILER)
	प्रत्यागम "Intel ICC";
#नोचेद्यदि defined(__GNUC__) || defined(__GNUG__)
	प्रत्यागम "GNU GCC";
#नोचेद्यदि defined(_MSC_VER)
	प्रत्यागम "MS Visual Studio";
#अन्यथा
	प्रत्यागम "Unknown";
#यदि_अन्तः
}

ध्रुव वर्ण *px4_toolchain_संस्करणम्(शून्य)
{
#यदि_परिभाषितम् __VERSION__
	प्रत्यागम __VERSION__;
#अन्यथा
	प्रत्यागम "";
#यदि_अन्तः
}

ध्रुव वर्ण *px4_firmware_संस्करणम्_string(शून्य)
{
	प्रत्यागम PX4_GIT_संस्करणम्_STR;
}

uint64_t px4_firmware_संस्करणम्_द्विमान(शून्य)
{
	प्रत्यागम PX4_GIT_संस्करणम्_द्विमान;
}


#यदि_परिभाषितम् MAVLINK_LIB_GIT_संस्करणम्_द्विमान
uint64_t px4_mavlink_lib_संस्करणम्_द्विमान(शून्य)
{
	प्रत्यागम MAVLINK_LIB_GIT_संस्करणम्_द्विमान;
}
#यदि_अन्तः /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_संस्करणम्_द्विमान(शून्य)
{
#यदि_परिभाषितम् NUTTX_GIT_संस्करणम्_द्विमान
	प्रत्यागम NUTTX_GIT_संस्करणम्_द्विमान;
#अन्यथा
	प्रत्यागम 0;
#यदि_अन्तः
}

ध्रुव वर्ण *px4_firmware_oem_संस्करणम्_string(शून्य)
{
	प्रत्यागम PX4_GIT_OEM_संस्करणम्_STR;
}
