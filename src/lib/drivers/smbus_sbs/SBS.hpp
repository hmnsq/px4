/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
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
 * @file SBS.hpp
 *
 * Header for a core SBS 1.1 specification of Smart Battery
 * http://sbs-forum.org/specs/sbdat110.pdf
 *
 * @author Eohan George <eohan@rotoye.com>
 * @author Nick Belanger <nbelanger@mail.skymul.com>
 */

#pragma once

#অন্তর্ভাব <px4_platform_common/i2c_spi_buses.h>
#অন্তর্ভাব <lib/drivers/smbus/SMBus.hpp>
#অন্তর্ভাব <uORB/topics/battery_info.h>
#অন্তর্ভাব <uORB/topics/battery_status.h>
#অন্তর্ভাব <px4_platform_common/param.h>
#অন্তর্ভাব <lib/atmosphere/atmosphere.h>

using নামাকাশ time_literals;

#define SBS_MEASUREMENT_INTERVAL_US             	100_ms          ///< time in microseconds, measure at 10Hz (not part of SBS spec)

template<শ্রেণী T>
শ্রেণী SMBUS_SBS_BaseClass : সর্বজনীন I2CSPIDriver<T>
{
সর্বজনীন:
	SMBUS_SBS_BaseClass(ধ্রুবক I2CSPIDriverConfig &config, SMBus *interface);
	SMBUS_SBS_BaseClass();

	~SMBUS_SBS_BaseClass();

	স্থির শূন্য print_usage();

	friend SMBus;

	পূর্ণসংখ্যা populate_smbus_তথ্য(battery_status_s &বার্তা, battery_info_s &battery_info);

	আভাসী শূন্য RunImpl(); // Can be overridden by derived implimentation

	আভাসী শূন্য custom_method(ধ্রুবক BusCLIArguments &cli) = 0; //Has be overriden by derived implimentation

	/**
	 * @brief Returns the SBS serial number of the battery device.
	 * @return Returns the SBS serial number of the battery device.
	 */
	uint16_t get_serial_সংখ্যা();

	/**
	* @brief Read info from battery on startup.
	* @return Returns PX4_OK on success, PX4_ERROR on failure. Can be overridden by derived implimentation
	*/
	আভাসী পূর্ণসংখ্যা get_startup_info();

	/**
	 * @brief Gets the SBS manufacture date of the battery.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	পূর্ণসংখ্যা manufacture_date();

	/**
	 * @brief Gets the SBS manufacturer name of the battery device.
	 * @param manufacturer_name Pointer to a buffer into which the manufacturer name is to be written.
	 * @param max_length The maximum number of bytes to attempt to read from the manufacturer name register,
	 *                   including the null character that is appended to the end.
	 * @return Returns PX4_OK on success, PX4_ERROR on failure.
	 */
	পূর্ণসংখ্যা manufacturer_name(uint8_t *manufacturer_name, ধ্রুবক uint8_t দৈর্ঘ্য);

	/**
	 * @brief Enables or disables the cell under voltage protection emergency shut off.
	 */

	শূন্য suspend();

	শূন্য resume();
	গণনা {
		BATT_SMBUS_অস্থায়ী                         =	0x08,           ///< temperature register
		BATT_SMBUS_VOLTAGE                      =       0x09,           ///< voltage register
		BATT_SMBUS_CURRENT                      =       0x0A,           ///< current register
		BATT_SMBUS_AVERAGE_CURRENT              =       0x0B,           ///< average current register
		BATT_SMBUS_MAX_ERROR                    =       0x0C,           ///< max error
		BATT_SMBUS_RELATIVE_SOC                 =       0x0D,           ///< Relative State Of Charge
		BATT_SMBUS_ABSOLUTE_SOC                 =       0x0E,           ///< Absolute State of charge
		BATT_SMBUS_REMAINING_CAPACITY           =       0x0F,           ///< predicted remaining battery capacity as a percentage
		BATT_SMBUS_FULL_CHARGE_CAPACITY         =       0x10,           ///< capacity when fully charged
		BATT_SMBUS_RUN_TIME_TO_EMPTY            =       0x11,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_AVERAGE_TIME_TO_EMPTY        =       0x12,           ///< predicted remaining battery capacity based on the present rate of discharge in min
		BATT_SMBUS_CYCLE_গুনতি                  =       0x17,           ///< number of cycles the battery has experienced
		BATT_SMBUS_DESIGN_CAPACITY              =       0x18,           ///< design capacity register
		BATT_SMBUS_DESIGN_VOLTAGE               =       0x19,           ///< design voltage register
		BATT_SMBUS_MANUFACTURER_NAME            =       0x20,           ///< manufacturer name
		BATT_SMBUS_MANUFACTURER_NAME_আকার       =       21,             ///< manufacturer name data size (not part of SBS spec)
		BATT_SMBUS_DEVICE_NAME			=	0x21,		///< character string that contains the battery's name
		BATT_SMBUS_MANUFACTURE_DATE             =       0x1B,           ///< manufacture date register
		BATT_SMBUS_SERIAL_সংখ্যা                =       0x1C,           ///< serial number register
		BATT_SMBUS_MANUFACTURER_ACCESS          =       0x00,
		BATT_SMBUS_MANUFACTURER_তথ্য            =       0x23
	} SBS_REGISTERS;

সুরক্ষিত:

	SMBus *_interface;

	perf_গণক_t _cycle{perf_alloc(PC_ELAPSED, "batmon_cycle")}; // TODO

	orb_advert_t _battery_info_topic{nullptr};
	orb_advert_t _batt_topic{nullptr};

	/** @param _cell_count Number of series cell (retrieved from cell_count PX4 params) */
	uint8_t cell_গুনতি_2{0};

	/** @param _batt_capacity Battery design capacity in mAh (0 means unknown). */
	uint16_t _batt_capacity{0};

	/** @param _batt_startup_capacity Battery remaining capacity in mAh on startup. */
	uint16_t _batt_startup_capacity{0};

	/** @param _cycle_count The number of cycles the battery has experienced. */
	uint16_t cycle_গুনতি{0};

	/** @param _serial_number Serial number register. */
	uint16_t serial_সংখ্যা{0};

	/** @param _manufacturer_name Name of the battery manufacturer. */
	অক্ষর _manufacturer_name[BATT_SMBUS_MANUFACTURER_NAME_আকার + 1] {};	// Plus one for terminator

	/** @param _manufacture_date Date of the battery manufacturing. */
	uint16_t _manufacture_date{0};

	/** @param _state_of_health state of health as read on connection  */
	দশমিক _state_of_health{0.f}; // Not part of SBS, can be calculated from FullChargeCapacity
	// and DesignCapacity

	শূন্য print_man_info();

};

