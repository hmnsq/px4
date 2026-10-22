/****************************************************************************
 *
 *   Copyright (c) 2013-2019 PX4 Development Team. All rights reserved.
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

/**
 * @file version.h
 *
 * Tools for system version detection.
 *
 * @author Anton Babushkin <anton.babushkin@me.com>
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#निर्देशः once

#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <systemlib/px4_macros.h>
#अन्तर्भाव <stdint.h>

__BEGIN_DECLS

/**
 * get the board name as string (including the version if there are multiple)
 */
स्थिर अन्तःस्थ ध्रुव वर्ण *px4_board_name(शून्य)
{
	प्रत्यागम PX4_BOARD_NAME;
}

/**
 * get the board build target variant
 */
स्थिर अन्तःस्थ ध्रुव वर्ण *px4_board_target_label(शून्य)
{
	प्रत्यागम PX4_BOARD_LABEL;
}

/**
 * get the board sub type
 */
स्थिर अन्तःस्थ ध्रुव वर्ण *px4_board_sub_type(शून्य)
{
	प्रत्यागम board_get_hw_type_name();
}

/**
 * get the board HW version
 */
स्थिर अन्तःस्थ पूर्णाङ्क px4_board_hw_संस्करणम्(शून्य)
{
	प्रत्यागम board_get_hw_संस्करणम्();
}

/**
 * get the board HW revision
 */
स्थिर अन्तःस्थ पूर्णाङ्क px4_board_hw_revision(शून्य)
{
	प्रत्यागम board_get_hw_revision();
}

#यदि defined(BOARD_HAS_HW_विभज_VERSIONING)
/**
 * get the base board type
 */
स्थिर अन्तःस्थ ध्रुव वर्ण *px4_board_आधारः_type(शून्य)
{
	प्रत्यागम board_get_hw_आधारः_type_name();
}
#यदि_अन्तः

/**
 * get the build URI (used for crash logging)
 */
ध्रुव वर्ण *px4_build_uri(शून्य);

/**
 * Convert a version tag string to a number
 * @param tag version tag in one of the following forms:
 *            - vendor: v1.4.0-0.2.0
 *            - dev: v1.4.0-rc3-7-g7e282f57
 *            - rc: v1.4.0-rc4
 *            - beta: v1.4.0-beta1
 *            - release: v1.4.0
 *            - linux: 7.9.3
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t संस्करणम्_चिह्नकम्_गन्तव्यम्_सङ्ख्या(ध्रुव वर्ण *चिह्नकम्);

/**
 * get the PX4 Firmware version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_firmware_संस्करणम्(शून्य);

/**
 * Convert a version tag string to a vendor version number
 * @param tag version tag in one of the following forms:
 *            - vendor: v1.4.0-0.2.0
 *            - dev: v1.4.0-rc3-7-g7e282f57
 *            - rc: v1.4.0-rc4
 *            - beta: v1.4.0-beta1
 *            - release: v1.4.0
 *            - linux: 7.9.3
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t संस्करणम्_चिह्नकम्_गन्तव्यम्_vendor_संस्करणम्_सङ्ख्या(ध्रुव वर्ण *चिह्नकम्);

/**
 * get the PX4 Firmware vendor version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_firmware_vendor_संस्करणम्(शून्य);

/**
 * get the board version (last 8 bytes should be silicon ID, if any)
 */
__EXPORT uint32_t px4_board_संस्करणम्(शून्य);

/**
 * operating system version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_os_संस्करणम्(शून्य);

/**
 * Operating system version as human readable string (git tag)
 * @return string or NULL if not defined
 */
__EXPORT ध्रुव वर्ण *px4_os_संस्करणम्_string(शून्य);

/**
 * name of the operating system
 * @return human readable string
 */
__EXPORT ध्रुव वर्ण *px4_os_name(शून्य);

/**
 * Toolchain name used to compile PX4
 */
__EXPORT ध्रुव वर्ण *px4_toolchain_name(शून्य);

/**
 * Toolchain version used to compile PX4 (no particular format)
 */
__EXPORT ध्रुव वर्ण *px4_toolchain_संस्करणम्(शून्य);

/**
 * Firmware version as human readable string (git tag)
 */
__EXPORT ध्रुव वर्ण *px4_firmware_संस्करणम्_string(शून्य);

/**
 * get the git branch name (can be empty, for example if HEAD points to a tag)
 */
__EXPORT ध्रुव वर्ण *px4_firmware_git_branch(शून्य);

/**
 * Firmware version in binary form (first part of the git tag)
 */
__EXPORT uint64_t px4_firmware_संस्करणम्_द्विमान(शून्य);

/**
 * MAVLink lib version in binary form (first part of the git tag)
 */
__EXPORT uint64_t px4_mavlink_lib_संस्करणम्_द्विमान(शून्य);

/**
 * Operating system version in binary form (first part of the git tag)
 * @return this is not available on all OSes and can return 0
 */
__EXPORT uint64_t px4_os_संस्करणम्_द्विमान(शून्य);

/**
 * get the git oem version tag (can be empty, no particular format)
 */
__EXPORT ध्रुव वर्ण *px4_firmware_oem_संस्करणम्_string(शून्य);

__END_DECLS
