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

#जर !defined(CONFIG_CDCACM_PRODUCTID)
# define CONFIG_CDCACM_PRODUCTID 0
#endif

#जर defined(__PX4_LINUX)
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

स्थिरांक अक्षर *px4_build_uri(शून्य)
{
	परत STRINGIFY(BUILD_URI);
}

uint32_t version_tag_to_संख्या(स्थिरांक अक्षर *tag)
{
	uint32_t version_संख्या = 0;

	int16_t buffer = -1;
	माप_प्रकार buffer_counter = 0;
	माप_प्रकार dash_मोजणी = 0;
	माप_प्रकार point_मोजणी = 0;
	अक्षर version[3] = {0, 0, 0};
	पूर्णांक firmware_type = FIRMWARE_TYPE_RELEASE;

	साठी (माप_प्रकार i = 0; i < म_माप(tag); i++) {
		निवड (tag[i]) {
		स्थिती '-':
			dash_मोजणी++;
			थांबा;

		स्थिती '.':
			point_मोजणी++;
			थांबा;

		स्थिती 'r':
			जर (i < म_माप(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			थांबा;

		स्थिती 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			थांबा;

		स्थिती 't':
			जर (i < म_माप(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} अन्यथा {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			थांबा;

		स्थिती 'v':
			जर (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			थांबा;

		पूर्वनिर्धारित:
			थांबा;
		}
	}

	जर ((dash_मोजणी == 1 && point_मोजणी == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_मोजणी == 2 && point_मोजणी == 2) ||
	    (dash_मोजणी == 3 && point_मोजणी == 4) ||
	    (dash_मोजणी == 4 && point_मोजणी == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	साठी (माप_प्रकार i = 0; i < म_माप(tag); i++) {
		जर (buffer_counter > 2) {
			सुरूठेवा;
		}

		जर (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} अन्यथा {
			जर (buffer >= 0) {
				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	जर (buffer >= 0) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	जर (buffer_counter <= 0) {
		firmware_type = 0x00;
	}

	जर (buffer_counter == 3 || buffer_counter == 6) {
		version_संख्या = ((uint8_t)version[0] << 8 * 3) |
				 ((uint8_t)version[1] << 8 * 2) |
				 ((uint8_t)version[2] << 8 * 1) | firmware_type;

	} अन्यथा {
		version_संख्या = 0;
	}

	परत version_संख्या;
}

uint32_t px4_firmware_version(शून्य)
{
	परत version_tag_to_संख्या(PX4_GIT_TAG_STR);
}

uint32_t version_tag_to_vendor_version_संख्या(स्थिरांक अक्षर *tag)
{
	uint32_t version_संख्या = 0;

	int16_t buffer = -1;
	माप_प्रकार buffer_counter = 0;
	अक्षर version[6] = {0, 0, 0, 0, 0, 0};
	माप_प्रकार dash_मोजणी = 0;
	माप_प्रकार point_मोजणी = 0;
	पूर्णांक firmware_type = FIRMWARE_TYPE_RELEASE;

	साठी (माप_प्रकार i = 0; i < म_माप(tag); i++) {
		निवड (tag[i]) {
		स्थिती '-':
			dash_मोजणी++;
			थांबा;

		स्थिती '.':
			point_मोजणी++;
			थांबा;

		स्थिती 'r':
			जर (i < म_माप(tag) - 1 && tag[i + 1] == 'c') {
				firmware_type = FIRMWARE_TYPE_RC;

			}

			थांबा;

		स्थिती 'p':
			firmware_type = FIRMWARE_TYPE_ALPHA;
			थांबा;

		स्थिती 't':
			जर (i < म_माप(tag) - 1 && tag[i + 1] == 'y') {
				firmware_type = FIRMWARE_TYPE_DEV;

			} अन्यथा {
				firmware_type = FIRMWARE_TYPE_BETA;

			}

			थांबा;

		स्थिती 'v':
			जर (i > 0) {
				firmware_type = FIRMWARE_TYPE_DEV;

			}

			थांबा;

		पूर्वनिर्धारित:
			थांबा;
		}
	}

	जर ((dash_मोजणी == 1 && point_मोजणी == 2 && firmware_type == FIRMWARE_TYPE_RELEASE) ||
	    (dash_मोजणी == 2 && point_मोजणी == 2) ||
	    (dash_मोजणी == 3 && point_मोजणी == 4) ||
	    (dash_मोजणी == 4 && point_मोजणी == 4)) {
		firmware_type = FIRMWARE_TYPE_DEV;
	}

	साठी (माप_प्रकार i = 0; i < म_माप(tag); i++) {
		जर (buffer_counter > 5) {
			सुरूठेवा;
		}

		जर (tag[i] >= '0' && tag[i] <= '9') {
			buffer = (buffer == -1) ? 0 : buffer;
			buffer = buffer * 10 + (tag[i] - '0');

		} अन्यथा {
			जर (buffer >= 0) {
				जर (buffer_counter + 1 == 4 && tag[i] == '-') {
					थांबा;
				}

				version[buffer_counter] = buffer;
				buffer_counter++;
			}

			buffer = -1;
		}
	}

	जर (buffer >= 0 && (buffer_counter + 1 == 3 || buffer_counter + 1 == 6)) {
		version[buffer_counter] = buffer;
		buffer_counter++;
	}

	जर (buffer_counter == 6) {
		version_संख्या = ((uint8_t)version[3] << 8 * 3) |
				 ((uint8_t)version[4] << 8 * 2) |
				 ((uint8_t)version[5] << 8 * 1) | firmware_type;

	} अन्यथा जर (buffer_counter == 3) {
		version_संख्या = firmware_type;

	} अन्यथा {
		version_संख्या = 0;
	}

	परत version_संख्या;
}

uint32_t px4_firmware_vendor_version(शून्य)
{
	परत version_tag_to_vendor_version_संख्या(PX4_GIT_TAG_STR);
}

स्थिरांक अक्षर *px4_firmware_git_branch(शून्य)
{
	परत PX4_GIT_BRANCH_NAME;
}

uint32_t px4_board_version(शून्य)
{
#जर defined(__PX4_NUTTX)
	परत CONFIG_CDCACM_PRODUCTID;
#अन्यथा
	परत 1;
#endif
}

uint32_t px4_os_version(शून्य)
{
#जर defined(__PX4_DARWIN) || defined(__PX4_CYGWIN) || defined(__PX4_QURT)
	परत 0; //TODO: implement version for Darwin, Cygwin, QuRT
#किंवाजर defined(__PX4_LINUX)
	संरचना utsname name;

	जर (uname(&name) == 0) {
		अक्षर *c = name.release;

		// cut the part after the first '-'
		पर्यंत (*c && *c != '-') {
			++c;
		}

		*c = 0;
		परत version_tag_to_संख्या(name.release);

	} अन्यथा {
		परत 0;
	}

#किंवाजर defined(__PX4_NUTTX)
	परत version_tag_to_संख्या(NUTTX_GIT_TAG_STR);
#अन्यथा
# error "px4_os_version not implemented for current OS"
#endif
}

स्थिरांक अक्षर *px4_os_version_string(शून्य)
{
#जर defined(__PX4_NUTTX)
	परत NUTTX_GIT_VERSION_STR;
#अन्यथा
	परत रिक्त;
#endif
}

स्थिरांक अक्षर *px4_os_name(शून्य)
{
#जर defined(__PX4_DARWIN)
	परत "Darwin";
#किंवाजर defined(__PX4_LINUX)
	परत "Linux";
#किंवाजर defined(__PX4_QURT)
	परत "QuRT";
#किंवाजर defined(__PX4_NUTTX)
	परत "NuttX";
#किंवाजर defined(__PX4_CYGWIN)
	परत "Cygwin";
#अन्यथा
# error "px4_os_name not implemented for current OS"
#endif
}

स्थिरांक अक्षर *px4_toolchain_name(शून्य)
{
#जर defined(__clang__)
	परत "Clang/LLVM";
#किंवाजर defined(__ICC) || defined(__INTEL_COMPILER)
	परत "Intel ICC";
#किंवाजर defined(__GNUC__) || defined(__GNUG__)
	परत "GNU GCC";
#किंवाजर defined(_MSC_VER)
	परत "MS Visual Studio";
#अन्यथा
	परत "Unknown";
#endif
}

स्थिरांक अक्षर *px4_toolchain_version(शून्य)
{
#ifdef __VERSION__
	परत __VERSION__;
#अन्यथा
	परत "";
#endif
}

स्थिरांक अक्षर *px4_firmware_version_string(शून्य)
{
	परत PX4_GIT_VERSION_STR;
}

uint64_t px4_firmware_version_द्विमान(शून्य)
{
	परत PX4_GIT_VERSION_द्विमान;
}


#ifdef MAVLINK_LIB_GIT_VERSION_द्विमान
uint64_t px4_mavlink_lib_version_द्विमान(शून्य)
{
	परत MAVLINK_LIB_GIT_VERSION_द्विमान;
}
#endif /* MAVLINK_LIB_GIT_VERSION_BINARY */

uint64_t px4_os_version_द्विमान(शून्य)
{
#ifdef NUTTX_GIT_VERSION_द्विमान
	परत NUTTX_GIT_VERSION_द्विमान;
#अन्यथा
	परत 0;
#endif
}

स्थिरांक अक्षर *px4_firmware_oem_version_string(शून्य)
{
	परत PX4_GIT_OEM_VERSION_STR;
}
