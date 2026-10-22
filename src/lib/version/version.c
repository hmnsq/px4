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
#endif

#यदि defined(__PX4_LINUX)
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

अचर वर्ण *px4_build_uri(शून्य)
{
	फर्काऊ STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_सङ्ख्या(अचर वर्ण *tag)
{
	uint32_t version_सङ्ख्या = 0;

	int16_t buffer = -1;
	माप_प्रकार buffer_counter = 0;
	माप_प्रकार dash_गन्ती = 0;
	माप_प्रकार point_गन्ती = 0;
	वर्ण version[3] = {0, 0, 0};
	पूर्णाङ्क firmware_type = FIRMWARE_TYPE_RELEASE;

	लागि (माप_प्रकार i = 0; i < म_माप(tag); i++) {
		विकल्प (tag[i]) {
		पक्ष '-':
			dash_गन्ती++;
			रोक;

		पक्ष '.':
			point_गन्ती++;
			रोक;

		पक्ष 'r':
			यदि (i < म_माप(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			रोक;

		पक्ष 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			रोक;

		पक्ष 't':
			यदि (i < म_माप(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} अन्यथा {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			रोक;

		पक्ष 'v':
			यदि (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			रोक;

		स्वभाव:
			रोक;
		}
	}

	यदि ((dash_गन्ती == 1 && point_गन्ती == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_गन्ती == 2 && point_गन्ती == 2) ||
	    (dash_गन्ती == 3 && point_गन्ती == 4) ||
	    (dash_गन्ती == 4 && point_गन्ती == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	लागि (माप_प्रकार i = 0; i < म_माप(tag); i++) {
		यदि (buffer_counter > 2) {
			जारी;
		}

		यदि (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} अन्यथा {
			यदि (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	यदि (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	यदि (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	यदि (buffer_counter == 3 || buffer_counter == 6) {
		version_सङ्ख्या = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} अन्यथा {
		version_सङ्ख्या = 0;
	}

	फर्काऊ version_सङ्ख्या;
}

uint32_t px4_firmware_version(शून्य)
{
	फर्काऊ version_tag_to_सङ्ख्या(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_सङ्ख्या(अचर वर्ण *tag)
{
	uint32_t version_सङ्ख्या = 0;

	int16_t buffer = -1;
	माप_प्रकार buffer_counter = 0;
	वर्ण version[6] = {0, 0, 0, 0, 0, 0};
	माप_प्रकार dash_गन्ती = 0;
	माप_प्रकार point_गन्ती = 0;
	पूर्णाङ्क firmware_type = FIRMWARE_TYPE_RELEASE;

	लागि (माप_प्रकार i = 0; i < म_माप(tag); i++) {
		विकल्प (tag[i]) {
		पक्ष '-':
			dash_गन्ती++;
			रोक;

		पक्ष '.':
			point_गन्ती++;
			रोक;

		पक्ष 'r':
			यदि (i < म_माप(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			रोक;

		पक्ष 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			रोक;

		पक्ष 't':
			यदि (i < म_माप(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} अन्यथा {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			रोक;

		पक्ष 'v':
			यदि (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			रोक;

		स्वभाव:
			रोक;
		}
	}

	यदि ((dash_गन्ती == 1 && point_गन्ती == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_गन्ती == 2 && point_गन्ती == 2) ||
	    (dash_गन्ती == 3 && point_गन्ती == 4) ||
	    (dash_गन्ती == 4 && point_गन्ती == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	लागि (माप_प्रकार i = 0; i < म_माप(tag); i++) {
		यदि (buffer_counter > 5) {
			जारी;
		}

		यदि (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} अन्यथा {
			यदि (buffer >= 0) {
				यदि (buffer_counter + 1 == 4 && tag[i] == '-') {
					रोक;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	यदि (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	यदि (buffer_counter == 6) {
		version_सङ्ख्या = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} अन्यथा यदि (buffer_counter == 3) {
		version_सङ्ख्या = firmware_type;

	} अन्यथा {
		version_सङ्ख्या = 0;
	}

	फर्काऊ version_सङ्ख्या;
}

uint32_t px4_firmware_vendor_version(शून्य)
{
	फर्काऊ version_tag_to_vendor_version_सङ्ख्या(PX4_GIT_TAG_STR);
}

अचर वर्ण *px4_firmware_git_branch(शून्य)
{
	फर्काऊ PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(शून्य)
{
#यदि defined(__PX4_NUTTX)
	फर्काऊ CONFIG_CDCACM_PRODUCTID;
#अन्यथा
	फर्काऊ 1;
#endif
}

uint32_t px4_os_version(शून्य)
{
#यदि defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	फर्काऊ 0; //TODO: implement version for Darwin, Cygwin, QuRT
#अथवायदि defined(__PX4_LINUX)
	संरचना utsname name;

	यदि (uname(&name) == 0) {
		वर्ण *c = name.release;

		// cut the part after the first '-'
		जबसम्म (*c && *c != '-') {
			++c;
		}

		*c = 0;
		फर्काऊ version_tag_to_सङ्ख्या(name.release);

	} अन्यथा {
		फर्काऊ 0;
	}

#अथवायदि defined(__PX4_NUTTX)
	फर्काऊ version_tag_to_सङ्ख्या(NUTTX_GIT_TAG_STR);
#अन्यथा
# error "px4_os_version not implemented for current OS"
#endif
}

अचर वर्ण *px4_os_version_string(शून्य)
{
#यदि defined(__PX4_NUTTX)
	फर्काऊ NUTTX_GIT_VERSION_STR;
#अन्यथा
	फर्काऊ रिक्त;
#endif
}

अचर वर्ण *px4_os_name(शून्य)
{
#यदि defined(__PX4_DARWIN)
	फर्काऊ "Darwin";
#अथवायदि defined(__PX4_LINUX)
	फर्काऊ "Linux";
#अथवायदि defined(__PX4_QURT)
	फर्काऊ "QuRT";
#अथवायदि defined(__PX4_NUTTX)
	फर्काऊ "NuttX";
#अथवायदि defined(__PX4_CYGWIN)
	फर्काऊ "Cygwin";
#अन्यथा
# error "px4_os_name not implemented for current OS"
#endif
}

अचर वर्ण *px4_toolchain_name(शून्य)
{
#यदि defined(__clang__)
	फर्काऊ "Clang/LLVM";
#अथवायदि defined(__ICC) || defined(__INTEL_COMPILER)
	फर्काऊ "Intel ICC";
#अथवायदि defined(__GNUC__) || defined(__GNUG__)
	फर्काऊ "GNU GCC";
#अथवायदि defined(_MSC_VER)
	फर्काऊ "MS Visual Studio";
#अन्यथा
	फर्काऊ "Unknown";
#endif
}

अचर वर्ण *px4_toolchain_version(शून्य)
{
#ifdef __VERSION__
	फर्काऊ __VERSION__;
#अन्यथा
	फर्काऊ "";
#endif
}

अचर वर्ण *px4_firmware_version_string(शून्य)
{
	फर्काऊ PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_द्विआधारी(शून्य)
{
	फर्काऊ PX4_GIT_VERSION_द्विआधारी;
}


#ifdef MAVLINK_LIB_GIT_VERSION_द्विआधारी
uint64_t px4_mavlink_lib_version_द्विआधारी(शून्य)
{
	फर्काऊ MAVLINK_LIB_GIT_VERSION_द्विआधारी;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_द्विआधारी(शून्य)
{
#ifdef NUTTX_GIT_VERSION_द्विआधारी
	फर्काऊ NUTTX_GIT_VERSION_द्विआधारी;
#अन्यथा
	फर्काऊ 0;
#endif
}

अचर वर्ण *px4_firmware_oem_version_string(शून्य)
{
	फर्काऊ PX4_GIT_OEM_VERSION_STR;
}
