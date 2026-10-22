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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <systemlib/px4_macros.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

__BEGIN_DECLS

/**
 * get the board name as string (including the version if there are multiple)
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *px4_board_name(ਖਾਲੀ)
{
	ਵਾਪਸ PX4_BOARD_NAME;
}

/**
 * get the board build target variant
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *px4_board_ਨਿਸ਼ਾਨਾ_label(ਖਾਲੀ)
{
	ਵਾਪਸ PX4_BOARD_LABEL;
}

/**
 * get the board sub type
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *px4_board_sub_type(ਖਾਲੀ)
{
	ਵਾਪਸ board_get_hw_type_name();
}

/**
 * get the board HW version
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ px4_board_hw_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
	ਵਾਪਸ board_get_hw_ਸੰਸਕਰਣ();
}

/**
 * get the board HW revision
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ px4_board_hw_revision(ਖਾਲੀ)
{
	ਵਾਪਸ board_get_hw_revision();
}

#ਜੇ defined(BOARD_HAS_HW_ਵੰਡੋ_VERSIONING)
/**
 * get the base board type
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *px4_board_ਅਧਾਰ_type(ਖਾਲੀ)
{
	ਵਾਪਸ board_get_hw_ਅਧਾਰ_type_name();
}
#ਜੇ_ਅੰਤ

/**
 * get the build URI (used for crash logging)
 */
ਅਚਲ ਅੱਖਰ *px4_build_uri(ਖਾਲੀ);

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
__EXPORT uint32_t ਸੰਸਕਰਣ_ਟੈਗ_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(ਅਚਲ ਅੱਖਰ *ਟੈਗ);

/**
 * get the PX4 Firmware version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_firmware_ਸੰਸਕਰਣ(ਖਾਲੀ);

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
__EXPORT uint32_t ਸੰਸਕਰਣ_ਟੈਗ_ਮੰਜ਼ਿਲ_vendor_ਸੰਸਕਰਣ_ਸੰਖਿਆ(ਅਚਲ ਅੱਖਰ *ਟੈਗ);

/**
 * get the PX4 Firmware vendor version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_firmware_vendor_ਸੰਸਕਰਣ(ਖਾਲੀ);

/**
 * get the board version (last 8 bytes should be silicon ID, if any)
 */
__EXPORT uint32_t px4_board_ਸੰਸਕਰਣ(ਖਾਲੀ);

/**
 * operating system version
 * @return version in the form 0xAABBCCTT (AA: Major, BB: Minor, CC: Patch, TT Type @see FIRMWARE_TYPE)
 */
__EXPORT uint32_t px4_os_ਸੰਸਕਰਣ(ਖਾਲੀ);

/**
 * Operating system version as human readable string (git tag)
 * @return string or NULL if not defined
 */
__EXPORT ਅਚਲ ਅੱਖਰ *px4_os_ਸੰਸਕਰਣ_string(ਖਾਲੀ);

/**
 * name of the operating system
 * @return human readable string
 */
__EXPORT ਅਚਲ ਅੱਖਰ *px4_os_name(ਖਾਲੀ);

/**
 * Toolchain name used to compile PX4
 */
__EXPORT ਅਚਲ ਅੱਖਰ *px4_toolchain_name(ਖਾਲੀ);

/**
 * Toolchain version used to compile PX4 (no particular format)
 */
__EXPORT ਅਚਲ ਅੱਖਰ *px4_toolchain_ਸੰਸਕਰਣ(ਖਾਲੀ);

/**
 * Firmware version as human readable string (git tag)
 */
__EXPORT ਅਚਲ ਅੱਖਰ *px4_firmware_ਸੰਸਕਰਣ_string(ਖਾਲੀ);

/**
 * get the git branch name (can be empty, for example if HEAD points to a tag)
 */
__EXPORT ਅਚਲ ਅੱਖਰ *px4_firmware_git_branch(ਖਾਲੀ);

/**
 * Firmware version in binary form (first part of the git tag)
 */
__EXPORT uint64_t px4_firmware_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ(ਖਾਲੀ);

/**
 * MAVLink lib version in binary form (first part of the git tag)
 */
__EXPORT uint64_t px4_mavlink_lib_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ(ਖਾਲੀ);

/**
 * Operating system version in binary form (first part of the git tag)
 * @return this is not available on all OSes and can return 0
 */
__EXPORT uint64_t px4_os_ਸੰਸਕਰਣ_ਦੁਆਧਾਰੀ(ਖਾਲੀ);

/**
 * get the git oem version tag (can be empty, no particular format)
 */
__EXPORT ਅਚਲ ਅੱਖਰ *px4_firmware_oem_ਸੰਸਕਰਣ_string(ਖਾਲੀ);

__END_DECLS
