/****************************************************************************
 *
 *   Copyright (c) 2020-2022 PX4 Development Team. All rights reserved.
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

#અન્તર્ભાવ "param_translation.h"


#અન્તર્ભાવ <inttypes.h>
#અન્તર્ભાવ <px4_platform_common/log.h>
#અન્તર્ભાવ <lib/drivers/device/Device.hpp>
#અન્તર્ભાવ <drivers/drv_sensor.h>
#અન્તર્ભાવ <lib/parameters/param.h>
#અન્તર્ભાવ <lib/mathlib/mathlib.h>
#અન્તર્ભાવ <uORB/topics/vtx.h>

param_modify_on_import_ret param_modify_on_import(bson_ગાંઠ_t ગાંઠ_2)
{
	// 2023-12-06: translate and invert FW_ARSP_MODE-> FW_USE_AIRSPD
	{
		જો (strcmp("FW_ARSP_MODE", ગાંઠ_2->name) == 0) {
			જો (ગાંઠ_2->i32 == 0) {
				ગાંઠ_2->i32 = 1;

			} નહીંતર {
				ગાંઠ_2->i32 = 0;
			}

			strcpy(ગાંઠ_2->name, "FW_USE_AIRSPD");
			PX4_INFO("copying and inverting %s -> %s", "FW_ARSP_MODE", "FW_USE_AIRSPD");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2023-12-06: translate CBRK_AIRSPD_CHK-> SYS_HAS_NUM_ASPD
	{
		જો (strcmp("CBRK_AIRSPD_CHK", ગાંઠ_2->name) == 0) {
			જો (ગાંઠ_2->i32 == 162128) {
				ગાંઠ_2->i32 = 0;

				strcpy(ગાંઠ_2->name, "SYS_HAS_NUM_ASPD");
				PX4_INFO("copying %s -> %s", "CBRK_AIRSPD_CHK", "SYS_HAS_NUM_ASPD");

			}

			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2024-04-15 SYS_MC_EST_GROUP removed
	જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strcmp("SYS_MC_EST_GROUP", ગાંઠ_2->name) == 0)) {

		int32_t value = ગાંઠ_2->i32;

		// value 1 local_position_estimator, attitude_estimator_q (unsupported)
		જો (value == 1) {
			// enable local_position_estimator
			int32_t lpe_en_val = 1;
			પૂર્ણાંક lpe_en_ret = param_set(param_find("LPE_EN"), &lpe_en_val);

			// enable attitude_estimator_q
			int32_t att_en_val = 1;
			પૂર્ણાંક att_en_ret = param_set(param_find("ATT_EN"), &att_en_val);

			// disable ekf2 (only if enabling lpe and att_w was successful)
			જો (lpe_en_ret == PX4_OK && att_en_ret == PX4_OK) {
				int32_t ekf2_en_val = 0;
				param_set(param_find("EKF2_EN"), &ekf2_en_val);

			} નહીંતર {
				int32_t ekf2_en_val = 1;
				param_set(param_find("EKF2_EN"), &ekf2_en_val);
			}

			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}

		// value 2 ekf2 (recommended)
		જો (value == 2) {
			// disable local_position_estimator
			int32_t lpe_en_val = 0;
			param_set(param_find("LPE_EN"), &lpe_en_val);

			// disable attitude_estimator_q
			int32_t att_en_val = 0;
			param_set(param_find("ATT_EN"), &att_en_val);

			// enable ekf2
			int32_t ekf2_en_val = 1;
			param_set(param_find("EKF2_EN"), &ekf2_en_val);

			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}

		// value 3 Q attitude estimator (no position)
		જો (value == 3) {
			// disable local_position_estimator
			int32_t lpe_en_val = 0;
			param_set(param_find("LPE_EN"), &lpe_en_val);

			// enable attitude_estimator_q
			int32_t att_en_val = 1;
			પૂર્ણાંક att_en_ret = param_set(param_find("ATT_EN"), &att_en_val);

			// disable ekf2 (only if enabling att_w was successful)
			જો (att_en_ret == PX4_OK) {
				int32_t ekf2_en_val = 0;
				param_set(param_find("EKF2_EN"), &ekf2_en_val);

			} નહીંતર {
				int32_t ekf2_en_val = 1;
				param_set(param_find("EKF2_EN"), &ekf2_en_val);
			}

			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2025-03-19: translate ASPD_FALLBACK_GW to ASPD_FALLBACK
	{
		જો (strcmp("ASPD_FALLBACK_GW", ગાંઠ_2->name) == 0) {
			strcpy(ગાંઠ_2->name, "ASPD_FALLBACK");
			PX4_INFO("copying %s -> %s", "ASPD_FALLBACK_GW", "ASPD_FALLBACK");
		}
	}

	// 2025-08-22: translate SDLOG_MODE (disabled) to SDLOG_BACKEND (no logging backend)
	{
		જો (strcmp("SDLOG_MODE", ગાંઠ_2->name) == 0) {
			જો (ગાંઠ_2->i32 == -1) {
				ગાંઠ_2->i32 = 0;

				int32_t sdlog_backend_val = 0;
				param_set(param_find("SDLOG_BACKEND"), &sdlog_backend_val);
				PX4_INFO("migrating %s -> %s", "SDLOG_MODE", "SDLOG_BACKEND");
			}
		}
	}

	// 2025-11-17: translate MNT_RANGE_PITCH to MNT_MAX_PITCH, MNT_MIN_PITCH
	{
		જો (strcmp("MNT_RANGE_PITCH", ગાંઠ_2->name) == 0) {
			જો (ગાંઠ_2->d > DBL_EPSILON) {
				દશાંશ mnt_max_pitch = static_cast<દશાંશ>(ગાંઠ_2->d) * 0.5f;
				દશાંશ mnt_min_pitch = static_cast<દશાંશ>(-ગાંઠ_2->d) * 0.5f;
				param_set(param_find("MNT_MAX_PITCH"), &mnt_max_pitch);
				param_set(param_find("MNT_MIN_PITCH"), &mnt_min_pitch);
				PX4_INFO("migrating %s -> %s, %s", "MNT_RANGE_PITCH", "MNT_MAX_PITCH", "MNT_MIN_PITCH");
			}

		}
	}

	// 2026-03-11: translate EKF2_GPS_POS_X/Y/Z -> SENS_GNSS0_OFFX/OFFY/OFFZ
	{
		જો (strcmp("EKF2_GPS_POS_X", ગાંઠ_2->name) == 0) {
			strcpy(ગાંઠ_2->name, "SENS_GNSS0_OFFX");
			PX4_INFO("migrating %s -> %s", "EKF2_GPS_POS_X", "SENS_GNSS0_OFFX");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}

		જો (strcmp("EKF2_GPS_POS_Y", ગાંઠ_2->name) == 0) {
			strcpy(ગાંઠ_2->name, "SENS_GNSS0_OFFY");
			PX4_INFO("migrating %s -> %s", "EKF2_GPS_POS_Y", "SENS_GNSS0_OFFY");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}

		જો (strcmp("EKF2_GPS_POS_Z", ગાંઠ_2->name) == 0) {
			strcpy(ગાંઠ_2->name, "SENS_GNSS0_OFFZ");
			PX4_INFO("migrating %s -> %s", "EKF2_GPS_POS_Z", "SENS_GNSS0_OFFZ");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2026-03-11: translate EKF2_GPS_DELAY to SENS_GNSS0_DELAY and SENS_GNSS1_DELAY
	{
		જો (strcmp("EKF2_GPS_DELAY", ગાંઠ_2->name) == 0) {
			int32_t delay_ms = static_cast<int32_t>(ગાંઠ_2->d);
			param_set(param_find("SENS_GNSS0_DELAY"), &delay_ms);
			param_set(param_find("SENS_GNSS1_DELAY"), &delay_ms);
			PX4_INFO("migrating %s -> %s, %s", "EKF2_GPS_DELAY", "SENS_GNSS0_DELAY", "SENS_GNSS1_DELAY");
		}
	}

	// 2026-03-11: translate MOT_POLE_COUNT to per-motor DSHOT_MOT_POL1-12
	{
		જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strcmp("MOT_POLE_COUNT", ગાંઠ_2->name) == 0)) {
			માટે (પૂર્ણાંક i = 1; i <= 12; i++) {
				અક્ષર name[20];
				snprintf(name, કદ(name), "DSHOT_MOT_POL%d", i);
				param_set(param_find(name), &ગાંઠ_2->i32);
			}

			PX4_INFO("migrating %s -> DSHOT_MOT_POL1-12 (value=%" PRId32 ")", "MOT_POLE_COUNT", ગાંઠ_2->i32);
			પાછા param_modify_on_import_ret::PARAM_SKIP_IMPORT;
		}
	}

	// 2026-03-19: translate EKF2_ENGINE_WRM to EKF2_POS_LOCK
	{
		જો (strcmp("EKF2_ENGINE_WRM", ગાંઠ_2->name) == 0) {
			strcpy(ગાંઠ_2->name, "EKF2_POS_LOCK");
			PX4_INFO("migrating %s -> %s", "EKF2_ENGINE_WRM", "EKF2_POS_LOCK");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2026-03-19: translate RC*_REV from float to int32
	{
		જો ((ગાંઠ_2->type == bson_type_t::BSON_DOUBLE) && (strncmp("RC", ગાંઠ_2->name, 2) == 0)
		    && strstr(ગાંઠ_2->name, "_REV") != nullptr) {
			ગાંઠ_2->i32 = (ગાંઠ_2->d < 0.0) ? -1 : 1;
			ગાંઠ_2->type = bson_type_t::BSON_INT32;
			PX4_INFO("migrating %s from float to int32", ગાંઠ_2->name);
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2026-06-12: merge COM_RC_OVERRIDE + COM_RC_STICK_OV into MAN_OVERRIDE_SPD
	{
		જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strcmp("COM_RC_OVERRIDE", ગાંઠ_2->name) == 0) && (ગાંઠ_2->i32 == 0)) {
			ગાંઠ_2->d = -1.0;
			ગાંઠ_2->type = bson_type_t::BSON_DOUBLE;
			strcpy(ગાંઠ_2->name, "MAN_OVERRIDE_SPD");
			PX4_INFO("migrating %s -> %s (disabled)", "COM_RC_OVERRIDE", "MAN_OVERRIDE_SPD");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2026-06-22: translate HEATER*_IMU_ID to HEATER*_SENS_ID
	//	HEATER_MAX_INSTANCES == 3 (in heater.h)
	{
		જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strncmp("HEATER", ગાંઠ_2->name, 6) == 0)
		    && strstr(ગાંઠ_2->name, "_IMU_ID") != nullptr) {
			અક્ષર old_name[BSON_MAXNAME];
			strncpy(old_name, ગાંઠ_2->name, કદ(old_name));
			અક્ષર new_name[16];
			snprintf(new_name, કદ(new_name), "HEATER%c_SENS_ID", ગાંઠ_2->name[6]);
			strcpy(ગાંઠ_2->name, new_name);
			PX4_INFO("migrating %s -> %s", old_name, new_name);
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2026-10-20: the single heater's SENS_* parameters became those of heater instance 1
	{
		સ્થિર constexpr અચલ અક્ષર *kRenames[][2] {
			{"SENS_IMU_TEMP", "HEATER1_TEMP"},
			{"SENS_IMU_TEMP_FF", "HEATER1_TEMP_FF"},
			{"SENS_IMU_TEMP_I", "HEATER1_TEMP_I"},
			{"SENS_IMU_TEMP_P", "HEATER1_TEMP_P"},
		};

		માટે (અચલ auto &rename : kRenames) {
			જો (strcmp(rename[0], ગાંઠ_2->name) == 0) {
				strcpy(ગાંઠ_2->name, rename[1]);
				PX4_INFO("migrating %s -> %s", rename[0], rename[1]);
				પાછા param_modify_on_import_ret::PARAM_MODIFIED;
			}
		}

		// SENS_TEMP_ID 0 kept the heater from starting, while HEATER1_SENS_ID 0 selects a sensor and -1 disables it
		જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strcmp("SENS_TEMP_ID", ગાંઠ_2->name) == 0)) {
			જો (ગાંઠ_2->i32 == 0) {
				ગાંઠ_2->i32 = -1;
			}

			strcpy(ગાંઠ_2->name, "HEATER1_SENS_ID");
			PX4_INFO("migrating %s -> %s", "SENS_TEMP_ID", "HEATER1_SENS_ID");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2026-06-29: MPC_LAND_RC_HELP replaced by the MPC_AUTO_NUDGING bitmask (landing nudging = bit 1).
	// The old in-Hold yaw nudge is not migrated; it now lives in bit 0 (yaw nudging in all auto modes).
	{
		જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strcmp("MPC_LAND_RC_HELP", ગાંઠ_2->name) == 0)) {
			જો (ગાંઠ_2->i32 != 0) {
				int32_t nudging = 0;
				param_get(param_find("MPC_AUTO_NUDGING"), &nudging);
				nudging |= (1 << 1);
				param_set(param_find("MPC_AUTO_NUDGING"), &nudging);
			}

			PX4_INFO("migrating MPC_LAND_RC_HELP -> MPC_AUTO_NUDGING bit 1 (value=%" PRId32 ")", ગાંઠ_2->i32);
			પાછા param_modify_on_import_ret::PARAM_SKIP_IMPORT;
		}
	}

	// 2026-07-13: translate COM_ARM_TRAFF (arming check only) to COM_TRAFF_AVOID (arming check + failsafe action)
	{
		જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strcmp("COM_ARM_TRAFF", ગાંઠ_2->name) == 0)) {
			// old: 0 Disabled, 1 Warning only (arming allowed), 2 Enforce all modes, 3 Enforce mission only
			// new: 0 Disabled, 1 Warning (arming allowed), 2 Error (arming blocked)
			// Old value 3 (mission-only enforcement) is intentionally mapped to 2 (all modes): mode-scoped
			// arming enforcement is no longer supported, so we err on the restrictive side.
			// COM_ARM_TRAFF never triggered an in-flight failsafe action, so the new failsafe
			// action defaults to Warning either way; only the arming behavior is preserved.
			જો (ગાંઠ_2->i32 == 1) {
				ગાંઠ_2->i32 = 1;

			} નહીંતર જો (ગાંઠ_2->i32 >= 2) {
				ગાંઠ_2->i32 = 2;
			}

			strcpy(ગાંઠ_2->name, "COM_TRAFF_AVOID");
			PX4_INFO("migrating %s -> %s", "COM_ARM_TRAFF", "COM_TRAFF_AVOID");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2026-08-05: the protocol selection moved out of VTX_DEVICE into VTX_PROTOCOL. VTX_DEVICE keeps
	// the old layout that holds the device in its high byte, so only the protocol has to be derived:
	// the Peak THOR speaks SmartAudio. The Rush MAX SOLO entry was removed and falls back to a generic
	// device on Tramp, the only protocol it speaks. A value that is not listed stays untouched and acts
	// as a generic device on SmartAudio, which is what both parameters default to, so the old value 0
	// needs nothing.
	{
		સ્થિર constexpr int32_t PROTOCOL_SMART_AUDIO = 0; // VTX_PROTOCOL value, not in msg/Vtx.msg

		જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strcmp("VTX_DEVICE", ગાંઠ_2->name) == 0)) {
			int32_t device = -1;
			int32_t protocol = PROTOCOL_SMART_AUDIO;

			પસંદગી (ગાંઠ_2->i32) {
			સ્થિતિ 100: // generic device, Tramp
				device = vtx_s::DEVICE_UNKNOWN << 8;
				protocol = vtx_s::PROTOCOL_TRAMP;
				તોડો;

			સ્થિતિ 5120: // Peak THOR (T35, T67, T78, T89), SmartAudio only
				device = vtx_s::DEVICE_PEAK_THOR << 8;
				protocol = PROTOCOL_SMART_AUDIO;
				તોડો;

			સ્થિતિ 10240: // Rush MAX SOLO -> generic device, Tramp
				device = vtx_s::DEVICE_UNKNOWN << 8;
				protocol = vtx_s::PROTOCOL_TRAMP;
				તોડો;
			}

			જો (device >= 0) {
				ગાંઠ_2->i32 = device;
				param_set(param_find("VTX_PROTOCOL"), &protocol);
				PX4_INFO("migrating VTX_DEVICE -> VTX_DEVICE %" PRId32 " + VTX_PROTOCOL %" PRId32,
					 device, protocol);
				પાછા param_modify_on_import_ret::PARAM_MODIFIED;
			}
		}
	}

	// 2026-08-18: UAVCAN_ECU_MAXF replaced by per-tank (idx 1 based)
	{
		જો ((ગાંઠ_2->type == bson_type_t::BSON_DOUBLE) && (strcmp("UAVCAN_ECU_MAXF", ગાંઠ_2->name) == 0)) {
			strcpy(ગાંઠ_2->name, "UAVCAN_ECU_MAXF1");
			PX4_INFO("copying %s -> %s", "UAVCAN_ECU_MAXF", "UAVCAN_ECU_MAXF1");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	// 2026-09-28: translate SENS_GPS* to SENS_GNSS*
	{
		સ્થિર constexpr અચલ અક્ષર *kRenames[][2] {
			{"SENS_GPS_PRIME", "SENS_GNSS_PRIME"},
			{"SENS_GPS0_ID", "SENS_GNSS0_ID"},
			{"SENS_GPS0_OFFX", "SENS_GNSS0_OFFX"},
			{"SENS_GPS0_OFFY", "SENS_GNSS0_OFFY"},
			{"SENS_GPS0_OFFZ", "SENS_GNSS0_OFFZ"},
			{"SENS_GPS0_DELAY", "SENS_GNSS0_DELAY"},
			{"SENS_GPS1_ID", "SENS_GNSS1_ID"},
			{"SENS_GPS1_OFFX", "SENS_GNSS1_OFFX"},
			{"SENS_GPS1_OFFY", "SENS_GNSS1_OFFY"},
			{"SENS_GPS1_OFFZ", "SENS_GNSS1_OFFZ"},
			{"SENS_GPS1_DELAY", "SENS_GNSS1_DELAY"},
		};

		માટે (અચલ auto &rename : kRenames) {
			જો (strcmp(rename[0], ગાંઠ_2->name) == 0) {
				strcpy(ગાંઠ_2->name, rename[1]);
				PX4_INFO("migrating %s -> %s", rename[0], rename[1]);
				પાછા param_modify_on_import_ret::PARAM_MODIFIED;
			}
		}
	}

	// 2026-09-29: the GNSS checks move from EKF2 to the sensors module
	{
		સ્થિર constexpr અચલ અક્ષર *kRenames[][2] {
			{"EKF2_GPS_CHECK", "GNSS_CHECK"},
			{"EKF2_REQ_EPH", "GNSS_REQ_EPH"},
			{"EKF2_REQ_EPV", "GNSS_REQ_EPV"},
			{"EKF2_REQ_NSATS", "GNSS_REQ_NSATS"},
			{"EKF2_REQ_PDOP", "GNSS_REQ_PDOP"},
			{"EKF2_REQ_HDRIFT", "GNSS_REQ_HDRIFT"},
			{"EKF2_REQ_VDRIFT", "GNSS_REQ_VDRIFT"},
			{"EKF2_REQ_FIX", "GNSS_REQ_FIX"},
			{"EKF2_REQ_SACC", "GNSS_REQ_SACC"},
			{"EKF2_REQ_GPS_H", "GNSS_REQ_TIME"},
		};

		માટે (અચલ auto &rename : kRenames) {
			જો (strcmp(rename[0], ગાંઠ_2->name) == 0) {
				strcpy(ગાંઠ_2->name, rename[1]);
				PX4_INFO("migrating %s -> %s", rename[0], rename[1]);
				પાછા param_modify_on_import_ret::PARAM_MODIFIED;
			}
		}
	}

	// 2026-10-06: translate CBRK_IO_SAFETY -> COM_SAFETY_MODE. Any value but the breaker key had safety enabled, with
	// both the switch and the MAVLink command able to turn it off.
	{
		જો ((ગાંઠ_2->type == bson_type_t::BSON_INT32) && (strcmp("CBRK_IO_SAFETY", ગાંઠ_2->name) == 0)) {
			ગાંઠ_2->i32 = (ગાંઠ_2->i32 == 22027) ? 0 : 1;
			strcpy(ગાંઠ_2->name, "COM_SAFETY_MODE");
			PX4_INFO("migrating %s -> %s", "CBRK_IO_SAFETY", "COM_SAFETY_MODE");
			પાછા param_modify_on_import_ret::PARAM_MODIFIED;
		}
	}

	પાછા param_modify_on_import_ret::PARAM_NOT_MODIFIED;
}