template<শ্রেণী T>
SMBUS_SBS_BaseClass<T>::SMBUS_SBS_BaseClass(ধ্রুবক I2CSPIDriverConfig &config, SMBus *interface):
	I2CSPIDriver<T>(config),
	_interface(interface)
{
	battery_info_s battery_info{};
	battery_status_s new_report = {};
	পূর্ণসংখ্যা SBS_instance_সংখ্যা = 0;
	_battery_info_topic = orb_advertise_multi(ORB_ID(battery_info), &battery_info, &SBS_instance_সংখ্যা);
	_batt_topic = orb_advertise_multi(ORB_ID(battery_status), &new_report, &SBS_instance_সংখ্যা);
	_interface->init();
}

template<শ্রেণী T>
SMBUS_SBS_BaseClass<T>::~SMBUS_SBS_BaseClass()
{
	orb_unadvertise(_batt_topic);
	perf_free(_cycle); // TODO

	যদি (_interface != nullptr) {
		delete _interface;
	}

}

template<শ্রেণী T>
শূন্য SMBUS_SBS_BaseClass<T>::suspend()
{
	এই->ScheduleClear();
}

template<শ্রেণী T>
শূন্য SMBUS_SBS_BaseClass<T>::resume()
{
	এই->ScheduleOnInterval(SBS_MEASUREMENT_INTERVAL_US);
}

template<শ্রেণী T>
uint16_t SMBUS_SBS_BaseClass<T>::get_serial_সংখ্যা()
{
	uint16_t serial_অঙ্ক = 0;

	যদি (_interface->read_word(BATT_SMBUS_SERIAL_সংখ্যা, serial_অঙ্ক) == PX4_OK) {
		ফেরত serial_অঙ্ক;
	}

	ফেরত PX4_ERROR;
}

