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

#जइ !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#जइ defined(__PX4_LINUX)
#अन्तर्भाव <sys/utsname.h>
#endif

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

uint32_t version_tag_to_सङ्ख्या(ध्रुव वर्ण *tag)
{
	uint32_t version_सङ्ख्या = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	size_t dash_गणनम् = 0;
	size_t point_गणनम् = 0;
	वर्ण version[3] = {0, 0, 0};
	पूर्णाङ्क firmware_type = FIRMWARE_TYPE_RELEASE;

	प्रति (size_t i = 0; i < strlen(tag); i++) {
		विकल्प (tag[i]) {
		पक्ष '-':
			dash_गणनम्++;
			भङ्ग;

		पक्ष '.':
			point_गणनम्++;
			भङ्ग;

		पक्ष 'r':
			जइ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			भङ्ग;

		पक्ष 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			भङ्ग;

		पक्ष 't':
			जइ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} अण्णहा {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			भङ्ग;

		पक्ष 'v':
			जइ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			भङ्ग;

		स्वभाव:
			भङ्ग;
		}
	}

	जइ ((dash_गणनम् == 1 && point_गणनम् == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_गणनम् == 2 && point_गणनम् == 2) ||
	    (dash_गणनम् == 3 && point_गणनम् == 4) ||
	    (dash_गणनम् == 4 && point_गणनम् == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	प्रति (size_t i = 0; i < strlen(tag); i++) {
		जइ (buffer_counter > 2) {
			अनुवर्त;
		}

		जइ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} अण्णहा {
			जइ (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	जइ (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	जइ (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	जइ (buffer_counter == 3 || buffer_counter == 6) {
		version_सङ्ख्या = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} अण्णहा {
		version_सङ्ख्या = 0;
	}

	प्रत्यागम version_सङ्ख्या;
}

uint32_t px4_firmware_version(शून्य)
{
	प्रत्यागम version_tag_to_सङ्ख्या(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_सङ्ख्या(ध्रुव वर्ण *tag)
{
	uint32_t version_सङ्ख्या = 0;

	int16_t buffer = -1;
	size_t buffer_counter = 0;
	वर्ण version[6] = {0, 0, 0, 0, 0, 0};
	size_t dash_गणनम् = 0;
	size_t point_गणनम् = 0;
	पूर्णाङ्क firmware_type = FIRMWARE_TYPE_RELEASE;

	प्रति (size_t i = 0; i < strlen(tag); i++) {
		विकल्प (tag[i]) {
		पक्ष '-':
			dash_गणनम्++;
			भङ्ग;

		पक्ष '.':
			point_गणनम्++;
			भङ्ग;

		पक्ष 'r':
			जइ (i < strlen(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			भङ्ग;

		पक्ष 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			भङ्ग;

		पक्ष 't':
			जइ (i < strlen(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} अण्णहा {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			भङ्ग;

		पक्ष 'v':
			जइ (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			भङ्ग;

		स्वभाव:
			भङ्ग;
		}
	}

	जइ ((dash_गणनम् == 1 && point_गणनम् == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_गणनम् == 2 && point_गणनम् == 2) ||
	    (dash_गणनम् == 3 && point_गणनम् == 4) ||
	    (dash_गणनम् == 4 && point_गणनम् == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	प्रति (size_t i = 0; i < strlen(tag); i++) {
		जइ (buffer_counter > 5) {
			अनुवर्त;
		}

		जइ (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} अण्णहा {
			जइ (buffer >= 0) {
				जइ (buffer_counter + 1 == 4 && tag[i] == '-') {
					भङ्ग;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	जइ (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	जइ (buffer_counter == 6) {
		version_सङ्ख्या = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} अण्णहा जइ (buffer_counter == 3) {
		version_सङ्ख्या = firmware_type;

	} अण्णहा {
		version_सङ्ख्या = 0;
	}

	प्रत्यागम version_सङ्ख्या;
}

uint32_t px4_firmware_vendor_version(शून्य)
{
	प्रत्यागम version_tag_to_vendor_version_सङ्ख्या(PX4_GIT_TAG_STR);
}

ध्रुव वर्ण *px4_firmware_git_branch(शून्य)
{
	प्रत्यागम PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(शून्य)
{
#जइ defined(__PX4_NUTTX)
	प्रत्यागम CONFIG_CDCACM_PRODUCTID;
#अण्णहा
	प्रत्यागम 1;
#endif
}

uint32_t px4_os_version(शून्य)
{
#जइ defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	प्रत्यागम 0; //TODO: implement version for Darwin, Cygwin, QuRT
#नोचेद्यदि defined(__PX4_LINUX)
	संरचना utsname name;

	जइ (uname(&name) == 0) {
		वर्ण *c = name.release;

		// cut the part after the first '-'
		जाव (*c && *c != '-') {
			++c;
		}

		*c = 0;
		प्रत्यागम version_tag_to_सङ्ख्या(name.release);

	} अण्णहा {
		प्रत्यागम 0;
	}

#नोचेद्यदि defined(__PX4_NUTTX)
	प्रत्यागम version_tag_to_सङ्ख्या(NUTTX_GIT_TAG_STR);
#अण्णहा
# error "px4_os_version not implemented for current OS"
#endif
}

ध्रुव वर्ण *px4_os_version_string(शून्य)
{
#जइ defined(__PX4_NUTTX)
	प्रत्यागम NUTTX_GIT_VERSION_STR;
#अण्णहा
	प्रत्यागम रिक्त;
#endif
}

ध्रुव वर्ण *px4_os_name(शून्य)
{
#जइ defined(__PX4_DARWIN)
	प्रत्यागम "Darwin";
#नोचेद्यदि defined(__PX4_LINUX)
	प्रत्यागम "Linux";
#नोचेद्यदि defined(__PX4_QURT)
	प्रत्यागम "QuRT";
#नोचेद्यदि defined(__PX4_NUTTX)
	प्रत्यागम "NuttX";
#नोचेद्यदि defined(__PX4_CYGWIN)
	प्रत्यागम "Cygwin";
#अण्णहा
# error "px4_os_name not implemented for current OS"
#endif
}

ध्रुव वर्ण *px4_toolchain_name(शून्य)
{
#जइ defined(__clang__)
	प्रत्यागम "Clang/LLVM";
#नोचेद्यदि defined(__ICC) || defined(__INTEL_COMPILER)
	प्रत्यागम "Intel ICC";
#नोचेद्यदि defined(__GNUC__) || defined(__GNUG__)
	प्रत्यागम "GNU GCC";
#नोचेद्यदि defined(_MSC_VER)
	प्रत्यागम "MS Visual Studio";
#अण्णहा
	प्रत्यागम "Unknown";
#endif
}

ध्रुव वर्ण *px4_toolchain_version(शून्य)
{
#ifdef __VERSION__
	प्रत्यागम __VERSION__;
#अण्णहा
	प्रत्यागम "";
#endif
}

ध्रुव वर्ण *px4_firmware_version_string(शून्य)
{
	प्रत्यागम PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_द्विमान(शून्य)
{
	प्रत्यागम PX4_GIT_VERSION_द्विमान;
}


#ifdef MAVLINK_LIB_GIT_VERSION_द्विमान
uint64_t px4_mavlink_lib_version_द्विमान(शून्य)
{
	प्रत्यागम MAVLINK_LIB_GIT_VERSION_द्विमान;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_द्विमान(शून्य)
{
#ifdef NUTTX_GIT_VERSION_द्विमान
	प्रत्यागम NUTTX_GIT_VERSION_द्विमान;
#अण्णहा
	प्रत्यागम 0;
#endif
}

ध्रुव वर्ण *px4_firmware_oem_version_string(शून्य)
{
	प्रत्यागम PX4_GIT_OEM_VERSION_STR;
}
