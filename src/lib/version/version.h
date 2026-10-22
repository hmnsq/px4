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

#pragma once

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_config.h>
#ଅନ୍ତର୍ଭାଵ <systemlib/px4_macros.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>

__BEGIN_DECLS

/**
 * get the board name as string (including the version if there are multiple)
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର *px4_board_name(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ PX4_BOARD_NAME;
}

/**
 * get the board build target variant
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର *px4_board_target_label(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ PX4_BOARD_LABEL;
}

/**
 * get the board sub type
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର *px4_board_sub_type(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ board_get_hw_type_name();
}

/**
 * get the board HW version
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା px4_board_hw_version(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ board_get_hw_version();
}

/**
 * get the board HW revision
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା px4_board_hw_revision(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ board_get_hw_revision();
}

#ଯଦି defined(BOARD_HAS_HW_SPLIT_VERSIONING)
/**
 * get the base board type
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର *px4_board_base_type(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ board_get_hw_base_type_name();
}
#endif

/**
 * get the build URI (used for crash logging)
 */
ଅଚଳ ଅକ୍ଷର *px4_build_uri(ଶୂନ୍ୟ);

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
__EXPORT uint32_t version_tag_to_ସଂଖ୍ୟା(ଅଚଳ ଅକ୍ଷର *tag);

/**
 * get the PX4 Firmware version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_firmware_version(ଶୂନ୍ୟ);

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
__EXPORT uint32_t version_tag_to_vendor_version_ସଂଖ୍ୟା(ଅଚଳ ଅକ୍ଷର *tag);

/**
 * get the PX4 Firmware vendor version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_firmware_vendor_version(ଶୂନ୍ୟ);

/**
 * get the board version (last 8 bytes should be silicon ID, if any)
 */
__EXPORT uint32_t px4_board_version(ଶୂନ୍ୟ);

/**
 * operating system version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_os_version(ଶୂନ୍ୟ);

/**
 * Operating system version as human readable string (git tag)
 * @return string or NULL if not defined
 */
__EXPORT ଅଚଳ ଅକ୍ଷର *px4_os_version_string(ଶୂନ୍ୟ);

/**
 * name of the operating system
 * @return human readable string
 */
__EXPORT ଅଚଳ ଅକ୍ଷର *px4_os_name(ଶୂନ୍ୟ);

/**
 * Toolchain name used to compile PX4
 */
__EXPORT ଅଚଳ ଅକ୍ଷର *px4_toolchain_name(ଶୂନ୍ୟ);

/**
 * Toolchain version used to compile PX4 (no particular format)
 */
__EXPORT ଅଚଳ ଅକ୍ଷର *px4_toolchain_version(ଶୂନ୍ୟ);

/**
 * Firmware version as human readable string (git tag)
 */
__EXPORT ଅଚଳ ଅକ୍ଷର *px4_firmware_version_string(ଶୂନ୍ୟ);

/**
 * get the git branch name (can be empty, for example if HEAD points to a tag)
 */
__EXPORT ଅଚଳ ଅକ୍ଷର *px4_firmware_git_branch(ଶୂନ୍ୟ);

/**
 * Firmware version in binary form (first part of the git tag)
 */
__EXPORT uint64_t px4_firmware_version_ଦ୍ୱିମିକ(ଶୂନ୍ୟ);

/**
 * MAVLink lib version in binary form (first part of the git tag)
 */
__EXPORT uint64_t px4_mavlink_lib_version_ଦ୍ୱିମିକ(ଶୂନ୍ୟ);

/**
 * Operating system version in binary form (first part of the git tag)
 * @return this is not available on all OSes and can return 0
 */
__EXPORT uint64_t px4_os_version_ଦ୍ୱିମିକ(ଶୂନ୍ୟ);

/**
 * get the git oem version tag (can be empty, no particular format)
 */
__EXPORT ଅଚଳ ଅକ୍ଷର *px4_firmware_oem_version_string(ଶୂନ୍ୟ);

__END_DECLS