template<শ্রেণী T>
পূর্ণসংখ্যা SMBUS_SBS_BaseClass<T>::get_startup_info()
{
	পূর্ণসংখ্যা ret = PX4_OK;

	ret |= _interface->block_read(BATT_SMBUS_MANUFACTURER_NAME, _manufacturer_name, BATT_SMBUS_MANUFACTURER_NAME_আকার,
				      সত্য);
	_manufacturer_name[আকার(_manufacturer_name) - 1] = '\0';

	uint16_t serial_অঙ্ক;
	ret |= _interface->read_word(BATT_SMBUS_SERIAL_সংখ্যা, serial_অঙ্ক);

	uint16_t remaining_cap;
	ret |= _interface->read_word(BATT_SMBUS_REMAINING_CAPACITY, remaining_cap);

	uint16_t cycle_গুনতি_2;
	ret |= _interface->read_word(BATT_SMBUS_CYCLE_গুনতি, cycle_গুনতি_2);

	uint16_t full_cap;
	ret |= _interface->read_word(BATT_SMBUS_FULL_CHARGE_CAPACITY, full_cap);

	uint16_t manufacture_date;
	ret |= _interface->read_word(BATT_SMBUS_MANUFACTURE_DATE, manufacture_date);

	যদি (!ret) {
		serial_সংখ্যা = serial_অঙ্ক;
		_batt_startup_capacity = (uint16_t)((দশমিক)remaining_cap);
		cycle_গুনতি = cycle_গুনতি_2;
		_batt_capacity = (uint16_t)((দশমিক)full_cap);
		_manufacture_date = manufacture_date;
	}

	ফেরত ret;
}

template<শ্রেণী T>
পূর্ণসংখ্যা SMBUS_SBS_BaseClass<T>::populate_smbus_তথ্য(battery_status_s &তথ্য, battery_info_s &battery_info)
{

	// Temporary variable for storing SMBUS reads.
	uint16_t ফলাফল;

	পূর্ণসংখ্যা ret = _interface->read_word(BATT_SMBUS_VOLTAGE, ফলাফল);

	// Convert millivolts to volts.
	তথ্য.voltage_v = ((দশমিক)ফলাফল) * 0.001f;

	// Read current.
	ret |= _interface->read_word(BATT_SMBUS_CURRENT, ফলাফল);

	তথ্য.current_a = (-1.0f * ((দশমিক)(*(int16_t *)&ফলাফল)) * 0.001f);

	// Read remaining capacity.
	ret |= _interface->read_word(BATT_SMBUS_RELATIVE_SOC, ফলাফল);
	তথ্য.remaining = (দশমিক)ফলাফল * 0.01f;

	// Read remaining capacity.
	ret |= _interface->read_word(BATT_SMBUS_REMAINING_CAPACITY, ফলাফল);
	তথ্য.discharged_mah = _batt_startup_capacity - ফলাফল;

	// Read full capacity.
	ret |= _interface->read_word(BATT_SMBUS_FULL_CHARGE_CAPACITY, ফলাফল);
	তথ্য.capacity = ফলাফল;

	// Read cycle count.
	ret |= _interface->read_word(BATT_SMBUS_CYCLE_গুনতি, ফলাফল);
	তথ্য.cycle_গুনতি_2 = ফলাফল;

	// Read serial number.
	ret |= _interface->read_word(BATT_SMBUS_SERIAL_সংখ্যা, ফলাফল);
	snprintf(battery_info.serial_সংখ্যা_2, আকার(battery_info.serial_সংখ্যা_2), "%" PRIu16, ফলাফল);

	// Read battery temperature and covert to Celsius.
	ret |= _interface->read_word(BATT_SMBUS_অস্থায়ী, ফলাফল);
	তথ্য.temperature = ((দশমিক)ফলাফল * 0.1f) + atmosphere::kAbsoluteNullCelsius;

	ফেরত ret;

}

template<শ্রেণী T>
শূন্য SMBUS_SBS_BaseClass<T>::RunImpl()
{

	// Get the current time.
	uint64_t now = hrt_absolute_time();

	// Read data from sensor.
	battery_status_s new_report = {};

	new_report.id = 1;

	// Set time of reading.
	new_report.timestamp = now;

	new_report.connected = সত্য;

	battery_info_s battery_info{};
	battery_info.timestamp = now;
	battery_info.id = new_report.id;

	পূর্ণসংখ্যা ret = populate_smbus_তথ্য(new_report, battery_info);

	new_report.cell_গুনতি = cell_গুনতি_2;

	// Only publish if no errors.
	যদি (!ret) {
		orb_publish(ORB_ID(battery_status), _batt_topic, &new_report);
		orb_publish(ORB_ID(battery_info), _battery_info_topic, &battery_info);
	}
}
