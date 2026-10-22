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

#અન્તર્ભાવ <px4_platform_common/px4_config.h>
#અન્તર્ભાવ <systemlib/px4_macros.h>
#અન્તર્ભાવ <stdint.h>

__BEGIN_DECLS

/**
 * get the board name as string (including the version if there are multiple)
 */
સ્થિર અંતઃરેખ અચલ અક્ષર *px4_board_name(શૂન્ય)
{
	પાછા PX4_BOARD_NAME;
}

/**
 * get the board build target variant
 */
સ્થિર અંતઃરેખ અચલ અક્ષર *px4_board_target_label(શૂન્ય)
{
	પાછા PX4_BOARD_LABEL;
}

/**
 * get the board sub type
 */
સ્થિર અંતઃરેખ અચલ અક્ષર *px4_board_sub_type(શૂન્ય)
{
	પાછા board_get_hw_type_name();
}

/**
 * get the board HW version
 */
સ્થિર અંતઃરેખ પૂર્ણાંક px4_board_hw_version(શૂન્ય)
{
	પાછા board_get_hw_version();
}

/**
 * get the board HW revision
 */
સ્થિર અંતઃરેખ પૂર્ણાંક px4_board_hw_revision(શૂન્ય)
{
	પાછા board_get_hw_revision();
}

#જો defined(BOARD_HAS_HW_SPLIT_VERSIONING)
/**
 * get the base board type
 */
સ્થિર અંતઃરેખ અચલ અક્ષર *px4_board_base_type(શૂન્ય)
{
	પાછા board_get_hw_base_type_name();
}
#endif

/**
 * get the build URI (used for crash logging)
 */
અચલ અક્ષર *px4_build_uri(શૂન્ય);

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
__EXPORT uint32_t version_tag_to_સંખ્યા(અચલ અક્ષર *tag);

/**
 * get the PX4 Firmware version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_firmware_version(શૂન્ય);

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
__EXPORT uint32_t version_tag_to_vendor_version_સંખ્યા(અચલ અક્ષર *tag);

/**
 * get the PX4 Firmware vendor version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_firmware_vendor_version(શૂન્ય);

/**
 * get the board version (last 8 bytes should be silicon ID, if any)
 */
__EXPORT uint32_t px4_board_version(શૂન્ય);

/**
 * operating system version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_os_version(શૂન્ય);

/**
 * Operating system version as human readable string (git tag)
 * @return string or NULL if not defined
 */
__EXPORT અચલ અક્ષર *px4_os_version_string(શૂન્ય);

/**
 * name of the operating system
 * @return human readable string
 */
__EXPORT અચલ અક્ષર *px4_os_name(શૂન્ય);

/**
 * Toolchain name used to compile PX4
 */
__EXPORT અચલ અક્ષર *px4_toolchain_name(શૂન્ય);

/**
 * Toolchain version used to compile PX4 (no particular format)
 */
__EXPORT અચલ અક્ષર *px4_toolchain_version(શૂન્ય);

/**
 * Firmware version as human readable string (git tag)
 */
__EXPORT અચલ અક્ષર *px4_firmware_version_string(શૂન્ય);

/**
 * get the git branch name (can be empty, for example if HEAD points to a tag)
 */
__EXPORT અચલ અક્ષર *px4_firmware_git_branch(શૂન્ય);

/**
 * Firmware version in binary form (first part of the git tag)
 */
__EXPORT uint64_t px4_firmware_version_દ્વિઅંકી(શૂન્ય);

/**
 * MAVLink lib version in binary form (first part of the git tag)
 */
__EXPORT uint64_t px4_mavlink_lib_version_દ્વિઅંકી(શૂન્ય);

/**
 * Operating system version in binary form (first part of the git tag)
 * @return this is not available on all OSes and can return 0
 */
__EXPORT uint64_t px4_os_version_દ્વિઅંકી(શૂન્ય);

/**
 * get the git oem version tag (can be empty, no particular format)
 */
__EXPORT અચલ અક્ષર *px4_firmware_oem_version_string(શૂન્ય);

__END_DECLS
